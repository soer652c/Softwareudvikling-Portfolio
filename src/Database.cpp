#include "Database.h"

#include "Item.h"

#include <iostream>
#include <sqlite3.h>

namespace {
std::string columnText(sqlite3_stmt* statement, int column) {
    const unsigned char* text = sqlite3_column_text(statement, column);
    return text == nullptr ? "" : reinterpret_cast<const char*>(text);
}

bool bindText(sqlite3_stmt* statement, int index, const std::string& value) {
    return sqlite3_bind_text(statement, index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
}
}

Database::Database(const std::string& path) : db_(nullptr) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        return;
    }

    createSchema();
}

Database::~Database() {
    if (db_ != nullptr) {
        sqlite3_close(db_);
    }
}

bool Database::isOpen() const {
    return db_ != nullptr;
}

const std::string& Database::lastError() const {
    return lastError_;
}

std::vector<HeroRecord> Database::listHeroes() const {
    std::vector<HeroRecord> heroes;
    if (!isOpen()) {
        return heroes;
    }

    sqlite3_stmt* statement = nullptr;

    const char* sql = "SELECT id, name FROM heroes ORDER BY name;";
    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return heroes;
    }

    while (sqlite3_step(statement) == SQLITE_ROW) {
        heroes.push_back(HeroRecord{sqlite3_column_int(statement, 0), columnText(statement, 1)});
    }

    sqlite3_finalize(statement);
    return heroes;
}

std::optional<Character> Database::loadCharacter(int heroId) const {
    if (!isOpen()) {
        return std::nullopt;
    }

    sqlite3_stmt* statement = nullptr;
    const char* heroSql = "SELECT name FROM heroes WHERE id = ?;";

    if (sqlite3_prepare_v2(db_, heroSql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return std::nullopt;
    }

    sqlite3_bind_int(statement, 1, heroId);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    Character character(columnText(statement, 0));
    sqlite3_finalize(statement);

    const char* monsterSql =
        "SELECT id, name, hit_points, max_hit_points, strength "
        "FROM monsters WHERE hero_id = ? ORDER BY slot;";

    if (sqlite3_prepare_v2(db_, monsterSql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return std::nullopt;
    }

    sqlite3_bind_int(statement, 1, heroId);

    while (sqlite3_step(statement) == SQLITE_ROW) {
        const int monsterId = sqlite3_column_int(statement, 0);
        Monster monster(
            columnText(statement, 1), sqlite3_column_int(statement, 3), sqlite3_column_int(statement, 4));
        monster.takeDamage(sqlite3_column_int(statement, 3) - sqlite3_column_int(statement, 2));

        sqlite3_stmt* itemStatement = nullptr;
        const char* itemSql = "SELECT item_name FROM monster_items WHERE monster_id = ? ORDER BY id;";

        if (sqlite3_prepare_v2(db_, itemSql, -1, &itemStatement, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(itemStatement, 1, monsterId);
            while (sqlite3_step(itemStatement) == SQLITE_ROW) {
                const std::optional<Item> item = Item::fromName(columnText(itemStatement, 0));
                if (item) {
                    monster.addItem(*item);
                }
            }
        }

        sqlite3_finalize(itemStatement);
        character.addMonster(monster);
    }

    sqlite3_finalize(statement);

    const char* totalSql = "SELECT total_defeated FROM stats WHERE hero_id = ?;";
    if (sqlite3_prepare_v2(db_, totalSql, -1, &statement, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(statement, 1, heroId);
        if (sqlite3_step(statement) == SQLITE_ROW) {
            character.stats().totalDefeatedMonsters = sqlite3_column_int(statement, 0);
        }
    }
    sqlite3_finalize(statement);

    const char* mapQueries[] = {
        "SELECT item_name, count FROM item_uses WHERE hero_id = ?;",
        "SELECT item_name, count FROM item_defeats WHERE hero_id = ?;",
        "SELECT monster_name, count FROM monster_uses WHERE hero_id = ?;",
    };

    for (int mapIndex = 0; mapIndex < 3; ++mapIndex) {
        if (sqlite3_prepare_v2(db_, mapQueries[mapIndex], -1, &statement, nullptr) != SQLITE_OK) {
            continue;
        }

        sqlite3_bind_int(statement, 1, heroId);
        while (sqlite3_step(statement) == SQLITE_ROW) {
            std::map<std::string, int>* target = &character.stats().itemUses;
            if (mapIndex == 1) {
                target = &character.stats().itemDefeats;
            } else if (mapIndex == 2) {
                target = &character.stats().monsterUses;
            }

            (*target)[columnText(statement, 0)] = sqlite3_column_int(statement, 1);
        }
        sqlite3_finalize(statement);
    }

    return character;
}

bool Database::saveCharacter(const Character& character) {
    if (!isOpen()) {
        return false;
    }

    if (!execute("BEGIN TRANSACTION;")) {
        return false;
    }

    std::optional<int> heroId = findHeroIdByName(character.name());

    if (!heroId) {
        sqlite3_stmt* statement = nullptr;
        const char* sql = "INSERT INTO heroes(name) VALUES(?);";

        if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK ||
            !bindText(statement, 1, character.name()) ||
            sqlite3_step(statement) != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            sqlite3_finalize(statement);
            execute("ROLLBACK;");
            return false;
        }

        sqlite3_finalize(statement);
        heroId = static_cast<int>(sqlite3_last_insert_rowid(db_));
    }

    const std::string heroIdText = std::to_string(*heroId);
    if (!execute("DELETE FROM monster_items WHERE monster_id IN "
                 "(SELECT id FROM monsters WHERE hero_id = " + heroIdText + ");") ||
        !execute("DELETE FROM monsters WHERE hero_id = " + heroIdText + ";") ||
        !execute("DELETE FROM stats WHERE hero_id = " + heroIdText + ";") ||
        !execute("DELETE FROM item_uses WHERE hero_id = " + heroIdText + ";") ||
        !execute("DELETE FROM item_defeats WHERE hero_id = " + heroIdText + ";") ||
        !execute("DELETE FROM monster_uses WHERE hero_id = " + heroIdText + ";")) {
        execute("ROLLBACK;");
        return false;
    }

    if (!saveMonsters(*heroId, character) || !saveStats(*heroId, character.stats())) {
        execute("ROLLBACK;");
        return false;
    }

    return execute("COMMIT;");
}

void Database::printStatistics(std::ostream& output) const {
    const auto heroes = listHeroes();

    if (heroes.empty()) {
        output << "Ingen gemte helte endnu.\n";
        return;
    }

    for (const HeroRecord& hero : heroes) {
        const std::optional<Character> character = loadCharacter(hero.id);
        if (!character) {
            continue;
        }

        output << "\n" << character->name() << "\n";
        output << "Besejrede monstre i alt: " << character->stats().totalDefeatedMonsters << "\n";

        output << "Besejrede monstre per ting:\n";
        for (const auto& entry : character->stats().itemDefeats) {
            output << "  " << entry.first << ": " << entry.second << "\n";
        }

        std::string mostUsedItem = "Ingen";
        int mostUsedItemCount = 0;
        for (const auto& entry : character->stats().itemUses) {
            if (entry.second > mostUsedItemCount) {
                mostUsedItem = entry.first;
                mostUsedItemCount = entry.second;
            }
        }
        output << "Mest brugte ting: " << mostUsedItem << " (" << mostUsedItemCount << ")\n";

        std::string mostUsedMonster = "Ingen";
        int mostUsedMonsterCount = 0;
        for (const auto& entry : character->stats().monsterUses) {
            if (entry.second > mostUsedMonsterCount) {
                mostUsedMonster = entry.first;
                mostUsedMonsterCount = entry.second;
            }
        }
        output << "Mest brugte monster: " << mostUsedMonster << " (" << mostUsedMonsterCount << ")\n";
    }
}

bool Database::execute(const std::string& sql) const {
    if (!isOpen()) {
        return false;
    }

    char* errorMessage = nullptr;
    const int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK) {
        lastError_ = errorMessage == nullptr ? sqlite3_errmsg(db_) : errorMessage;
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

bool Database::createSchema() {
    return execute(
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS heroes("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE"
        ");"
        "CREATE TABLE IF NOT EXISTS monsters("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "hero_id INTEGER NOT NULL,"
        "slot INTEGER NOT NULL,"
        "name TEXT NOT NULL,"
        "hit_points INTEGER NOT NULL,"
        "max_hit_points INTEGER NOT NULL,"
        "strength INTEGER NOT NULL,"
        "FOREIGN KEY(hero_id) REFERENCES heroes(id) ON DELETE CASCADE"
        ");"
        "CREATE TABLE IF NOT EXISTS monster_items("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "monster_id INTEGER NOT NULL,"
        "item_name TEXT NOT NULL,"
        "FOREIGN KEY(monster_id) REFERENCES monsters(id) ON DELETE CASCADE"
        ");"
        "CREATE TABLE IF NOT EXISTS stats("
        "hero_id INTEGER PRIMARY KEY,"
        "total_defeated INTEGER NOT NULL,"
        "FOREIGN KEY(hero_id) REFERENCES heroes(id) ON DELETE CASCADE"
        ");"
        "CREATE TABLE IF NOT EXISTS item_uses("
        "hero_id INTEGER NOT NULL,"
        "item_name TEXT NOT NULL,"
        "count INTEGER NOT NULL,"
        "PRIMARY KEY(hero_id, item_name)"
        ");"
        "CREATE TABLE IF NOT EXISTS item_defeats("
        "hero_id INTEGER NOT NULL,"
        "item_name TEXT NOT NULL,"
        "count INTEGER NOT NULL,"
        "PRIMARY KEY(hero_id, item_name)"
        ");"
        "CREATE TABLE IF NOT EXISTS monster_uses("
        "hero_id INTEGER NOT NULL,"
        "monster_name TEXT NOT NULL,"
        "count INTEGER NOT NULL,"
        "PRIMARY KEY(hero_id, monster_name)"
        ");");
}

std::optional<int> Database::findHeroIdByName(const std::string& name) const {
    sqlite3_stmt* statement = nullptr;
    const char* sql = "SELECT id FROM heroes WHERE name = ?;";

    if (sqlite3_prepare_v2(db_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return std::nullopt;
    }

    bindText(statement, 1, name);
    const bool found = sqlite3_step(statement) == SQLITE_ROW;
    const int id = found ? sqlite3_column_int(statement, 0) : 0;
    sqlite3_finalize(statement);

    if (!found) {
        return std::nullopt;
    }

    return id;
}

bool Database::saveMonsters(int heroId, const Character& character) {
    sqlite3_stmt* monsterStatement = nullptr;
    const char* monsterSql =
        "INSERT INTO monsters(hero_id, slot, name, hit_points, max_hit_points, strength) "
        "VALUES(?, ?, ?, ?, ?, ?);";

    for (std::size_t index = 0; index < character.monsters().size(); ++index) {
        const Monster& monster = character.monsters()[index];

        if (sqlite3_prepare_v2(db_, monsterSql, -1, &monsterStatement, nullptr) != SQLITE_OK) {
            lastError_ = sqlite3_errmsg(db_);
            return false;
        }

        sqlite3_bind_int(monsterStatement, 1, heroId);
        sqlite3_bind_int(monsterStatement, 2, static_cast<int>(index));
        bindText(monsterStatement, 3, monster.name());
        sqlite3_bind_int(monsterStatement, 4, monster.hitPoints());
        sqlite3_bind_int(monsterStatement, 5, monster.maxHitPoints());
        sqlite3_bind_int(monsterStatement, 6, monster.strength());

        if (sqlite3_step(monsterStatement) != SQLITE_DONE) {
            lastError_ = sqlite3_errmsg(db_);
            sqlite3_finalize(monsterStatement);
            return false;
        }

        sqlite3_finalize(monsterStatement);
        const int monsterId = static_cast<int>(sqlite3_last_insert_rowid(db_));

        for (const Item& item : monster.items()) {
            sqlite3_stmt* itemStatement = nullptr;
            const char* itemSql = "INSERT INTO monster_items(monster_id, item_name) VALUES(?, ?);";

            if (sqlite3_prepare_v2(db_, itemSql, -1, &itemStatement, nullptr) != SQLITE_OK) {
                lastError_ = sqlite3_errmsg(db_);
                return false;
            }

            sqlite3_bind_int(itemStatement, 1, monsterId);
            bindText(itemStatement, 2, item.name());

            if (sqlite3_step(itemStatement) != SQLITE_DONE) {
                lastError_ = sqlite3_errmsg(db_);
                sqlite3_finalize(itemStatement);
                return false;
            }

            sqlite3_finalize(itemStatement);
        }
    }

    return true;
}

bool Database::saveStats(int heroId, const GameStats& stats) {
    sqlite3_stmt* statement = nullptr;
    const char* totalSql = "INSERT INTO stats(hero_id, total_defeated) VALUES(?, ?);";

    if (sqlite3_prepare_v2(db_, totalSql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }

    sqlite3_bind_int(statement, 1, heroId);
    sqlite3_bind_int(statement, 2, stats.totalDefeatedMonsters);

    if (sqlite3_step(statement) != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        sqlite3_finalize(statement);
        return false;
    }

    sqlite3_finalize(statement);

    const struct {
        const char* sql;
        const std::map<std::string, int>& values;
    } tables[] = {
        {"INSERT INTO item_uses(hero_id, item_name, count) VALUES(?, ?, ?);", stats.itemUses},
        {"INSERT INTO item_defeats(hero_id, item_name, count) VALUES(?, ?, ?);", stats.itemDefeats},
        {"INSERT INTO monster_uses(hero_id, monster_name, count) VALUES(?, ?, ?);", stats.monsterUses},
    };

    for (const auto& table : tables) {
        for (const auto& entry : table.values) {
            if (sqlite3_prepare_v2(db_, table.sql, -1, &statement, nullptr) != SQLITE_OK) {
                lastError_ = sqlite3_errmsg(db_);
                return false;
            }

            sqlite3_bind_int(statement, 1, heroId);
            bindText(statement, 2, entry.first);
            sqlite3_bind_int(statement, 3, entry.second);

            if (sqlite3_step(statement) != SQLITE_DONE) {
                lastError_ = sqlite3_errmsg(db_);
                sqlite3_finalize(statement);
                return false;
            }

            sqlite3_finalize(statement);
        }
    }

    return true;
}

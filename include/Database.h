#ifndef DATABASE_H
#define DATABASE_H

#include "Character.h"

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

struct sqlite3;

struct HeroRecord {
    int id;
    std::string name;
};

class Database {
public:
    explicit Database(const std::string& path);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool isOpen() const;
    const std::string& lastError() const;

    std::vector<HeroRecord> listHeroes() const;
    std::optional<Character> loadCharacter(int heroId) const;
    bool saveCharacter(const Character& character);
    void printStatistics(std::ostream& output) const;

private:
    bool execute(const std::string& sql) const;
    bool createSchema();
    std::optional<int> findHeroIdByName(const std::string& name) const;
    bool saveMonsters(int heroId, const Character& character);
    bool saveStats(int heroId, const GameStats& stats);

    sqlite3* db_;
    mutable std::string lastError_;
};

#endif

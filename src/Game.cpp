#include "Game.h"

#include "Battle.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

Game::Game(std::istream& input, std::ostream& output)
    : output_(output),
      input_(input, output),
      randomEngine_(static_cast<std::mt19937::result_type>(
          std::chrono::steady_clock::now().time_since_epoch().count())) {}

void Game::run() {
    bool running = true;

    output_ << "Monster Adventure\n";

    while (running) {
        showMainMenu();
        const int choice = input_.readIntInRange("Vaelg: ", 1, 4);

        switch (choice) {
        case 1:
            createNewCharacter();
            break;
        case 2:
            startAdventure();
            break;
        case 3:
            listCharacterMonsters();
            input_.waitForEnter();
            break;
        case 4:
            running = false;
            output_ << "Farvel.\n";
            break;
        default:
            break;
        }
    }
}

void Game::showMainMenu() {
    output_ << "\nHovedmenu\n"
            << "1. Lav ny karakter\n"
            << "2. Start eventyr\n"
            << "3. Vis karakter\n"
            << "4. Luk spillet\n";
}

void Game::createNewCharacter() {
    const std::string name = input_.readNonEmptyLine("Karakterens navn: ");
    Character newCharacter(name);

    newCharacter.addMonster(catalog_.starterHorse());
    newCharacter.addMonster(catalog_.starterHorse());

    character_ = newCharacter;

    output_ << character_->name() << " er oprettet med to Hest.\n";
}

void Game::startAdventure() {
    if (!character_) {
        output_ << "Lav en karakter foerst.\n";
        input_.waitForEnter();
        return;
    }

    if (!character_->hasLivingMonsters()) {
        output_ << "Alle dine monstre er besejret. Lav en ny karakter for at proeve igen.\n";
        input_.waitForEnter();
        return;
    }

    bool adventuring = true;

    while (adventuring && character_->hasLivingMonsters()) {
        output_ << "\nEventyr\n"
                << "1. Kaemp mod et monster\n"
                << "2. Kaemp mod en grotte\n"
                << "3. Tilbage til hovedmenu\n";
        const int choice = input_.readIntInRange("Vaelg: ", 1, 3);

        if (choice == 1) {
            startSingleMonsterFight();
        } else if (choice == 2) {
            startCaveFight();
        } else {
            adventuring = false;
        }
    }

    input_.waitForEnter();
}

bool Game::fightEnemy(Monster& enemy) {
    Battle battle(randomEngine_);
    bool enemyDefeated = false;

    while (!enemy.isDefeated() && character_->hasLivingMonsters()) {
        const std::size_t playerMonsterIndex = chooseLivingMonsterIndex();
        Monster& playerMonster = character_->monsters().at(playerMonsterIndex);
        const BattleResult result = battle.fight(playerMonster, enemy, input_, output_);

        if (result.playerMonsterWon) {
            enemyDefeated = true;
        } else if (character_->hasLivingMonsters()) {
            output_ << "Du har stadig monstre tilbage. Send et nyt ind.\n";
        }
    }

    if (!enemyDefeated && !character_->hasLivingMonsters()) {
        output_ << "Alle dine monstre er besejret. Du vender tilbage til hovedmenuen.\n";
    }

    return enemyDefeated;
}

void Game::startSingleMonsterFight() {
    const int enemyIndex = chooseEnemyIndex();
    Monster enemy = catalog_.monsters().at(static_cast<std::size_t>(enemyIndex));

    if (fightEnemy(enemy)) {
        Monster captured = enemy;
        captured.healToFull();
        offerCapturedMonster(captured);
    }
}

void Game::startCaveFight() {
    std::vector<Cave> caves;

    for (int caveNumber = 1; caveNumber <= 3; ++caveNumber) {
        caves.push_back(Cave::generateForCharacter(*character_, catalog_, caveNumber, randomEngine_));
    }

    Cave cave = caves.at(static_cast<std::size_t>(chooseCaveIndex(caves)));
    output_ << "\nDu gaar ind i " << cave.name() << ". Alle monstre skal besejres.\n";

    while (cave.hasLivingMonsters() && character_->hasLivingMonsters()) {
        const std::size_t enemyIndex = cave.livingMonsterIndexes().front();
        Monster& enemy = cave.monsters().at(enemyIndex);
        output_ << "\nNaeste grottemonster: " << enemy.summary() << "\n";

        if (!fightEnemy(enemy)) {
            return;
        }
    }

    output_ << cave.name() << " er gennemfoert.\n";
    giveItemToMonster(randomRewardItem());
}

void Game::listCharacterMonsters() const {
    if (!character_) {
        output_ << "Der er ikke oprettet en karakter endnu.\n";
        return;
    }

    output_ << "\nKarakter: " << character_->name() << "\n";
    const auto& monsters = character_->monsters();

    for (std::size_t index = 0; index < monsters.size(); ++index) {
        output_ << index + 1 << ". " << monsters[index].summary();
        if (monsters[index].isDefeated()) {
            output_ << " [besejret]";
        }
        output_ << "\n";

        const auto& items = monsters[index].items();
        for (std::size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex) {
            output_ << "   Ting " << itemIndex + 1 << ": " << items[itemIndex].summary() << "\n";
        }
    }
}

int Game::chooseEnemyIndex() {
    output_ << "\nVaelg fjende\n";
    const auto& monsters = catalog_.monsters();

    for (std::size_t index = 0; index < monsters.size(); ++index) {
        output_ << index + 1 << ". " << monsters[index].summary() << "\n";
    }

    return input_.readIntInRange("Fjende: ", 1, static_cast<int>(monsters.size())) - 1;
}

int Game::chooseCaveIndex(const std::vector<Cave>& caves) {
    output_ << "\nVaelg grotte\n";

    for (std::size_t index = 0; index < caves.size(); ++index) {
        output_ << index + 1 << ". " << caves[index].summary() << "\n";
        const auto& monsters = caves[index].monsters();
        for (const Monster& monster : monsters) {
            output_ << "   - " << monster.summary() << "\n";
        }
    }

    return input_.readIntInRange("Grotte: ", 1, static_cast<int>(caves.size())) - 1;
}

std::size_t Game::chooseLivingMonsterIndex() {
    output_ << "\nVaelg dit monster\n";
    const auto livingIndexes = character_->livingMonsterIndexes();

    for (std::size_t listIndex = 0; listIndex < livingIndexes.size(); ++listIndex) {
        const std::size_t monsterIndex = livingIndexes[listIndex];
        output_ << listIndex + 1 << ". "
                << character_->monsters().at(monsterIndex).summary() << "\n";
    }

    const int choice =
        input_.readIntInRange("Dit monster: ", 1, static_cast<int>(livingIndexes.size()));
    return livingIndexes.at(static_cast<std::size_t>(choice - 1));
}

void Game::offerCapturedMonster(const Monster& monster) {
    output_ << "Du besejrede " << monster.name() << ".\n";

    if (!input_.readYesNo("Vil du beholde monsteret")) {
        output_ << monster.name() << " blev efterladt.\n";
        return;
    }

    if (character_->addMonster(monster)) {
        output_ << monster.name() << " er nu dit monster.\n";
        return;
    }

    output_ << "Du har allerede fire monstre.\n";

    if (!input_.readYesNo("Vil du udskifte et af dem")) {
        output_ << monster.name() << " blev efterladt.\n";
        return;
    }

    listCharacterMonsters();
    const int replaceChoice = input_.readIntInRange(
        "Hvilket monster skal udskiftes: ", 1, static_cast<int>(character_->monsters().size()));

    character_->replaceMonster(static_cast<std::size_t>(replaceChoice - 1), monster);
    output_ << monster.name() << " er tilfoejet.\n";
}

Item Game::randomRewardItem() {
    const std::vector<Item> rewards{
        Item::bomb(),
        Item::fireBomb(),
        Item::thunderBomb(),
        Item::club(),
        Item::blaster(),
        Item::curse(),
        Item::poison(),
    };

    std::uniform_int_distribution<std::size_t> roll(0, rewards.size() - 1);
    return rewards[roll(randomEngine_)];
}

void Game::giveItemToMonster(const Item& item) {
    output_ << "Du fandt " << item.summary() << ". Vaelg hvilket monster der skal have den.\n";

    const auto& monsters = character_->monsters();
    for (std::size_t index = 0; index < monsters.size(); ++index) {
        output_ << index + 1 << ". " << monsters[index].summary() << "\n";
    }

    const int choice =
        input_.readIntInRange("Monster: ", 1, static_cast<int>(monsters.size()));
    character_->monsters().at(static_cast<std::size_t>(choice - 1)).addItem(item);

    output_ << item.name() << " blev givet til "
            << character_->monsters().at(static_cast<std::size_t>(choice - 1)).name() << ".\n";
}

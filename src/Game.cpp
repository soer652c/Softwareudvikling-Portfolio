#include "Game.h"

#include "Battle.h"

#include <chrono>
#include <iostream>
#include <string>

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
                << "2. Tilbage til hovedmenu\n";
        const int choice = input_.readIntInRange("Vaelg: ", 1, 2);

        if (choice == 2) {
            adventuring = false;
            continue;
        }

        const int enemyIndex = chooseEnemyIndex();
        Monster enemy = catalog_.monsters().at(static_cast<std::size_t>(enemyIndex));

        Battle battle(randomEngine_);
        bool enemyDefeated = false;

        while (!enemy.isDefeated() && character_->hasLivingMonsters()) {
            const std::size_t playerMonsterIndex = chooseLivingMonsterIndex();
            Monster& playerMonster = character_->monsters().at(playerMonsterIndex);
            const BattleResult result = battle.fight(playerMonster, enemy, output_);

            if (result.playerMonsterWon) {
                enemyDefeated = true;
            } else if (character_->hasLivingMonsters()) {
                output_ << "Du har stadig monstre tilbage. Send et nyt ind.\n";
            }
        }

        if (enemyDefeated) {
            Monster captured = enemy;
            captured.healToFull();
            offerCapturedMonster(captured);
        } else if (!character_->hasLivingMonsters()) {
            output_ << "Alle dine monstre er besejret. Du vender tilbage til hovedmenuen.\n";
            adventuring = false;
        }
    }

    input_.waitForEnter();
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

#include "Battle.h"

#include "Input.h"

#include <iostream>

Battle::Battle(std::mt19937& randomEngine) : randomEngine_(randomEngine) {}

BattleResult Battle::fight(
    Monster& playerMonster, Monster& enemyMonster, Input& input, std::ostream& output) {
    std::bernoulli_distribution startsFirst(0.5);
    bool playerTurn = startsFirst(randomEngine_);
    std::vector<std::string> usedItemNames;
    std::optional<std::string> defeatingItemName;

    output << "\nKamp starter: " << playerMonster.summary() << " mod "
           << enemyMonster.summary() << "\n";
    output << (playerTurn ? playerMonster.name() : enemyMonster.name())
           << " angriber foerst.\n";

    while (!playerMonster.isDefeated() && !enemyMonster.isDefeated()) {
        Monster& activeMonster = playerTurn ? playerMonster : enemyMonster;

        const bool canAct = activeMonster.applyStartOfTurn(output, randomEngine_);
        if (activeMonster.isDefeated()) {
            break;
        }

        if (canAct) {
            if (playerTurn) {
                const std::optional<std::string> usedItem =
                    takePlayerTurn(playerMonster, enemyMonster, input, output);
                if (usedItem) {
                    usedItemNames.push_back(*usedItem);
                    if (enemyMonster.isDefeated()) {
                        defeatingItemName = usedItem;
                    }
                }
            } else {
                takeEnemyTurn(enemyMonster, playerMonster, output);
            }
        }

        activeMonster.finishTurn();
        playerTurn = !playerTurn;
    }

    const bool playerWon = !playerMonster.isDefeated();
    output << (playerWon ? playerMonster.name() : enemyMonster.name()) << " vandt kampen.\n";

    playerMonster.clearStatuses();
    enemyMonster.clearStatuses();

    return BattleResult{playerWon, usedItemNames, defeatingItemName};
}

std::optional<std::string> Battle::takePlayerTurn(
    Monster& playerMonster, Monster& enemyMonster, Input& input, std::ostream& output) {
    output << "\nTur for " << playerMonster.name() << "\n"
           << "1. Angrib\n";

    const auto& items = playerMonster.items();
    for (std::size_t index = 0; index < items.size(); ++index) {
        output << index + 2 << ". Brug " << items[index].summary() << "\n";
    }

    const int maxChoice = static_cast<int>(items.size()) + 1;
    const int choice = input.readIntInRange("Vaelg handling: ", 1, maxChoice);

    if (choice == 1) {
        basicAttack(playerMonster, enemyMonster, output);
        return std::nullopt;
    }

    const Item& item = items[static_cast<std::size_t>(choice - 2)];
    item.use(playerMonster, enemyMonster, output, randomEngine_);
    if (item.dealsDamage()) {
        playerMonster.applyAfterDealingDamage(output);
    }
    return item.name();
}

void Battle::takeEnemyTurn(
    Monster& enemyMonster, Monster& playerMonster, std::ostream& output) {
    basicAttack(enemyMonster, playerMonster, output);
}

void Battle::basicAttack(Monster& attacker, Monster& defender, std::ostream& output) {
    defender.takeDamage(attacker.strength());
    output << attacker.name() << " rammer " << defender.name() << " for "
           << attacker.strength() << " skade. " << defender.name()
           << " har " << defender.hitPoints() << " hp tilbage.\n";
    attacker.applyAfterDealingDamage(output);
}

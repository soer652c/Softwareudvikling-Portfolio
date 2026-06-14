#include "Battle.h"

#include <iostream>

Battle::Battle(std::mt19937& randomEngine) : randomEngine_(randomEngine) {}

BattleResult Battle::fight(Monster& playerMonster, Monster& enemyMonster, std::ostream& output) {
    std::bernoulli_distribution startsFirst(0.5);
    bool playerTurn = startsFirst(randomEngine_);

    output << "\nKamp starter: " << playerMonster.summary() << " mod "
           << enemyMonster.summary() << "\n";
    output << (playerTurn ? playerMonster.name() : enemyMonster.name())
           << " angriber foerst.\n";

    while (!playerMonster.isDefeated() && !enemyMonster.isDefeated()) {
        Monster& attacker = playerTurn ? playerMonster : enemyMonster;
        Monster& defender = playerTurn ? enemyMonster : playerMonster;

        defender.takeDamage(attacker.strength());
        output << attacker.name() << " rammer " << defender.name() << " for "
               << attacker.strength() << " skade. " << defender.name()
               << " har " << defender.hitPoints() << " hp tilbage.\n";

        playerTurn = !playerTurn;
    }

    const bool playerWon = !playerMonster.isDefeated();
    output << (playerWon ? playerMonster.name() : enemyMonster.name()) << " vandt kampen.\n";
    return BattleResult{playerWon};
}

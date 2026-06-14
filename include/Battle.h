#ifndef BATTLE_H
#define BATTLE_H

#include "Monster.h"

#include <iosfwd>
#include <optional>
#include <random>
#include <string>
#include <vector>

class Input;

struct BattleResult {
    bool playerMonsterWon;
    std::vector<std::string> usedItemNames;
    std::optional<std::string> defeatingItemName;
};

class Battle {
public:
    explicit Battle(std::mt19937& randomEngine);

    BattleResult fight(
        Monster& playerMonster, Monster& enemyMonster, Input& input, std::ostream& output);

private:
    std::optional<std::string> takePlayerTurn(
        Monster& playerMonster, Monster& enemyMonster, Input& input, std::ostream& output);
    void takeEnemyTurn(Monster& enemyMonster, Monster& playerMonster, std::ostream& output);
    void basicAttack(Monster& attacker, Monster& defender, std::ostream& output);

    std::mt19937& randomEngine_;
};

#endif

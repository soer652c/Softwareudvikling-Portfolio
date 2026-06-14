#ifndef BATTLE_H
#define BATTLE_H

#include "Monster.h"

#include <iosfwd>
#include <random>

struct BattleResult {
    bool playerMonsterWon;
};

class Battle {
public:
    explicit Battle(std::mt19937& randomEngine);

    BattleResult fight(Monster& playerMonster, Monster& enemyMonster, std::ostream& output);

private:
    std::mt19937& randomEngine_;
};

#endif

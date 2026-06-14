#ifndef CHARACTER_H
#define CHARACTER_H

#include "Monster.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

struct GameStats {
    int totalDefeatedMonsters = 0;
    std::map<std::string, int> itemUses;
    std::map<std::string, int> itemDefeats;
    std::map<std::string, int> monsterUses;
};

class Character {
public:
    static constexpr std::size_t maxMonsterCount = 4;

    explicit Character(std::string name);

    const std::string& name() const;
    const std::vector<Monster>& monsters() const;
    std::vector<Monster>& monsters();
    const GameStats& stats() const;
    GameStats& stats();

    bool addMonster(const Monster& monster);
    bool replaceMonster(std::size_t index, const Monster& monster);
    bool hasLivingMonsters() const;
    std::vector<std::size_t> livingMonsterIndexes() const;

private:
    std::string name_;
    std::vector<Monster> monsters_;
    GameStats stats_;
};

#endif

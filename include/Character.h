#ifndef CHARACTER_H
#define CHARACTER_H

#include "Monster.h"

#include <cstddef>
#include <string>
#include <vector>

class Character {
public:
    static constexpr std::size_t maxMonsterCount = 4;

    explicit Character(std::string name);

    const std::string& name() const;
    const std::vector<Monster>& monsters() const;
    std::vector<Monster>& monsters();

    bool addMonster(const Monster& monster);
    bool replaceMonster(std::size_t index, const Monster& monster);
    bool hasLivingMonsters() const;
    std::vector<std::size_t> livingMonsterIndexes() const;

private:
    std::string name_;
    std::vector<Monster> monsters_;
};

#endif

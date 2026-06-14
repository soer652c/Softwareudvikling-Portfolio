#ifndef CAVE_H
#define CAVE_H

#include "Monster.h"

#include <cstddef>
#include <random>
#include <string>
#include <vector>

class Character;
class MonsterCatalog;

class Cave {
public:
    Cave(std::string name, std::vector<Monster> monsters);

    const std::string& name() const;
    const std::vector<Monster>& monsters() const;
    std::vector<Monster>& monsters();

    bool hasLivingMonsters() const;
    std::vector<std::size_t> livingMonsterIndexes() const;
    std::string summary() const;

    static Cave generateForCharacter(
        const Character& character,
        const MonsterCatalog& catalog,
        int caveNumber,
        std::mt19937& randomEngine);

private:
    std::string name_;
    std::vector<Monster> monsters_;
};

#endif

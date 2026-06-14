#ifndef MONSTER_CATALOG_H
#define MONSTER_CATALOG_H

#include "Monster.h"

#include <vector>

class MonsterCatalog {
public:
    MonsterCatalog();

    const std::vector<Monster>& monsters() const;
    Monster starterHorse() const;

private:
    std::vector<Monster> monsters_;
};

#endif

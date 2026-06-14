#include "Character.h"

#include <algorithm>
#include <utility>

Character::Character(std::string name) : name_(std::move(name)) {}

const std::string& Character::name() const {
    return name_;
}

const std::vector<Monster>& Character::monsters() const {
    return monsters_;
}

std::vector<Monster>& Character::monsters() {
    return monsters_;
}

bool Character::addMonster(const Monster& monster) {
    if (monsters_.size() >= maxMonsterCount) {
        return false;
    }

    monsters_.push_back(monster);
    return true;
}

bool Character::replaceMonster(std::size_t index, const Monster& monster) {
    if (index >= monsters_.size()) {
        return false;
    }

    monsters_[index] = monster;
    return true;
}

bool Character::hasLivingMonsters() const {
    return std::any_of(monsters_.begin(), monsters_.end(), [](const Monster& monster) {
        return !monster.isDefeated();
    });
}

std::vector<std::size_t> Character::livingMonsterIndexes() const {
    std::vector<std::size_t> indexes;

    for (std::size_t index = 0; index < monsters_.size(); ++index) {
        if (!monsters_[index].isDefeated()) {
            indexes.push_back(index);
        }
    }

    return indexes;
}

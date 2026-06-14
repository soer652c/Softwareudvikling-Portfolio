#include "Cave.h"

#include "Character.h"
#include "MonsterCatalog.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace {
std::size_t highestAllowedMonsterIndex(
    const Character& character, const MonsterCatalog& catalog, int caveNumber) {
    const std::size_t playerMonsterCount = character.monsters().size();
    const std::size_t wantedIndex = playerMonsterCount + static_cast<std::size_t>(caveNumber);
    return std::min(wantedIndex, catalog.monsters().size() - 1);
}
}

Cave::Cave(std::string name, std::vector<Monster> monsters)
    : name_(std::move(name)), monsters_(std::move(monsters)) {}

const std::string& Cave::name() const {
    return name_;
}

const std::vector<Monster>& Cave::monsters() const {
    return monsters_;
}

std::vector<Monster>& Cave::monsters() {
    return monsters_;
}

bool Cave::hasLivingMonsters() const {
    return std::any_of(monsters_.begin(), monsters_.end(), [](const Monster& monster) {
        return !monster.isDefeated();
    });
}

std::vector<std::size_t> Cave::livingMonsterIndexes() const {
    std::vector<std::size_t> indexes;

    for (std::size_t index = 0; index < monsters_.size(); ++index) {
        if (!monsters_[index].isDefeated()) {
            indexes.push_back(index);
        }
    }

    return indexes;
}

std::string Cave::summary() const {
    std::ostringstream output;
    output << name_ << " med " << monsters_.size() << " monstre";
    return output.str();
}

Cave Cave::generateForCharacter(
    const Character& character,
    const MonsterCatalog& catalog,
    int caveNumber,
    std::mt19937& randomEngine) {
    const auto& catalogMonsters = catalog.monsters();
    const std::size_t highestIndex = highestAllowedMonsterIndex(character, catalog, caveNumber);
    std::uniform_int_distribution<std::size_t> monsterRoll(0, highestIndex);
    const int monsterCount = std::clamp(2 + caveNumber, 2, 5);
    std::vector<Monster> monsters;

    for (int index = 0; index < monsterCount; ++index) {
        monsters.push_back(catalogMonsters[monsterRoll(randomEngine)]);
    }

    return Cave("Grotte " + std::to_string(caveNumber), monsters);
}

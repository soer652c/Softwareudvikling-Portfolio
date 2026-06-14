#include "Cave.h"

#include "Character.h"
#include "MonsterCatalog.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <utility>

namespace {
int monsterScore(const Monster& monster) {
    return monster.maxHitPoints() + monster.strength() * 3;
}

int characterLevelScore(const Character& character) {
    const auto& monsters = character.monsters();

    if (monsters.empty()) {
        return 5;
    }

    const int total = std::accumulate(monsters.begin(), monsters.end(), 0, [](int sum, const Monster& monster) {
        return sum + monsterScore(monster);
    });

    return std::max(5, total / static_cast<int>(monsters.size()));
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
    const int playerScore = characterLevelScore(character);
    std::vector<Monster> candidates;

    for (const Monster& monster : catalog.monsters()) {
        if (monsterScore(monster) <= playerScore + caveNumber * 8) {
            candidates.push_back(monster);
        }
    }

    if (candidates.empty()) {
        candidates.push_back(catalog.starterHorse());
    }

    std::uniform_int_distribution<std::size_t> monsterRoll(0, candidates.size() - 1);
    const int monsterCount = std::clamp(2 + caveNumber, 2, 5);
    std::vector<Monster> monsters;

    for (int index = 0; index < monsterCount; ++index) {
        monsters.push_back(candidates[monsterRoll(randomEngine)]);
    }

    return Cave("Grotte " + std::to_string(caveNumber), monsters);
}

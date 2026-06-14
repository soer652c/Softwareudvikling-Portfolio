#include "StatusEffect.h"

#include "Monster.h"

#include <algorithm>
#include <iostream>

namespace {
std::string statusName(StatusType type) {
    switch (type) {
    case StatusType::Poisoned:
        return "Poisoned";
    case StatusType::Frozen:
        return "Frozen";
    case StatusType::Paralyzed:
        return "Paralyzed";
    case StatusType::Stunned:
        return "Stunned";
    case StatusType::Cursed:
        return "Cursed";
    }

    return "Unknown";
}
}

StatusEffect::StatusEffect(StatusType type, int remainingTurns)
    : type_(type),
      remainingTurns_(std::max(1, remainingTurns)),
      name_(statusName(type)) {}

StatusType StatusEffect::type() const {
    return type_;
}

const std::string& StatusEffect::name() const {
    return name_;
}

int StatusEffect::remainingTurns() const {
    return remainingTurns_;
}

bool StatusEffect::isExpired() const {
    return remainingTurns_ <= 0;
}

bool StatusEffect::applyStartOfTurn(
    Monster& monster, std::ostream& output, std::mt19937& randomEngine) {
    bool canAct = true;

    switch (type_) {
    case StatusType::Poisoned:
        monster.takeDamage(2);
        output << monster.name() << " tager 2 skade af Poisoned.\n";
        break;
    case StatusType::Frozen:
        output << monster.name() << " er Frozen og kan ikke handle.\n";
        canAct = false;
        break;
    case StatusType::Paralyzed: {
        std::bernoulli_distribution losesTurn(0.30);
        if (losesTurn(randomEngine)) {
            output << monster.name() << " er Paralyzed og mister sin tur.\n";
            canAct = false;
        }
        break;
    }
    case StatusType::Stunned:
        output << monster.name() << " er Stunned og mister sin tur.\n";
        canAct = false;
        break;
    case StatusType::Cursed:
        output << monster.name() << " er Cursed.\n";
        break;
    }

    --remainingTurns_;
    return canAct;
}

void StatusEffect::applyAfterDealingDamage(Monster& monster, std::ostream& output) const {
    if (type_ != StatusType::Cursed) {
        return;
    }

    monster.takeDamage(2);
    output << monster.name() << " tager 2 skade af Cursed efter sit angreb.\n";
}

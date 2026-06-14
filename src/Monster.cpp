#include "Monster.h"

#include <algorithm>
#include <sstream>
#include <utility>

Monster::Monster(std::string name, int hitPoints, int strength)
    : name_(std::move(name)),
      hitPoints_(std::max(1, hitPoints)),
      maxHitPoints_(std::max(1, hitPoints)),
      strength_(std::max(0, strength)) {}

const std::string& Monster::name() const {
    return name_;
}

int Monster::hitPoints() const {
    return hitPoints_;
}

int Monster::maxHitPoints() const {
    return maxHitPoints_;
}

int Monster::strength() const {
    return strength_;
}

bool Monster::isDefeated() const {
    return hitPoints_ <= 0;
}

void Monster::takeDamage(int damage) {
    hitPoints_ = std::max(0, hitPoints_ - std::max(0, damage));
}

void Monster::healToFull() {
    hitPoints_ = maxHitPoints_;
}

std::string Monster::summary() const {
    std::ostringstream output;
    output << name_ << " (hp " << hitPoints_ << "/" << maxHitPoints_
           << ", styrke " << strength_ << ")";
    return output.str();
}

#include "Monster.h"

#include <algorithm>
#include <iostream>
#include <random>
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

const std::vector<Item>& Monster::items() const {
    return items_;
}

const std::vector<StatusEffect>& Monster::statuses() const {
    return statuses_;
}

bool Monster::isDefeated() const {
    return hitPoints_ <= 0;
}

void Monster::takeDamage(int damage) {
    hitPoints_ = std::max(0, hitPoints_ - std::max(0, damage));
}

void Monster::heal(int amount) {
    hitPoints_ = std::min(maxHitPoints_, hitPoints_ + std::max(0, amount));
}

void Monster::healToFull() {
    hitPoints_ = maxHitPoints_;
}

void Monster::addItem(const Item& item) {
    items_.push_back(item);
}

void Monster::addStatus(const StatusEffect& status) {
    statuses_.push_back(status);
}

bool Monster::hasAnyStatus() const {
    return std::any_of(statuses_.begin(), statuses_.end(), [](const StatusEffect& status) {
        return !status.isExpired();
    });
}

bool Monster::applyStartOfTurn(std::ostream& output, std::mt19937& randomEngine) {
    bool canAct = true;

    for (StatusEffect& status : statuses_) {
        if (!status.applyStartOfTurn(*this, output, randomEngine)) {
            canAct = false;
        }
    }

    return canAct && !isDefeated();
}

void Monster::applyAfterDealingDamage(std::ostream& output) {
    for (const StatusEffect& status : statuses_) {
        status.applyAfterDealingDamage(*this, output);
    }
}

void Monster::finishTurn() {
    statuses_.erase(
        std::remove_if(statuses_.begin(), statuses_.end(), [](const StatusEffect& status) {
            return status.isExpired();
        }),
        statuses_.end());
}

void Monster::clearStatuses() {
    statuses_.clear();
}

std::string Monster::summary() const {
    std::ostringstream output;
    output << name_ << " (hp " << hitPoints_ << "/" << maxHitPoints_
           << ", styrke " << strength_ << ")";

    if (!items_.empty()) {
        output << ", ting " << items_.size();
    }

    if (!statuses_.empty()) {
        output << ", status ";
        for (std::size_t index = 0; index < statuses_.size(); ++index) {
            if (index > 0) {
                output << "/";
            }
            output << statuses_[index].name();
        }
    }

    return output.str();
}

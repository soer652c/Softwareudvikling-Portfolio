#include "Item.h"

#include "Monster.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>

Item::Item(std::string name, int damage)
    : name_(std::move(name)),
      damage_(std::max(0, damage)),
      appliesStatus_(false),
      statusChancePercent_(0),
      statusType_(StatusType::Poisoned),
      statusDuration_(0),
      statusTarget_(ItemTarget::Opponent) {}

Item::Item(std::string name,
           int damage,
           int statusChancePercent,
           StatusType statusType,
           int statusDuration,
           ItemTarget statusTarget)
    : name_(std::move(name)),
      damage_(std::max(0, damage)),
      appliesStatus_(true),
      statusChancePercent_(std::clamp(statusChancePercent, 0, 100)),
      statusType_(statusType),
      statusDuration_(std::max(1, statusDuration)),
      statusTarget_(statusTarget) {}

const std::string& Item::name() const {
    return name_;
}

bool Item::dealsDamage() const {
    return damage_ > 0;
}

std::string Item::summary() const {
    std::ostringstream output;
    output << name_;

    if (damage_ > 0) {
        output << " (" << damage_ << " skade";
        if (appliesStatus_) {
            output << ", status chance " << statusChancePercent_ << "%";
        }
        output << ")";
    } else if (appliesStatus_) {
        output << " (status chance " << statusChancePercent_ << "%)";
    }

    return output.str();
}

void Item::use(
    Monster& user, Monster& opponent, std::ostream& output, std::mt19937& randomEngine) const {
    output << user.name() << " bruger " << name_ << ".\n";

    if (damage_ > 0) {
        opponent.takeDamage(damage_);
        output << opponent.name() << " tager " << damage_ << " skade og har "
               << opponent.hitPoints() << " hp tilbage.\n";
    }

    if (!appliesStatus_) {
        return;
    }

    Monster& statusTarget = statusTarget_ == ItemTarget::Self ? user : opponent;
    std::uniform_int_distribution<int> roll(1, 100);

    if (roll(randomEngine) <= statusChancePercent_) {
        statusTarget.addStatus(StatusEffect(statusType_, statusDuration_));
        output << statusTarget.name() << " fik status " << StatusEffect(statusType_, 1).name()
               << ".\n";
    } else {
        output << name_ << " gav ingen status.\n";
    }
}

Item Item::bomb() {
    return Item("Bombe", 10);
}

Item Item::fireBomb() {
    return Item("Ildbombe", 5, 35, StatusType::Stunned, 1, ItemTarget::Opponent);
}

Item Item::thunderBomb() {
    return Item("Tordenbombe", 10, 50, StatusType::Paralyzed, 3, ItemTarget::Opponent);
}

Item Item::club() {
    return Item("Koelle", 20);
}

Item Item::blaster() {
    return Item("Blaeser", 0, 80, StatusType::Frozen, 2, ItemTarget::Opponent);
}

Item Item::curse() {
    return Item("Forbandelse", 0, 20, StatusType::Cursed, 3, ItemTarget::Opponent);
}

Item Item::poison() {
    return Item("Gift", 0, 20, StatusType::Poisoned, 3, ItemTarget::Opponent);
}

std::optional<Item> Item::fromName(const std::string& name) {
    if (name == "Bombe") {
        return Item::bomb();
    }
    if (name == "Ildbombe") {
        return Item::fireBomb();
    }
    if (name == "Tordenbombe") {
        return Item::thunderBomb();
    }
    if (name == "Koelle") {
        return Item::club();
    }
    if (name == "Blaeser") {
        return Item::blaster();
    }
    if (name == "Forbandelse") {
        return Item::curse();
    }
    if (name == "Gift") {
        return Item::poison();
    }

    return std::nullopt;
}

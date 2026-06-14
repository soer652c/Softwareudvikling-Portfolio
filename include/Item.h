#ifndef ITEM_H
#define ITEM_H

#include "StatusEffect.h"

#include <iosfwd>
#include <random>
#include <string>

class Monster;

enum class ItemTarget {
    Self,
    Opponent
};

enum class ItemChanceRule {
    Fixed,
    TargetHasStatus,
    TargetDamaged
};

class Item {
public:
    Item(std::string name, int damage);
    Item(std::string name,
         int damage,
         int statusChancePercent,
         StatusType statusType,
         int statusDuration,
         ItemTarget statusTarget,
         ItemChanceRule chanceRule = ItemChanceRule::Fixed);

    const std::string& name() const;
    bool dealsDamage() const;
    std::string summary() const;

    void use(Monster& user, Monster& opponent, std::ostream& output, std::mt19937& randomEngine) const;

    static Item bomb();
    static Item fireBomb();
    static Item thunderBomb();
    static Item club();
    static Item blaster();
    static Item curse();
    static Item poison();
    static Item focusStone();

private:
    int effectiveStatusChance(const Monster& target) const;

    std::string name_;
    int damage_;
    bool appliesStatus_;
    int statusChancePercent_;
    StatusType statusType_;
    int statusDuration_;
    ItemTarget statusTarget_;
    ItemChanceRule chanceRule_;
};

#endif

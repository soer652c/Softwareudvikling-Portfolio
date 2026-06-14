#ifndef MONSTER_H
#define MONSTER_H

#include "Item.h"
#include "StatusEffect.h"

#include <iosfwd>
#include <random>
#include <string>
#include <vector>

class Monster {
public:
    Monster(std::string name, int hitPoints, int strength);

    const std::string& name() const;
    int hitPoints() const;
    int maxHitPoints() const;
    int strength() const;
    const std::vector<Item>& items() const;
    const std::vector<StatusEffect>& statuses() const;

    bool isDefeated() const;
    void takeDamage(int damage);
    void heal(int amount);
    void healToFull();
    void addItem(const Item& item);
    void addStatus(const StatusEffect& status);
    bool hasAnyStatus() const;
    bool applyStartOfTurn(std::ostream& output, std::mt19937& randomEngine);
    void applyAfterDealingDamage(std::ostream& output);
    void finishTurn();
    void clearStatuses();
    std::string summary() const;

private:
    std::string name_;
    int hitPoints_;
    int maxHitPoints_;
    int strength_;
    std::vector<Item> items_;
    std::vector<StatusEffect> statuses_;
};

#endif

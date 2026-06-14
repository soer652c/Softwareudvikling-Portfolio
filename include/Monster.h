#ifndef MONSTER_H
#define MONSTER_H

#include <string>

class Monster {
public:
    Monster(std::string name, int hitPoints, int strength);

    const std::string& name() const;
    int hitPoints() const;
    int maxHitPoints() const;
    int strength() const;

    bool isDefeated() const;
    void takeDamage(int damage);
    void healToFull();
    std::string summary() const;

private:
    std::string name_;
    int hitPoints_;
    int maxHitPoints_;
    int strength_;
};

#endif

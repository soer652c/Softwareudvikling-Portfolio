#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

#include <iosfwd>
#include <random>
#include <string>

class Monster;

enum class StatusType {
    Poisoned,
    Frozen,
    Paralyzed,
    Stunned,
    Cursed
};

class StatusEffect {
public:
    StatusEffect(StatusType type, int remainingTurns);

    StatusType type() const;
    const std::string& name() const;
    int remainingTurns() const;
    bool isExpired() const;

    bool applyStartOfTurn(Monster& monster, std::ostream& output, std::mt19937& randomEngine);
    void applyAfterDealingDamage(Monster& monster, std::ostream& output) const;

private:
    StatusType type_;
    int remainingTurns_;
    std::string name_;
};

#endif

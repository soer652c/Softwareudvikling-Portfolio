#ifndef GAME_H
#define GAME_H

#include "Character.h"
#include "Input.h"
#include "MonsterCatalog.h"

#include <iosfwd>
#include <optional>
#include <random>

class Game {
public:
    Game(std::istream& input, std::ostream& output);

    void run();

private:
    void showMainMenu();
    void createNewCharacter();
    void startAdventure();
    void listCharacterMonsters() const;
    int chooseEnemyIndex();
    std::size_t chooseLivingMonsterIndex();
    void offerCapturedMonster(const Monster& monster);

    std::ostream& output_;
    Input input_;
    MonsterCatalog catalog_;
    std::optional<Character> character_;
    std::mt19937 randomEngine_;
};

#endif

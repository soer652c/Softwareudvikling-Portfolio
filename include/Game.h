#ifndef GAME_H
#define GAME_H

#include "Cave.h"
#include "Character.h"
#include "Input.h"
#include "Item.h"
#include "MonsterCatalog.h"

#include <iosfwd>
#include <optional>
#include <random>
#include <vector>

class Game {
public:
    Game(std::istream& input, std::ostream& output);

    void run();

private:
    void showMainMenu();
    void createNewCharacter();
    void startAdventure();
    bool fightEnemy(Monster& enemy);
    void startSingleMonsterFight();
    void startCaveFight();
    void listCharacterMonsters() const;
    int chooseEnemyIndex();
    int chooseCaveIndex(const std::vector<Cave>& caves);
    std::size_t chooseLivingMonsterIndex();
    void offerCapturedMonster(const Monster& monster);
    Item randomRewardItem();
    void giveItemToMonster(const Item& item);

    std::ostream& output_;
    Input input_;
    MonsterCatalog catalog_;
    std::optional<Character> character_;
    std::mt19937 randomEngine_;
};

#endif

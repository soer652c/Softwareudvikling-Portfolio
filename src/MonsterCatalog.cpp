#include "MonsterCatalog.h"

MonsterCatalog::MonsterCatalog()
    : monsters_{
          Monster("Hest", 4, 1),
          Monster("Weak Goblin", 4, 2),
          Monster("Strong Goblin", 8, 3),
          Monster("Stronger Goblin", 10, 4),
          Monster("Den staerkeste Goblin", 15, 5),
          Monster("Abe Kongen", 30, 5),
          Monster("Enhjoerning", 50, 8),
          Monster("Drage", 100, 10),
      } {}

const std::vector<Monster>& MonsterCatalog::monsters() const {
    return monsters_;
}

Monster MonsterCatalog::starterHorse() const {
    return monsters_.front();
}

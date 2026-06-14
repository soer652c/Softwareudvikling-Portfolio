cmake_minimum_required(VERSION 3.16)

project(MonsterAdventure VERSION 0.1.0 LANGUAGES CXX)

add_executable(monster_adventure
    src/main.cpp
    src/Battle.cpp
    src/Cave.cpp
    src/Character.cpp
    src/Database.cpp
    src/Game.cpp
    src/Input.cpp
    src/Item.cpp
    src/Monster.cpp
    src/MonsterCatalog.cpp
    src/StatusEffect.cpp
)

target_include_directories(monster_adventure PRIVATE include)

target_link_libraries(monster_adventure PRIVATE sqlite3)

target_compile_features(monster_adventure PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(monster_adventure PRIVATE /W4)
else()
    target_compile_options(monster_adventure PRIVATE -Wall -Wextra -pedantic)
endif()

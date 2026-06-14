cmake_minimum_required(VERSION 3.16)

project(MonsterAdventure VERSION 0.1.0 LANGUAGES CXX)

add_executable(monster_adventure
    src/main.cpp
    src/Battle.cpp
    src/Character.cpp
    src/Game.cpp
    src/Input.cpp
    src/Monster.cpp
    src/MonsterCatalog.cpp
)

target_include_directories(monster_adventure PRIVATE include)

target_compile_features(monster_adventure PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(monster_adventure PRIVATE /W4)
else()
    target_compile_options(monster_adventure PRIVATE -Wall -Wextra -pedantic)
endif()

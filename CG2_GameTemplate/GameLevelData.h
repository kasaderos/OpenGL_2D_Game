#pragma once

#include "stl.h"
#include "GameStructures.h"

namespace game
{

struct LevelData
{
    // Texture name -> texture id.
    stl::map<stl::string, int> textures;
    // Tile name -> tile id.
    stl::map<stl::string, int> tiles;
    stl::vector<Weapon> weapons;
    stl::vector<Potion> potions;
};

}
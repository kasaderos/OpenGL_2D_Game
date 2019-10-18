#pragma once

#include "stl.h"

namespace game
{

struct LevelData
{
    // Texture name -> texture id.
    stl::map<stl::string, int> textures;
    // Tile name -> tile id.
    stl::map<stl::string, int> tiles;
};

// Load entities from *.lvl file.
// Also loads data into the renderer.
LevelData LoadLevel(const char *filename);

}
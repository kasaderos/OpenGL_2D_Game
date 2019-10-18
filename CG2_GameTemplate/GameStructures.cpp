#include "GameStructures.h"

#include <cstdlib>

bool game::TiledSurface::Init(unsigned width, unsigned height, unsigned value)
{
    Release();
    pattern = new unsigned *[width];
    for (unsigned i = 0; i < width; ++i)
    {
        pattern[i] = new unsigned[height];
        for (unsigned j = 0; j < height; ++j)
            pattern[i][j] = value;
    }
    pos = sf::Vector2i(0, 0);
    size = sf::Vector2u(width, height);
    return true;
}

void game::TiledSurface::Release()
{
    if (pattern)
    {
        for (unsigned i = 0; i < size.x; ++i)
            delete[] pattern[i];
        delete[] pattern;
        pattern = nullptr;
    }
    pos = sf::Vector2i(0, 0);
    size = sf::Vector2u(0, 0);
}

bool game::PosPattern::Init(unsigned width, unsigned height, bool value)
{
    Release();
    pattern = new bool *[width];
    for (unsigned i = 0; i < width; ++i)
    {
        pattern[i] = new bool[height];
        for (unsigned j = 0; j < height; ++j)
            pattern[i][j] = value;
    }
    pos = sf::Vector2i(0, 0);
    size = sf::Vector2u(width, height);
    return true;
}

void game::PosPattern::Release()
{
    if (pattern)
    {
        for (unsigned i = 0; i < size.x; ++i)
            delete[] pattern[i];
        delete[] pattern;
        pattern = nullptr;
    }
    pos = sf::Vector2i(0, 0);
    size = sf::Vector2u(0, 0);
}

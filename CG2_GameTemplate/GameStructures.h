#pragma once

#include "Renderer.h"

namespace game
{

struct TiledSurface : public ITiledSurface
{
    // Pos of the bottom left cornder
    // in world coordinates (measured in tiles).
    sf::Vector2i pos = sf::Vector2i(0, 0);
    sf::Vector2u size = sf::Vector2u(0, 0);
    unsigned **pattern = nullptr; // 2D matrix with tile IDs.

    bool Init(unsigned width, unsigned height, unsigned value);
    void Release();

    inline sf::Vector2i GetPos() const override { return pos; }
    inline sf::Vector2u GetSize() const override { return size; }
    inline unsigned GetTileId(int x, int y) const override
    {
        x -= pos.x;
        y -= pos.y;
        return x < size.x && x >= 0 && y < size.y && y >= 0 ? pattern[x][y] : 0;
    }
};

struct PosPattern : public ITiledSurface
{
    // Pos of the bottom left cornder
    // in world coordinates (measured in tiles).
    sf::Vector2i pos = sf::Vector2i(0, 0);
    sf::Vector2u size = sf::Vector2u(0, 0);
    unsigned tileId = 0;
    bool **pattern = nullptr;

    bool Init(unsigned width, unsigned height, bool value);
    void Release();

    inline sf::Vector2i GetPos() const override { return pos; }
    inline sf::Vector2u GetSize() const override { return size; }
    inline bool GetPosExists(int x, int y) const
    {
        x -= pos.x;
        y -= pos.y;
        return x < size.x && x >= 0 && y < size.y && y >= 0 ? unsigned(pattern[x][y]) : 0;
    }
    inline unsigned GetTileId(int x, int y) const override
    {
        x -= pos.x;
        y -= pos.y;
        return x < size.x && x >= 0 && y < size.y && y >= 0 ? unsigned(pattern[x][y]) * tileId : 0;
    }
};

struct Weapon
{
    sf::Vector2u areaSize = sf::Vector2u(0, 0);
    int shift = 0;
    // Interpreted as 2d array.
    // Pattern's initial direction id (0, 1).
    const bool *pattern = nullptr;
    float physDamage = 0.0f;
    float magicDamage = 0.0f;
    float staminaConsumption = 0.0f;
    float manaConsumption = 0.0f;
    unsigned tileId = 0;
};

struct Potion
{
    float healthAmount = 0.0f;
    float staminaAmount = 0.0f;
    float magicAmount = 0.0f;
    unsigned tileId = 0;
};

}
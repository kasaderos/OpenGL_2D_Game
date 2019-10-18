#pragma once

#include "stl.h"

#include "ECS.h"
#include "GameStructures.h"
#include <sfml/Graphics.hpp>

/*
    Component is any non-standard (not int, double, etc.) type
    of data, which identified by its type. So you can't use several components
    of the same type in one entity.
    If you, for example, want to use int as amount of health, create
    a structure HealthComponent with an int member inside it
    (and then other entities will be able to work with your health
    without knowing what the entity you are).

    Here you can place all of the component types you want to use.
*/

#define FROM_STRING(fmt, ...) \
    void FromString(const char *str) \
    { sscanf(str, fmt, __VA_ARGS__); }

struct CompName : ecs::Component
{ 
    stl::string value;
    void FromString(const char *str)
    { value = stl::string(str); }
};

struct CompPos : ecs::Component
{ 
    sf::Vector2i v = sf::Vector2i(0, 0);
    FROM_STRING("%d/%d", &v.x, &v.y)
};

struct CompDir : ecs::Component
{
    sf::Vector2i v = sf::Vector2i(0, 0);
    FROM_STRING("%d/%d", &v.x, &v.y)
};

struct CompMoveDir : ecs::Component
{
    sf::Vector2i v = sf::Vector2i(0, 0);
    FROM_STRING("%d/%d", &v.x, &v.y)
};

struct CompHealth : ecs::Component
{ 
    float value = 100.0f;
    FROM_STRING("%f", &value)
};

struct CompStamina : ecs::Component
{ 
    float value = 100.0f;
    FROM_STRING("%f", &value)
};

struct CompMana : ecs::Component
{ 
    float value = 100.0f;
    FROM_STRING("%f", &value)
};

struct CompArmor : ecs::Component
{
    // finalDamage = physDamage * (1 - physResist) + magicDamage * (1 - magicResist)
    float physResist = 0.0f;
    float magicResist = 0.0f;
    FROM_STRING("%f/%f", &physResist, &magicResist)
};

struct CompDamage : ecs::Component
{
    float physDamage = 0.0f;
    float magicDamage = 0.0f;
    FROM_STRING("%f/%f", &physDamage, &magicDamage)
};

// How many stamina and mana entity takes.
struct CompConsumption : ecs::Component
{
    float stamina = 0.0f;
    float mana = 0.0f;
    FROM_STRING("%f/%f", &stamina, &mana)
};

struct CompTimeToLive : ecs::Component
{ 
    int ticks = 0;
    FROM_STRING("%d", &ticks)
};

struct CompDrawTileInfo
{ 
    enum
    {
        LAYER_BACKGROUND,
        LAYER_OBJECTS,
        LAYER_EFFECTS,
        LAYERS_COUNT
    };
    unsigned tileId = 0;
    unsigned layer = 0;
    FROM_STRING("%u%u", &tileId, &layer)
};

struct CompAnimInfo : ecs::Component
{
    // TODO
};

struct CompOwnedItems : ecs::Component
{
    stl::vector<const game::Weapon *> weapons;
    stl::vector<const game::Potion *> potions;
    int curWeaponId = -1;
};

// 2D array of positions.
struct CompPosPattern : ecs::Component
{
    game::PosPattern positions;
};

struct CompActivity : ecs::Component
{
    bool active = false;
    bool disableAfterOneTick = true;
};

struct CompVisibility : ecs::Component
{
    bool visible = true;
    bool hideAfterOneTick = false;
};

struct CompCurrentAttackArea : ecs::Component
{
    ecs::EntityDesc ent = ecs::EntityDesc(ecs::GetEntityTid<void>(), 0);
    sf::Vector2i shift = sf::Vector2i(0, 0);
};

struct CompOwner : ecs::Component
{
    ecs::EntityDesc owner = ecs::EntityDesc(ecs::GetEntityTid<void>(), 0);
};

struct CompParty : ecs::Component
{
    enum
    {
        NEUTRAL,
        ALLIANCE,
        ENEMIES
    };
    uint8_t party;
};

struct CompVisionRadius : ecs::Component
{
    unsigned value = 0;
    FROM_STRING("%u", &value)
};

struct TagCollidable : ecs::Component {};

struct TagCollecatble : ecs::Component {};

struct TagPlayer : ecs::Component {};

// Player is ally too.
// It is for enemies to know, whom to attack.
struct TagAlly : ecs::Component {};

struct TagEnemy : ecs::Component {};
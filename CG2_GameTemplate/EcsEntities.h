#pragma once

/*
    In current realisation, entity is something that
    have components and can work and interact with
    other entities through 'Update()' (called on each frame update).

    If you want to initialize an entity, use OnCreated() instead of constructor.

    Here you can place all of the entity types you want to use.
*/

#include "ECS.h"
#include "EcsComponents.h"
#include <iostream>

class Player : public ecs::Entity<
    CompPos,
    CompDir,
    CompMoveDir,
    CompHealth,
    CompDrawTileInfo,
    CompOwnedItems,
    CompCurrentAttackArea,
    CompStamina,
    CompMana,
    CompArmor,
    CompParty,
    // CompVisibility, // May be used for hide player (feature).
    //TagAlly,
    TagCollidable,
    TagPlayer>
{
public:
    ECS_ENTITY_DEFAULTS(Player)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
};

class AttackArea : public ecs::Entity<
    CompPosPattern,
    CompActivity,
    CompDamage,
    CompOwner,
    CompVisibility,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(AttackArea)
};

class Walls : public ecs::Entity<
    CompPosPattern,
    CompDrawTileInfo,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Walls)
};

class Floor : public ecs::Entity<
    CompPosPattern,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(Floor)
};

// Ususal enemies have no stamina and mana.
class Enemy : public ecs::Entity<
    CompPos,
    CompMoveDir,
    CompHealth,
    CompDrawTileInfo,
    CompCurrentAttackArea,
    CompArmor,
    CompDamage,
    CompParty,
    CompVisionRadius,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Enemy)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
private:
    unsigned curTick = 0;
};

#ifndef ECS_REGISTER_ENTITY_TYPE
#define ECS_REGISTER_ENTITY_TYPE(typeName)
#endif

/*
    Register here all of the entity types
    you want to create by string names (and therefore from files).
*/

ECS_REGISTER_ENTITY_TYPE(Player)
ECS_REGISTER_ENTITY_TYPE(AttackArea)
ECS_REGISTER_ENTITY_TYPE(Walls)
ECS_REGISTER_ENTITY_TYPE(Floor)
ECS_REGISTER_ENTITY_TYPE(Enemy)
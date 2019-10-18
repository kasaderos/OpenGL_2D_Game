#pragma once

#include "ECS.h"
#include "EcsComponents.h"
#include "EcsEntities.h"
#include "GameEvents.h"

/*
    System is just entity without components.

    Here you can place all of the systems you want to use.
*/

class GameInputControllerSys :
    public ecs::System,
    public game::EventListener<game::EventSfml>
{
public:
    ECS_SYSTEM_DEFAULTS(GameInputControllerSys)
    void OnEvent(const game::EventSfml &evt) override;
    void OnUpdate() override;
};

class GameRenderSys : public ecs::System
{
public:
    ECS_SYSTEM_DEFAULTS(GameRenderSys)
    void OnUpdate() override;
};

class GameAttackSys : public ecs::System
{
public:
    ECS_SYSTEM_DEFAULTS(GameAttackSys)
    void OnUpdate() override;
};

class GameGuiRenderSys :
    public ecs::System,
    public game::EventListener<game::EventRenderWindowWasCreated>,
    public game::EventListener<game::EventRendererWasInited>
{
public:
    ECS_SYSTEM_DEFAULTS(GameGuiRenderSys)
    void OnEvent(const game::EventRenderWindowWasCreated &evt) override;
    void OnEvent(const game::EventRendererWasInited &evt) override;
    void OnUpdate() override;
};

class GameBackgroundGuiRenderSys : public ecs::System
{
public:
    ECS_SYSTEM_DEFAULTS(GameBackgroundGuiRenderSys)
    void OnUpdate() override;
};

class GameMoveSys : public ecs::System
{
public:
    ECS_SYSTEM_DEFAULTS(GameMoveSys)
    void OnUpdate() override;
};

#ifndef ECS_REGISTER_SYSTEM
#define ECS_REGISTER_SYSTEM(sysName)
#endif

/*
Register here all of the systems you want to be processed.
*/

ECS_REGISTER_SYSTEM(GameInputControllerSys)
ECS_REGISTER_SYSTEM(GameRenderSys)
ECS_REGISTER_SYSTEM(GameAttackSys)
ECS_REGISTER_SYSTEM(GameGuiRenderSys)
ECS_REGISTER_SYSTEM(GameBackgroundGuiRenderSys)
ECS_REGISTER_SYSTEM(GameMoveSys)
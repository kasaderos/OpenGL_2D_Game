#include "ECS.h"
#include "EcsEntities.h"
#include "EcsSystems.h"

#define ECS_ORDER_ELEMENT(type) std::type_index(typeid(type))

namespace ecs
{

class StubEntity : public ecs::IEntity
{
public:
    void OnUpdate() override {}
};

EntityTypeId EntityManager::forwardUpdateOrder[] = {
    ECS_ORDER_ELEMENT(GameBackgroundGuiRenderSys),
    ECS_ORDER_ELEMENT(GameInputControllerSys),
    ECS_ORDER_ELEMENT(Enemy), // Has update.
    ECS_ORDER_ELEMENT(GameMoveSys),
    ECS_ORDER_ELEMENT(GameAttackSys)
    // Here you can place your systems, which will be updated first (in that order),
    // if the update order matters.
};

EntityTypeId EntityManager::backwardUpdateOrder[] = {
    ECS_ORDER_ELEMENT(GameRenderSys),
    ECS_ORDER_ELEMENT(GameGuiRenderSys)
    // Here you can place your systems, which will be updated last (in that order),
    // if the update order matters.
};

const unsigned EntityManager::forwardUpdateOrderSize =
sizeof(EntityManager::forwardUpdateOrder) / sizeof(EntityTypeId);

const unsigned EntityManager::backwardUpdateOrderSize =
sizeof(EntityManager::backwardUpdateOrder) / sizeof(EntityTypeId);

}
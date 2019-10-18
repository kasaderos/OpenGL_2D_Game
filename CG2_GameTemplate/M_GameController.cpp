#include "stl.h"
#include "EcsSystems.h"
#include "Utils.h"

static stl::queue<game::EventSfml> inputPool;

void GameInputControllerSys::OnEvent(const game::EventSfml &evt)
{
    inputPool.push(evt);
}

static bool Move(const CompPos *curPos)
{
    bool canMove = true;
    ECS_EXEC_FOR(
    [&](ecs::IEntity *ent) {
        if (CmpIntVec(*curPos, *ent->GetComp<CompPos>()))
            canMove = false;
    },
    ECS_COMP(CompPos),
    ECS_COMP(TagCollidable)
    );
    if (!canMove)
        return false;

    ECS_EXEC_FOR(
    [&](ecs::IEntity *ent) {
        if (ent->GetComp<CompPosPattern>()->positions.GetTileId(curPos->x, curPos->y))
            canMove = false;
    },
    ECS_COMP(CompPosPattern),
    ECS_COMP(TagCollidable)
    );
    if (!canMove)
        return false;


}

void GameInputControllerSys::OnUpdate()
{
    
}
#include "EcsSystems.h"

void MoveMovables()
{
    ecs::Apply<CompPos, CompMoveDir>([&](ecs::IEntity *movable)
    {
        CompMoveDir *dir = movable->GetComp<CompMoveDir>();
        CompPos *pos = movable->GetComp<CompPos>();

        if (movable->GetComp<TagCollidable>())
        {
            bool canMove = true;
            ecs::Apply<TagCollidable>([&](ecs::IEntity *collidable)
            {
                CompPos *colPos = collidable->GetComp<CompPos>();
                CompPosPattern *colPattern = collidable->GetComp<CompPosPattern>();
                if (colPos)
                {
                    if (pos->v + dir->v == colPos->v)
                        canMove = false;
                }
                else if (colPattern)
                {
                    if (colPattern->positions.GetPosExists(pos->v.x + dir->v.x, pos->v.y + dir->v.y))
                        canMove = false;
                }
            });
            if (canMove)
                pos->v += dir->v;
        }
        else
        {
            pos->v += dir->v;
        }
    });
}

void MoveAttackArea()
{
    ecs::Apply<CompPos, CompCurrentAttackArea>([&](ecs::IEntity *ent)
    {
        CompCurrentAttackArea *areaComp = ent->GetComp<CompCurrentAttackArea>();
        ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(areaComp->ent);
        area->GetComp<CompPosPattern>()->positions.pos = ent->GetComp<CompPos>()->v + areaComp->shift;
    });
}

void GameMoveSys::OnUpdate()
{
    MoveMovables();
    MoveAttackArea();
}
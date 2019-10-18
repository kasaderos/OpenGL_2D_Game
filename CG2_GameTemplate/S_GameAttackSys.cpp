#include "EcsSystems.h"
#include "stl.h"

void GameAttackSys::OnUpdate()
{
    ecs::Apply<CompPosPattern, CompActivity, CompDamage, CompOwner>([&](ecs::IEntity *area)
    {
        CompActivity *act = area->GetComp<CompActivity>();
        if (!act->active)
            return;
        if (act->disableAfterOneTick)
            act->active = false;
        CompPosPattern *positions = area->GetComp<CompPosPattern>();
        ecs::IEntity *owner = ecs::gEntityManager.GetEntityByDesc(area->GetComp<CompOwner>()->owner);

        ecs::Apply<CompPos, CompHealth, CompArmor, CompParty>([&](ecs::IEntity *creature)
        {
            if (creature->GetComp<CompParty>()->party == owner->GetComp<CompParty>()->party)
                return;

            CompPos *pos = creature->GetComp<CompPos>();
            if (positions->positions.GetPosExists(pos->v.x, pos->v.y))
            {
                CompDamage *damage = area->GetComp<CompDamage>();
                CompArmor *resist = creature->GetComp<CompArmor>();
                float physResist = 0.0f;
                float magicResist = 0.0f;
                if (resist)
                {
                    physResist = resist->physResist;
                    magicResist = resist->magicResist;
                }
                CompHealth *h = creature->GetComp<CompHealth>();
                h->value -= stl::max(damage->physDamage * (1.0f - physResist), 0.0f);
                h->value -= stl::max(damage->magicDamage * (1.0f - magicResist), 0.0f);
                if (h->value <= 0.0f)
                    ecs::gEntityManager.DestroyEntity(creature->GetDesc());
            }
        });
    });
}
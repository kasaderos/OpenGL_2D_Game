#include "EcsEntities.h"

#include "Renderer.h"

constexpr unsigned enemyUpdatePeriod = 2;

void Player::OnCreated()
{
    GetComp<CompParty>()->party = CompParty::ALLIANCE;

    int attackAreaTileId = game::gRenderer.AddTile(sf::Color(255, 0, 0, 128));
    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *ent = ecs::gEntityManager.GetEntityByDesc(eid);
    ent->GetComp<CompDrawTileInfo>()->tileId = attackAreaTileId;
    ent->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_EFFECTS;
    ent->GetComp<CompVisibility>()->visible = false;
    ent->GetComp<CompOwner>()->owner = this->GetDesc();

    GetComp<CompCurrentAttackArea>()->ent = eid;
}

void Player::OnDestroy()
{
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void Player::OnUpdate()
{
    CompHealth *h = GetComp<CompHealth>();
    CompStamina *s = GetComp<CompStamina>();
    CompMana *m = GetComp<CompMana>();
    if (h->value < 100.0f)
        h->value = std::min(100.0f, h->value + 0.5f);
    if (s->value < 100.0f)
        s->value = std::min(100.0f, s->value + 0.5f);
    if (m->value < 100.0f)
        m->value = std::min(100.0f, m->value + 0.5f);
}

void Enemy::OnCreated()
{
    GetComp<CompParty>()->party = CompParty::ENEMIES;

    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);

    area->GetComp<CompVisibility>()->visible = false;
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = false;
    area->GetComp<CompActivity>()->disableAfterOneTick = true;
    
    // 3x3 attack area.
    area->GetComp<CompPosPattern>()->positions.Init(3, 3, 1);

    GetComp<CompCurrentAttackArea>()->ent = eid;
    GetComp<CompCurrentAttackArea>()->shift = sf::Vector2i(-1, -1);
}

void Enemy::OnDestroy()
{
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void Enemy::OnUpdate()
{
    ++curTick;
    
    IEntity *area = ecs::gEntityManager.GetEntityByDesc(GetComp<CompCurrentAttackArea>()->ent);
    CompDamage *myDamage = GetComp<CompDamage>();
    area->GetComp<CompDamage>()->physDamage = myDamage->physDamage;
    area->GetComp<CompDamage>()->magicDamage = myDamage->magicDamage;
    if (!(curTick % enemyUpdatePeriod))
    {
        area->GetComp<CompActivity>()->active = true;
    }
    else
    {
        GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
        return;
    }

    unsigned vr = GetComp<CompVisionRadius>()->value;

    sf::Vector2i dir(0, 0);
    sf::Vector2i pos = GetComp<CompPos>()->v;

    // Now enemies are just hunt for the player.
    // Now there is very stupid algorithm:
    // just go in the player direction.
    ecs::Apply<TagPlayer>([&](IEntity *player)
    {
        CompPos *playerPos = player->GetComp<CompPos>();
        
        int dx = abs(playerPos->v.x - pos.x);
        int dy = abs(playerPos->v.y - pos.y);
        if (std::max(dx, dy) > GetComp<CompVisionRadius>()->value)
            return;

        if (playerPos->v.x < pos.x)
            dir.x = -1;
        else if (playerPos->v.x > pos.x)
            dir.x = 1;
        if (playerPos->v.y < pos.y)
            dir.y = -1;
        else if (playerPos->v.y > pos.y)
            dir.y = 1;
    });

    GetComp<CompMoveDir>()->v = dir;
}

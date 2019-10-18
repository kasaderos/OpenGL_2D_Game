#include "stl.h"
#include "EcsSystems.h"
#include "Utils.h"
#include "GameStructures.h"

static stl::queue<game::EventSfml> inputPool;

extern stl::vector<game::Weapon> gGameWeapons;

void GameInputControllerSys::OnEvent(const game::EventSfml &evt)
{
    inputPool.push(evt);
}

static bool Move(CompPos *curPos, sf::Vector2i dxy)
{
    bool canMove = true;
    ecs::Apply<CompPos, TagCollidable>([&](ecs::IEntity *ent) {
        if (curPos->v + dxy == ent->GetComp<CompPos>()->v)
            canMove = false;
    });
    if (!canMove)
        return false;

    ecs::Apply<CompPosPattern, TagCollidable>([&](ecs::IEntity *ent) {
        if (ent->GetComp<CompPosPattern>()->
            positions.GetPosExists(curPos->v.x + dxy.x, curPos->v.y + dxy.y))
            canMove = false;
    });
    if (!canMove)
        return false;

    curPos->v += dxy;

    ecs::Apply<CompPos, TagCollecatble>([&](ecs::IEntity *ent) {
        if (ent->GetComp<CompPos>()->v == curPos->v)
        {
            // TODO: Collect item.
            ecs::gEntityManager.DestroyEntity(ent->GetDesc());
        }
    });
}

static bool ChangeAttackField(CompPos *myPos, CompDir *dir, CompOwnedItems *ammo,
    CompCurrentAttackArea *area)
{
    if (ammo->curWeaponId < 0 || ammo->curWeaponId >= int(ammo->weapons.size()))
        return false;

    sf::Vector2i normDir(0, 0);
    if (abs(dir->v.x) > abs(dir->v.y))
        normDir.x = dir->v.x > 0 ? 1 : -1;
    else
        normDir.y = dir->v.y > 0 ? 1 : -1;

    ecs::IEntity *areaEnt = ecs::gEntityManager.GetEntityByDesc(area->ent);
    if (!areaEnt)
        return false;

    CompPosPattern *pp = areaEnt->GetComp<CompPosPattern>();

    const game::Weapon *weap = ammo->weapons[ammo->curWeaponId];

    int w = weap->areaSize.x;
    int h = weap->areaSize.y;

    if (normDir.y == 1)
    {
        pp->positions.Init(w, h, 0);
        area->shift = sf::Vector2i(- w / 2, 1 + weap->shift);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[i][j] = weap->pattern[i * h + j];
    }
    else if (normDir.y == -1)
    {
        pp->positions.Init(w, h, 0);
        area->shift = sf::Vector2i(- w / 2, - h - weap->shift);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[w - 1 - i][h - 1 - j] = weap->pattern[i * h + j];
    }
    else if (normDir.x == 1)
    {
        pp->positions.Init(h, w, 0);
        area->shift = sf::Vector2i(1 + weap->shift, - w / 2);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[j][w - 1 - i] = weap->pattern[i * h + j];
    }
    else
    {
        pp->positions.Init(h, w, 0);
        area->shift = sf::Vector2i(- h - weap->shift, - w / 2);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[h - 1 - j][i] = weap->pattern[i * h + j];
    }
    
    CompDamage *damage = areaEnt->GetComp<CompDamage>();
    damage->physDamage = weap->physDamage;
    damage->magicDamage = weap->magicDamage;

    return true;
}

static void ShowAttackField(CompVisibility *vis)
{
    vis->visible = true;
    vis->hideAfterOneTick = true;
}

static void Attack(CompActivity *act, CompOwnedItems *items, ecs::IEntity *player)
{
    float stamCons = items->weapons[items->curWeaponId]->staminaConsumption;
    float manaCons = items->weapons[items->curWeaponId]->manaConsumption;
    CompStamina *s = player->GetComp<CompStamina>();
    CompMana *m = player->GetComp<CompMana>();

    if (s->value - stamCons >= 0.0f && m->value - manaCons >= 0.0f)
    {
        s->value -= stamCons;
        m->value -= manaCons;
        act->active = true;
        act->disableAfterOneTick = true; // So the damage will be only in one step.
    }
}

void GameInputControllerSys::OnUpdate()
{
    auto players = ecs::gEntityManager.GetEntitiesByType(ecs::GetEntityTid<Player>());
    if (!players)
        return;
    IEntity *player = players->at(0).get();
    if (!player)
        return;

    // Yes, input is hardcoded.
    // You may change such realization, if you want.

    CompOwnedItems *items = player->GetComp<CompOwnedItems>();

    bool needAttackChange = false;
    bool needAttack = false;
    bool needShowAttack = false;

    sf::Vector2i moveDir(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moveDir += sf::Vector2i(0, 1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moveDir += sf::Vector2i(-1, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moveDir += sf::Vector2i(0, -1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moveDir += sf::Vector2i(1, 0);

    player->GetComp<CompMoveDir>()->v = moveDir;
    if (moveDir != sf::Vector2i(0, 0))
        needAttackChange = true;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        IEntity *attackArea = ecs::gEntityManager.GetEntityByDesc(
            player->GetComp<CompCurrentAttackArea>()->ent);
        if (attackArea)
            ShowAttackField(attackArea->GetComp<CompVisibility>());
    }

    if (needAttackChange)
    {
        ChangeAttackField(player->GetComp<CompPos>(), player->GetComp<CompDir>(),
            items, player->GetComp<CompCurrentAttackArea>());
    }

    while (inputPool.size())
    {
        needAttackChange = false;
        needAttack = false;
        needShowAttack = false;

        game::EventSfml &evt = inputPool.front();
        
        switch (evt.type)
        {
        case game::EventSfml::KeyPressed:
            switch (evt.key.code)
            {
            case sf::Keyboard::Up:
                player->GetComp<CompDir>()->v = sf::Vector2i(0, 1);
                needAttackChange = true;
                needShowAttack = true;
                break;
            case sf::Keyboard::Down:
                player->GetComp<CompDir>()->v = sf::Vector2i(0, -1);
                needAttackChange = true;
                needShowAttack = true;
                break;
            case sf::Keyboard::Left:
                player->GetComp<CompDir>()->v = sf::Vector2i(-1, 0);
                needAttackChange = true;
                needShowAttack = true;
                break;
            case sf::Keyboard::Right:
                player->GetComp<CompDir>()->v = sf::Vector2i(1, 0);
                needAttackChange = true;
                needShowAttack = true;
                break;
            }
            if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
            {
                unsigned weapId = evt.key.code - sf::Keyboard::Num1;
                if (weapId < items->weapons.size())
                {
                    items->curWeaponId = weapId;
                    needAttackChange = true;
                }
            }
            break;
        case game::EventSfml::KeyReleased:
            switch (evt.key.code)
            {
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
                needAttack = true;
                break;
            }
            break;
        }

        if (needAttackChange)
        {
            ChangeAttackField(player->GetComp<CompPos>(), player->GetComp<CompDir>(),
                items, player->GetComp<CompCurrentAttackArea>());
        }
        if (needAttack || needShowAttack)
        {
            IEntity *attackArea = ecs::gEntityManager.GetEntityByDesc(
                player->GetComp<CompCurrentAttackArea>()->ent);
            if (attackArea)
            {
                if (needShowAttack)
                    ShowAttackField(attackArea->GetComp<CompVisibility>());
                if (needAttack)
                    Attack(attackArea->GetComp<CompActivity>(), items, player);
            }
        }

        inputPool.pop();
    }
}
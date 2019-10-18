#include "stl.h"
#include "EcsSystems.h"
#include "Renderer.h"

#define LAYERS_COUNT CompDrawTileInfo::LAYERS_COUNT

void GameRenderSys::OnUpdate()
{
    sf::Vector2f camPos = sf::Vector2f(0, 0);
    stl::map<CompDrawTileInfo *, CompPos *> singleObjects[LAYERS_COUNT];
    stl::map<CompDrawTileInfo *, CompPosPattern *> patternObjects[LAYERS_COUNT];

    ecs::Apply<CompPos, CompDrawTileInfo>([&](ecs::IEntity *ent)
    {
        CompVisibility *vis = ent->GetComp<CompVisibility>();
        if (vis)
        {
            if (!vis->visible)
                return;
            if (vis->hideAfterOneTick)
                vis->visible = false;
        }
        CompDrawTileInfo *info = ent->GetComp<CompDrawTileInfo>();
        if (info->layer < LAYERS_COUNT)
        {
            CompPos *pos = ent->GetComp<CompPos>();
            singleObjects[info->layer].emplace(info, pos);
            if (ent->GetComp<TagPlayer>())
                camPos = sf::Vector2f(pos->v) + sf::Vector2f(0.5f, 0.5f);
        }
    });

    ecs::Apply<CompPosPattern, CompDrawTileInfo>([&](ecs::IEntity *ent)
    {
        CompVisibility *vis = ent->GetComp<CompVisibility>();
        if (vis)
        {
            if (!vis->visible)
                return;
            if (vis->hideAfterOneTick)
                vis->visible = false;
        }
        CompDrawTileInfo *info = ent->GetComp<CompDrawTileInfo>();
        if (info->layer < LAYERS_COUNT)
        {
            CompPosPattern *pattern = ent->GetComp<CompPosPattern>();
            pattern->positions.tileId = info->tileId;
            patternObjects[info->layer].emplace(info, pattern);
        }
    });

    game::gRenderer.SetCameraCenterPos(camPos);
    game::gRenderer.BeginDrawWorld();
    for (int i = 0; i < LAYERS_COUNT; ++i)
    {
        for (auto &surf : patternObjects[i])
            game::gRenderer.DrawTiledSurface(surf.second->positions);
        for (auto &obj : singleObjects[i])
            game::gRenderer.DrawSingleTile(obj.first->tileId, sf::Vector2f(obj.second->v.x, obj.second->v.y));
    }
}
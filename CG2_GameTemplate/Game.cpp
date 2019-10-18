#include "Game.h"
#include "stl.h"
#include "GameStructures.h"
#include "GameDataLoad.h"
#include "ECS.h"
#include "Timer.h"
#include "EcsSystems.h"

#include <windows.h>

#include "Renderer.h"

namespace game
{

static bool swordPattern[3 * 2] = {
    0, 0,
    1, 1,
    0, 0
};

static bool clubPattern[3 * 2] = {
    1, 0,
    0, 1,
    1, 0
};

stl::vector<Weapon> gGameWeapons;

void Game::Init(const stl::shared_ptr<sf::RenderWindow> &window)
{
    //this->window = window;
    //renderer = stl::make_unique<Renderer>();
    //renderer->Init(window);
    gRenderer.Init(window);
    SendEvent(EventRendererWasInited());
    //InitDefaultGui();

    Weapon w;
    int id;

    w.areaSize = sf::Vector2u(3, 2);
    w.staminaConsumption = 4.0f;
    w.manaConsumption = 8.0f;
    w.magicDamage = 5.0f;
    w.physDamage = 10.0f;
    w.shift = 0;
    w.pattern = swordPattern;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_218.png");
    w.tileId = gRenderer.AddTile(id);
    gGameWeapons.push_back(w);

    w.staminaConsumption = 15.0f;
    w.manaConsumption = 0.0f;
    w.magicDamage = 0.0f;
    w.physDamage = 20.0f;
    w.pattern = clubPattern;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_190.png");
    w.tileId = gRenderer.AddTile(id);
    gGameWeapons.push_back(w);
}

void Game::Release()
{
    //window.reset();
    gRenderer.Release();
    gGameWeapons.clear();
    ecs::gEntityManager.DestroyAllEntities();
}

GameResults Game::StartLevel(const char *levelFilename)
{
    LevelData data = LoadLevel(levelFilename);

    ecs::Apply<CompOwnedItems, CompCurrentAttackArea>([&](ecs::IEntity *ent) {
        CompOwnedItems *ammo = ent->GetComp<CompOwnedItems>();
        ammo->weapons.push_back(&gGameWeapons[0]);
        ammo->weapons.push_back(&gGameWeapons[1]);
        ammo->curWeaponId = 0;
        CompCurrentAttackArea *area = ent->GetComp<CompCurrentAttackArea>();
    });

    while (true)
    {
        gRenderer.BeginDraw(sf::Color(0x000000FFU));
        ecs::gEntityManager.UpdateEntities();
        gRenderer.EndDraw();
        Sleep(200);
    }
    return GameResults();
}

void Game::InitDefaultGui()
{
    gRenderer.GuiAddText("label1", -1, { 0, 0 }, "Hello world!", 20, sf::Color(0xFFFFFFFFU));

    //int tid = renderer->AddTexture("tileset/oryx_creatures_preview.gif");
    texNames.emplace("back", gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_920.png"));
    //gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_59.png");

    unsigned id = texNames["back"];
    gRenderer.GuiAddTexturedRect("background", -2, { 0, 0 }, { 1024, 768 }, id,
        {0, 0}, gRenderer.ntoat({16.0f, 12.0f}, id));

    //gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_59.png");

    //gRenderer.AddTile(id);

    gRenderer.SetVisionAreaSize(sf::Vector2f(15.0f, 11.0f));
    gRenderer.SetCameraViewport(100, 100, 750, 550);
    gRenderer.SetCameraCenterPos(sf::Vector2f(20.0f, 20.0f));
    //renderer->Se
}

}
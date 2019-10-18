#include "EcsSystems.h"

#include "Renderer.h"

static const sf::Vector2i cellSize = sf::Vector2i(40, 40);
static const sf::Vector2i vpCells = sf::Vector2i(21, 15);
static const sf::Vector2i vpCellsPos = sf::Vector2i(1, 1);
static const sf::Vector2i wndCells = sf::Vector2i(30, 17);
static const sf::Vector2i wSize = sf::Vector2i(cellSize.x * wndCells.x, cellSize.y * wndCells.y);
static const sf::Vector2i vpPos = sf::Vector2i(cellSize.x * vpCellsPos.x, cellSize.y * vpCellsPos.y);
static const sf::Vector2i vpSize = sf::Vector2i(cellSize.x * vpCells.x, cellSize.y * vpCells.y);
static const sf::Vector2i curWeaponPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 4);
static const sf::Vector2i ownedItemsPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 6);
static const sf::Vector2i healthPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 5, vpPos.y + cellSize.y * 0);
static const sf::Vector2i staminaPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 5, vpPos.y + cellSize.y * 1);
static const sf::Vector2i manaPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 5, vpPos.y + cellSize.y * 2);
static const int itemsBarWidth = wndCells.x - vpCells.x - vpCellsPos.x - 1;
static const int itemsBarHeight = 4;

static stl::map<stl::string, int> guiTextures;

void GameGuiRenderSys::OnEvent(const game::EventRenderWindowWasCreated &evt)
{
    evt.wnd->setSize(sf::Vector2u(wSize));
}

void GameGuiRenderSys::OnEvent(const game::EventRendererWasInited &evt)
{
    guiTextures.emplace("backBlock", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1969.png"));
    guiTextures.emplace("left", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1990.png"));
    guiTextures.emplace("right", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1988.png"));
    guiTextures.emplace("up", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1987.png"));
    guiTextures.emplace("down", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1989.png"));
    guiTextures.emplace("topLeft", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1978.png"));
    guiTextures.emplace("topRight", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1979.png"));
    guiTextures.emplace("bottomLeft", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1981.png"));
    guiTextures.emplace("bottomRight", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1980.png"));

    game::gRenderer.GuiAddTexturedRect("up", 1,
        { vpPos.x + cellSize.x * 1, vpPos.y },
        { vpSize.x - cellSize.x * 2, cellSize.y * 1 }, guiTextures["up"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(vpCells.x - 2, 1), guiTextures["up"]));
    game::gRenderer.GuiAddTexturedRect("down", 1,
        { vpPos.x + cellSize.x * 1, vpPos.y + vpSize.y - cellSize.y * 1 },
        { vpSize.x - cellSize.x * 2, cellSize.y * 1 }, guiTextures["down"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(vpCells.x - 2, 1), guiTextures["down"]));
    game::gRenderer.GuiAddTexturedRect("left", 1,
        { vpPos.x, vpPos.y + cellSize.y * 1 },
        { cellSize.x * 1, vpSize.y - cellSize.y * 2 }, guiTextures["left"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(1, vpCells.y - 2), guiTextures["left"]));
    game::gRenderer.GuiAddTexturedRect("right", 1,
        { vpPos.x + vpSize.x - cellSize.x * 1, vpPos.y + cellSize.y * 1 },
        { cellSize.x * 1, vpSize.y - cellSize.y * 2 }, guiTextures["right"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(1, vpCells.y - 2), guiTextures["right"]));


    game::gRenderer.GuiAddTexturedRect("topLeft", 1,
        { vpPos.x, vpPos.y },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["topLeft"]);
    game::gRenderer.GuiAddTexturedRect("topRight", 1,
        { vpPos.x + vpSize.x - cellSize.x * 1, vpPos.y },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["topRight"]);
    game::gRenderer.GuiAddTexturedRect("bottomLeft", 1,
        { vpPos.x, vpPos.y + vpSize.y - cellSize.y * 1 },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["bottomLeft"]);
    game::gRenderer.GuiAddTexturedRect("bottomRight", 1,
        { vpPos.x + vpSize.x - cellSize.x * 1, vpPos.y + vpSize.y - cellSize.y * 1 },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["bottomRight"]);

    game::gRenderer.GuiAddTexturedRect("backBlock", -1,
        { 0, 0 },
        { wSize.x, wSize.y }, guiTextures["backBlock"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(wndCells.x, wndCells.y), guiTextures["backBlock"]));

    game::gRenderer.GuiAddText("healthLabel", 2, { vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 0 },
        "Health: ", 20, sf::Color(0xFFFFFFFFU));
    game::gRenderer.GuiAddText("staminaLabel", 2, { vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 1 },
        "Stamina: ", 20, sf::Color(0xFFFFFFFFU));
    game::gRenderer.GuiAddText("manaLabel", 2, { vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 2 },
        "Mana: ", 20, sf::Color(0xFFFFFFFFU));
    game::gRenderer.GuiAddText("weaponLabel", 2, { vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 3 },
        "Current weapon: ", 20, sf::Color(0xFFFFFFFFU));
    game::gRenderer.GuiAddText("inventoryLabel", 2, { vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 5 },
        "Items: ", 20, sf::Color(0xFFFFFFFFU));

    game::gRenderer.GuiAddText("healthBar", 2, healthPos, "100.0", 20);
    game::gRenderer.GuiAddText("staminaBar", 2, staminaPos, "100.0", 20);
    game::gRenderer.GuiAddText("manaBar", 2, manaPos, "100.0", 20);

    game::gRenderer.GuiAddTile("weaponBar", 2, curWeaponPos, cellSize, 0);

    for (int i = 0; i < itemsBarWidth * itemsBarHeight; ++i)
    {
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "itm%d", i);
        game::gRenderer.GuiAddTile(buf, 2, sf::Vector2i(
            (i % itemsBarWidth) * cellSize.x + ownedItemsPos.x,
            (i / itemsBarWidth) * cellSize.y + ownedItemsPos.y),
            cellSize, 0);
    }

    //int tid = game::gRenderer.AddTile(sf::Color(0x000000FFU));
    //game::gRenderer.SetOuterFillTile(tid);



    game::gRenderer.SetVisionAreaSize(sf::Vector2f(vpSize.x / cellSize.x, vpSize.y / cellSize.y));
    game::gRenderer.SetCameraViewport(vpPos.x, vpPos.y, vpSize.x, vpSize.y);
    game::gRenderer.SetCameraCenterPos(sf::Vector2f(20.0f, 20.0f));
}

void GameGuiRenderSys::OnUpdate()
{
    ecs::Apply<CompHealth, CompStamina, CompMana, CompOwnedItems, TagPlayer>([&](IEntity *ent)
    {
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompHealth>()->value);
        game::gRenderer.GuiChangeText("healthBar", buf);
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompStamina>()->value);
        game::gRenderer.GuiChangeText("staminaBar", buf);
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompMana>()->value);
        game::gRenderer.GuiChangeText("manaBar", buf);

        CompOwnedItems *items = ent->GetComp<CompOwnedItems>();
        int i = 0;
        for (auto item : items->weapons)
        {
            std::snprintf(buf, bufSize, "itm%d", i);
            game::gRenderer.GuiChangeTile(buf, item->tileId);
            ++i;
        }
        if (items->curWeaponId >= 0)
            game::gRenderer.GuiChangeTile("weaponBar", items->weapons[items->curWeaponId]->tileId);
    });
    game::gRenderer.DrawGui(0, INT_MAX);
}

void GameBackgroundGuiRenderSys::OnUpdate()
{
    game::gRenderer.DrawGui(INT_MIN, -1);
}
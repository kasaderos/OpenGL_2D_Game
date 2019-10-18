#pragma once

#include <sfml/Graphics.hpp>

#include "stl.h"
#include "Utils.h"

namespace game
{

struct ITiledSurface
{
public:
    // Pos of the bottom left cornder
    // in world coordinates (measured in tiles).
    virtual sf::Vector2i GetPos() const = 0;
    virtual sf::Vector2u GetSize() const = 0;
    // Should return value in global coordinates.
    // (pattern[(x, y) - pos])
    virtual unsigned GetTileId(int x, int y) const = 0;
};

class Renderer
{
public:
    // All texture coordinates and coordinates of GUI elements are measured in pixels.
    // World coordinates are measured in tiles.

    // Texture coordinates and sizes may be negative.

    // When render world tiles, Y axis is directed up.
    // In other cases Y axis is directed down.

    // Window shoule be already active on the current thread.
    bool Init(const stl::shared_ptr<sf::RenderWindow> &window);
    void Release();

    int AddTexture(const char *filename); // Return texture id or -1 on error.
    int AddTexture(const sf::Image &img); // Return texture id or -1 on error.
    
    // 0 id is reserved for empty transparent tile.

    // Returns tile id or -1 on error.
    int AddTile(unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize);
    int AddTile(unsigned texId);
    int AddTile(const sf::Color &color);

    sf::Vector2f atonw(sf::Vector2i absCoord);
    sf::Vector2f atont(sf::Vector2i absCoord, unsigned tid); // No checks on texture validity.
    sf::Vector2i ntoaw(sf::Vector2f normCoord);
    sf::Vector2i ntoat(sf::Vector2f normCoord, unsigned tid); // No checks on texture validity.

    // If a GUI element have been already added, it's replaced by the new element.

    bool GuiAddTexturedRect(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize);
    bool GuiAddTexturedRect(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, unsigned texId);
    bool GuiAddColoredRect(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, const sf::Color &color);
    bool GuiAddText(const char *name, int layer, sf::Vector2i pos, const char *text,
        unsigned charSize = 30, const sf::Color &color = sf::Color(0xFFFFFFFF));
    bool GuiAddTile(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, unsigned tileId);

    void GuiChangeLayer(const char *elemName, int layer);
    void GuiChangePos(const char *elemName, sf::Vector2i pos);
    void GuiChangeSize(const char *elemName, sf::Vector2i size); // For text x component is taken.
    void GuiChangeTexture(const char *elemName, unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize);
    void GuiChangeTexture(const char *elemName, unsigned texId);
    void GuiChangeColor(const char *elemName, sf::Color color);
    void GuiChangeText(const char *elemName, const char *text);
    void GuiChangeTile(const char *elemName, unsigned tileId);

    void GuiRemoveElement(const char *elemName);

    void BeginDraw(sf::Color clearColor);
    void BeginDrawWorld();
    void EndDraw();
    // Draws world tiles in world viewport.
    // There is no z-buffer, so take care of the draw ordering.
    void DrawTiledSurface(const ITiledSurface &surface);
    void DrawSingleTile(unsigned tileId, sf::Vector2f pos);
    void DrawGui();
    void DrawGui(int minLayer, int maxLayer);

    // Sets area in which tiled surfaces are drawn.
    void SetCameraViewport(int x, int y, unsigned width, unsigned height);
    
    // Pos measured in tiles.
    void SetCameraCenterPos(sf::Vector2f pos);

    // Measured in tiles.
    void SetVisionAreaSize(sf::Vector2f size);
    
    // Sets tile which will fill space, which a tiled surface doesn't cover in the viewport.
    void SetOuterFillTile(unsigned tileId) { outerTileId = tileId; } // 0 by default.

    bool ChangeTileTexture(unsigned tileId, unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize);
    bool ChangeTileTexture(unsigned tileId, unsigned texId);

    void ClearGui();
    void ClearTextures();
    void ClearTiles();
    void ClearAll();

private:

    struct GuiTile
    {
        unsigned tileId;
        sf::Vector2i pos;
        sf::Vector2i size;
    };

    struct GUI
    {
        stl::vector<stl::unique_ptr<sf::RectangleShape>> commonElements;
        stl::vector<stl::unique_ptr<sf::Text>> textElements;
        stl::vector<GuiTile> tileElements;

        struct ElemRec
        {
            enum { COMMON, TEXT, TILE };
            uint16_t type = 0xFFFF;
            uint16_t id = 0;
            int layer = 0;
        };
        stl::map<stl::string, ElemRec> nameMap;
        stl::multimap<int, ElemRec> layerMap;

        bool AddCommonElement(const char *name, int layer, const sf::RectangleShape &elem);
        bool AddTextElement(const char *name, int layer, const sf::Text &elem);
        bool AddTileElement(const char *name, int layer, const GuiTile &elem);
        void RemoveElement(const char *name);

        void Clear();

    private:

        stl::vector<unsigned> freeCommonIds;
        stl::vector<unsigned> freeTextIds;
        stl::vector<unsigned> freeTileIds;
    };

    stl::shared_ptr<sf::RenderWindow> window;
    stl::vector<stl::unique_ptr<sf::Texture>> textures;
    stl::vector<stl::unique_ptr<sf::RectangleShape>> tiles;
    GUI gui;

    sf::View worldView;
    sf::View guiView;

    unsigned outerTileId = 0;
    bool isWorldViewCurrent = false;

    sf::Font font;
    sf::Vector2u winSize;
};

extern Renderer gRenderer;

}
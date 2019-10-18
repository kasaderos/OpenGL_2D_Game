#include "Renderer.h"

#include <iostream>

namespace game
{

Renderer gRenderer;

bool Renderer::Init(const stl::shared_ptr<sf::RenderWindow> &window)
{
    this->window = window;
    winSize = window->getSize();
    SetCameraViewport(0, 0, winSize.x, winSize.y);
    SetCameraCenterPos(sf::Vector2f(0.0f, 0.0f));
    SetVisionAreaSize(sf::Vector2f(16.0f, 16.0f));

    sf::RectangleShape emptyRect;
    emptyRect.setSize(sf::Vector2f(1.0f, 1.0f));
    emptyRect.setFillColor(sf::Color(0x0));
    tiles.push_back(stl::make_unique<sf::RectangleShape>(emptyRect));
    
    const char *fontname = "courbd.ttf";
    if (!font.loadFromFile(fontname))
    {
        std::cout << "Falied to load " << fontname << std::endl;
        return false;
    }

    return true;
}

void Renderer::Release()
{
    ClearAll();
    window.reset();
}

int Renderer::AddTexture(const char *filename)
{
    sf::Texture tex;
    if (!tex.loadFromFile(filename))
    {
        std::cout << "Failed to load " << filename << std::endl;
        return -1;
    }
    tex.setRepeated(true);
    textures.push_back(stl::make_unique<sf::Texture>(tex));
    return textures.size() - 1;
}

int Renderer::AddTexture(const sf::Image &img)
{
    sf::Texture tex;
    if (!tex.loadFromImage(img))
        return -1;
    tex.setRepeated(true);
    textures.push_back(stl::make_unique<sf::Texture>(tex));
    return textures.size() - 1;
}

int Renderer::AddTile(unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize)
{
    if (texId >= textures.size())
        return -1;
    sf::RectangleShape tile;
    tile.setTexture(textures[texId].get());
    tpos.y += tsize.y;
    tsize.y *= -1;
    tile.setTextureRect(sf::IntRect(tpos, tsize));
    tile.setSize(sf::Vector2f(1.0f, 1.0f));
    tiles.push_back(stl::make_unique<sf::RectangleShape>(tile));
    return tiles.size() - 1;
}

int Renderer::AddTile(unsigned texId)
{
    if (texId >= textures.size())
        return -1;
    sf::RectangleShape tile;
    sf::Texture *tex = textures[texId].get();
    tile.setTexture(tex);
    int w = tex->getSize().x;
    int h = tex->getSize().y;
    tile.setTextureRect(sf::IntRect(0, h, w, -h));
    tile.setSize(sf::Vector2f(1.0f, 1.0f));
    tiles.push_back(stl::make_unique<sf::RectangleShape>(tile));
    return tiles.size() - 1;
}

int Renderer::AddTile(const sf::Color &color)
{
    sf::RectangleShape tile;
    tile.setFillColor(color);
    tile.setSize(sf::Vector2f(1.0f, 1.0f));
    tiles.push_back(stl::make_unique<sf::RectangleShape>(tile));
    return tiles.size() - 1;
}

sf::Vector2f Renderer::atonw(sf::Vector2i absCoord)
{
    return sf::Vector2f(float(absCoord.x) / winSize.x, float(absCoord.y) / winSize.y);
}

sf::Vector2f Renderer::atont(sf::Vector2i absCoord, unsigned tid)
{
    sf::Vector2u size = textures[tid]->getSize();
    return sf::Vector2f(float(absCoord.x) / size.x, float(absCoord.y) / size.y);
}

sf::Vector2i Renderer::ntoaw(sf::Vector2f normCoord)
{
    return sf::Vector2i(int(normCoord.x * winSize.x), int(normCoord.y * winSize.y));
}

sf::Vector2i Renderer::ntoat(sf::Vector2f normCoord, unsigned tid)
{
    sf::Vector2u size = textures[tid]->getSize();
    return sf::Vector2i(int(normCoord.x * size.x), int(normCoord.y * size.y));
}

bool Renderer::GuiAddTexturedRect(const char *name, int layer, sf::Vector2i pos,
    sf::Vector2i size, unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize)
{
    if (texId >= textures.size())
        return false;

    sf::RectangleShape rect;
    rect.setTexture(textures[texId].get());
    rect.setTextureRect(sf::IntRect(tpos, tsize));
    rect.setPosition(sf::Vector2f(pos));
    rect.setSize(sf::Vector2f(size));
    gui.AddCommonElement(name, layer, rect);
    return true;
}

bool Renderer::GuiAddTexturedRect(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, unsigned texId)
{
    if (texId >= textures.size())
        return false;

    sf::RectangleShape rect;
    rect.setTexture(textures[texId].get());
    rect.setPosition(sf::Vector2f(pos));
    rect.setSize(sf::Vector2f(size));
    gui.AddCommonElement(name, layer, rect);
    return true;
}

bool Renderer::GuiAddColoredRect(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, const sf::Color &color)
{
    sf::RectangleShape rect;
    rect.setFillColor(color);
    rect.setPosition(sf::Vector2f(pos));
    rect.setSize(sf::Vector2f(size));
    gui.AddCommonElement(name, layer, rect);
    return true;
}

bool Renderer::GuiAddText(const char *name, int layer, sf::Vector2i pos,
    const char *text, unsigned charSize, const sf::Color &color)
{
    sf::Text t;
    t.setCharacterSize(charSize);
    t.setFillColor(color);
    t.setPosition(sf::Vector2f(pos));
    t.setString(text);
    t.setFont(font);
    gui.AddTextElement(name, layer, t);
    return true;
}

bool Renderer::GuiAddTile(const char *name, int layer, sf::Vector2i pos, sf::Vector2i size, unsigned tileId)
{
    GuiTile tile;
    tile.pos = pos;
    tile.size = size;
    tile.tileId = tileId;
    gui.AddTileElement(name, layer, tile);
    return true;
}

void Renderer::GuiChangeLayer(const char *elemName, int layer)
{
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = gui.nameMap[elemName];
        auto inLayerElems = gui.layerMap.equal_range(rec.layer);
        for (auto itr = inLayerElems.first; itr != inLayerElems.second; ++itr)
        {
            if (itr->second.id == rec.id && itr->second.type == rec.type)
            {
                gui.layerMap.erase(itr);
                break;
            }
        }
        gui.layerMap.emplace(layer, rec);
    }
}

void Renderer::GuiChangePos(const char *elemName, sf::Vector2i pos)
{
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::COMMON: gui.commonElements[rec.id]->setPosition(sf::Vector2f(pos)); break;
        case GUI::ElemRec::TEXT: gui.textElements[rec.id]->setPosition(sf::Vector2f(pos)); break;
        case GUI::ElemRec::TILE: gui.tileElements[rec.id].pos = pos; break;
        default: break;
        }
    }
}

void Renderer::GuiChangeSize(const char *elemName, sf::Vector2i size)
{
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::COMMON: gui.commonElements[rec.id]->setSize(sf::Vector2f(size)); break;
        case GUI::ElemRec::TEXT: gui.textElements[rec.id]->setCharacterSize(size.x); break;
        case GUI::ElemRec::TILE: gui.tileElements[rec.id].size = size; break;
        default: break;
        }
    }
}

void Renderer::GuiChangeTexture(const char *elemName, unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize)
{
    if (texId >= textures.size())
        return;
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::COMMON:
            gui.commonElements[rec.id]->setTexture(textures[texId].get());
            gui.commonElements[rec.id]->setTextureRect(sf::IntRect(tpos, tsize));
            break;
        default: break;
        }
    }
}

void Renderer::GuiChangeTexture(const char *elemName, unsigned texId)
{
    if (texId >= textures.size())
        return;
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::COMMON:
            gui.commonElements[rec.id]->setTexture(textures[texId].get());
            break;
        default: break;
        }
    }
}

void Renderer::GuiChangeColor(const char *elemName, sf::Color color)
{
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::COMMON: gui.commonElements[rec.id]->setFillColor(color); break;
        case GUI::ElemRec::TEXT: gui.textElements[rec.id]->setFillColor(color); break;
        default: break;
        }
    }
}

void Renderer::GuiChangeText(const char *elemName, const char *text)
{
    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::TEXT: gui.textElements[rec.id]->setString(text); break;
        default: break;
        }
    }
}

void Renderer::GuiChangeTile(const char *elemName, unsigned tileId)
{
    if (tileId >= tiles.size())
        return;

    auto elemRec = gui.nameMap.find(elemName);
    if (elemRec != gui.nameMap.end())
    {
        GUI::ElemRec rec = elemRec->second;
        switch (rec.type)
        {
        case GUI::ElemRec::TILE: gui.tileElements[rec.id].tileId = tileId; break;
        default: break;
        }
    }
}

void Renderer::GuiRemoveElement(const char *elemName)
{
    gui.RemoveElement(elemName);
}

void Renderer::BeginDraw(sf::Color clearColor)
{
    sf::Vector2u wSize = window->getSize();
    guiView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    guiView.setSize(sf::Vector2f(wSize));
    guiView.setCenter(sf::Vector2f(wSize) / 2.0f);
    window->clear(clearColor);
}

void Renderer::BeginDrawWorld()
{
    window->setView(worldView);
}

void Renderer::EndDraw()
{
    window->display();
}

void Renderer::DrawTiledSurface(const ITiledSurface &surface)
{
    sf::Vector2f absSize;
    absSize.x = abs(worldView.getSize().x);
    absSize.y = abs(worldView.getSize().y);
    sf::Vector2i bottomLeft = sf::Vector2i(worldView.getCenter() - absSize / 2.0f);
    sf::Vector2i topRight = sf::Vector2i(worldView.getCenter() + absSize / 2.0f);

    sf::Vector2i spos = surface.GetPos();
    sf::Vector2u ssize = surface.GetSize();

    for (int i = bottomLeft.x; i <= topRight.x; ++i)
        for (int j = bottomLeft.y; j <= topRight.y; ++j)
        {
            if (i >= spos.x && i < spos.x + ssize.x && j >= spos.y && j <= spos.y + ssize.y)
            {
                unsigned tileId = surface.GetTileId(i, j);
                if (tileId)
                {
                    tiles[tileId]->setPosition(sf::Vector2f(i, j));
                    window->draw(*tiles[tileId]);
                }
            }
            else if (outerTileId)
            {
                tiles[outerTileId]->setPosition(sf::Vector2f(i, j));
                window->draw(*tiles[outerTileId]);
            }
        }
}

void Renderer::DrawSingleTile(unsigned tileId, sf::Vector2f pos)
{
    tiles[tileId]->setPosition(pos);
    window->draw(*tiles[tileId]);
}

void Renderer::DrawGui()
{
    DrawGui(INT_MIN, INT_MAX);
}

void Renderer::DrawGui(int minLayer, int maxLayer)
{
    window->setView(guiView);

    for (auto &recPair : gui.layerMap)
    {
        if (recPair.first < minLayer || recPair.first > maxLayer)
            continue;
        unsigned id = recPair.second.id;
        GuiTile gt;
        switch (recPair.second.type)
        {
        case GUI::ElemRec::COMMON:
            window->draw(*gui.commonElements[id]);
            break;
        case GUI::ElemRec::TEXT:
            window->draw(*gui.textElements[id]);
            break;
        case GUI::ElemRec::TILE:
            gt = gui.tileElements[id];
            tiles[gt.tileId]->setPosition(sf::Vector2f(gt.pos));
            tiles[gt.tileId]->setScale(sf::Vector2f(gt.size));
            window->draw(*tiles[gt.tileId]);
            tiles[gt.tileId]->setScale(sf::Vector2f(1.0f, 1.0f));
            break;
        default: break;
        }
    }
}

void Renderer::SetCameraViewport(int x, int y, unsigned width, unsigned height)
{
    sf::Vector2f wSize = sf::Vector2f(window->getSize());
    worldView.setViewport(sf::FloatRect(x / wSize.x, y / wSize.y, width / wSize.x, height / wSize.y));
    //worldView.setSize(float(width), -float(height));
}

void Renderer::SetCameraCenterPos(sf::Vector2f pos)
{
    worldView.setCenter(pos);
}

void Renderer::SetVisionAreaSize(sf::Vector2f size)
{
    worldView.setSize(size.x, -size.y);
}

bool Renderer::ChangeTileTexture(unsigned tileId, unsigned texId, sf::Vector2i tpos, sf::Vector2i tsize)
{
    if (tileId >= tiles.size() || texId >= textures.size())
        return false;

    tiles[tileId]->setTexture(textures[texId].get());
    tiles[tileId]->setTextureRect(sf::IntRect(tpos, tsize));

    return true;
}

bool Renderer::ChangeTileTexture(unsigned tileId, unsigned texId)
{
    if (tileId >= tiles.size() || texId >= textures.size())
        return false;

    tiles[tileId]->setTexture(textures[texId].get());

    return true;
}

void Renderer::ClearGui()
{
    gui.Clear();
}

void Renderer::ClearTextures()
{
    textures.clear();
}

void Renderer::ClearTiles()
{
    tiles.clear();
}

void Renderer::ClearAll()
{
    ClearGui();
    ClearTextures();
    ClearTiles();
}

bool Renderer::GUI::AddCommonElement(const char *name, int layer, const sf::RectangleShape &elem)
{
    ElemRec rec;
    rec.type = ElemRec::COMMON;
    rec.id = InsertStable(stl::make_unique<sf::RectangleShape>(elem), commonElements, freeCommonIds);
    rec.layer = layer;
    nameMap.emplace(name, rec);
    layerMap.emplace(layer, rec);
    return true;
}

bool Renderer::GUI::AddTextElement(const char *name, int layer, const sf::Text &elem)
{
    ElemRec rec;
    rec.type = ElemRec::TEXT;
    rec.id = InsertStable(stl::make_unique<sf::Text>(elem), textElements, freeTextIds);
    rec.layer = layer;
    nameMap.emplace(name, rec);
    layerMap.emplace(layer, rec);
    return true;
}

bool Renderer::GUI::AddTileElement(const char *name, int layer, const GuiTile &elem)
{
    ElemRec rec;
    rec.type = ElemRec::TILE;
    rec.id = InsertStable(elem, tileElements, freeTileIds);
    rec.layer = layer;
    nameMap.emplace(name, rec);
    layerMap.emplace(layer, rec);
    return true;
}

void Renderer::GUI::RemoveElement(const char *name)
{
    auto elem = nameMap.find(name);
    if (elem != nameMap.end())
    {
        ElemRec rec = elem->second;
        nameMap.erase(elem);
        auto layer = layerMap.equal_range(rec.layer);
        for (auto itr = layer.first; itr != layer.second; ++itr)
        {
            if (itr->second.id == rec.id && itr->second.type == rec.type)
            {
                layerMap.erase(itr);
                break;
            }
        }
        switch (rec.type)
        {
        case ElemRec::COMMON: RemoveStable(rec.id, commonElements, freeCommonIds); break;
        case ElemRec::TEXT: RemoveStable(rec.id, textElements, freeTextIds); break;
        case ElemRec::TILE: RemoveStable(rec.id, tileElements, freeTileIds); break;
        default: break;
        }
    }
}

void Renderer::GUI::Clear()
{
    commonElements.clear();
    textElements.clear();
    tileElements.clear();
    nameMap.clear();
    layerMap.clear();
    freeCommonIds.clear();
    freeTextIds.clear();
    freeTileIds.clear();
}

}
#pragma once

#include "stl.h"
#include <sfml/Graphics.hpp>
#include "Renderer.h"

namespace game
{

class GameLevel
{

};

struct GameResults
{
public:
    enum CompleteStatus
    {
        FAILED,
        COMPLETE
    }
    status;
};

class Game
{
public:

    void Init(const stl::shared_ptr<sf::RenderWindow> &window);
    void Release();

    //void SetLevel(const GameLevel &level);
    GameResults StartLevel(const char *levelFilename);

protected:

    //stl::shared_ptr<sf::RenderWindow> window;
    //stl::unique_ptr<Renderer> renderer;
    stl::map<stl::string, unsigned> texNames;

    void InitDefaultGui();
};

}
#define _CRT_SECURE_NO_WARNINGS

#include "Event.h"

#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>

#include "stl.h"
#include "ECS.h"
#include "EcsEntities.h"
#include "EcsSystems.h"

#include <windows.h>

#include <sfml/Graphics.hpp>
//#include <sfml/OpenGL.hpp>
#include "Game.h"
#include "Utils.h"
#include "Timer.h"

Timer gTime;

#ifdef _WIN32
#pragma comment (lib, "sfml-graphics.lib")
#pragma comment (lib, "sfml-window.lib")
#pragma comment (lib, "sfml-system.lib")
#endif

//#define MAX_LAYER 1000
//#define MIN_LAYER -1000

void GameThread(std::shared_ptr<sf::RenderWindow> window)
{
    gTime.Reset();
    //window->setActive(true);
    //glDepthRange(MAX_LAYER, MIN_LAYER);
    game::Game g;
    g.Init(window);
    g.StartLevel("debugLevel.lvl");
    g.Release();
}

void EventThread(std::shared_ptr<sf::RenderWindow> window)
{
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            game::SendEvent(event);
            if (event.type == sf::Event::Closed)
                window->close();
        }
    }
}

struct Pos
{
    int x, y;
};

int main()
{
    //bool success;
    //ecs::EntityDesc desc = ecs::gEntityManager.CreateEntityByName("Player", success);
    //if (!success)
    //    return 1;
    //ecs::IEntity *ent = ecs::gEntityManager.GetEntityByDesc(desc);
    //ent->SetComp("CompHealth", "50");
    //std::cout << ent->GetComp<CompHealth>()->value << std::endl;

    ecs::EntityManager::AddRegisteredSystems();

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    //settings.depthBits = 32;

    std::shared_ptr<sf::RenderWindow> window =
        std::make_shared<sf::RenderWindow>(
            sf::VideoMode(1000, 840), "CG/2",
            //sf::Style::Default,
            sf::Style::Titlebar | sf::Style::Close,
            settings
            );

    game::SendEvent<game::EventRenderWindowWasCreated>({ window });

    sf::Thread game(GameThread, window);
    window->setActive(false);
    game.launch();
    EventThread(window);
    game.wait();

    //ecs::gEntityManager.CreateEntity<game::Player>();// game::Pos{ 0, 0 }, game::Health{ 100.0 });
    //ecs::gEntityManager.CreateEntity<game::Brick>(game::Pos{0, 0});
    //ecs::gEntityManager.CreateEntity<game::PlayerUpdater>();
    //while (true)
    //{
    //    ecs::gEntityManager.UpdateEntities();
    //    Sleep(500);
    //}
}
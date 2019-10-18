#pragma once

#include "Event.h"
#include <sfml/Window.hpp>

namespace game
{

typedef sf::Event EventSfml;
struct EventRenderWindowWasCreated { stl::shared_ptr<sf::RenderWindow> wnd; };
struct EventRendererWasInited {};

}
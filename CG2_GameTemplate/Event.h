#pragma once

#include "stl.h"

namespace game
{

template <class EType>
void SendEvent(const EType &evt);

template <class EType>
class EventListener
{
public:
    EventListener() { listeners.emplace(this); }
    virtual ~EventListener() { listeners.erase(this); }
    virtual void OnEvent(const EType &evt) = 0;
private:
    friend void SendEvent<EType>(const EType &evt);
    static stl::set<EventListener<EType> *> listeners;
};

template <class EType>
stl::set<EventListener<EType> *> EventListener<EType>::listeners;

template <class EType>
void SendEvent(const EType &evt)
{
    for (auto &listener : EventListener<EType>::listeners)
        listener->OnEvent(evt);
}

template <class EType>
class CallbackEventListener : private EventListener<EType>
{
public:
    CallbackEventListener(void (*handler)(const EType&)) { this->handler = handler; }
    bool operator<(const CallbackEventListener &listener) const { return handler < listener.handler; }
private:
    void OnEvent(const EType &evt) override { handler(evt); }
    void (*handler)(const EType&);
};

template <class EType>
void RegisterEventHandler(void (*handler)(const EType&))
{
    static stl::set<CallbackEventListener<EType>> listeners;
    listeners.emplace(handler);
}

}
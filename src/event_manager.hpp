#pragma once

#include "event.hpp"

#include <vector>
#include <functional>
#include <typeinfo>
#include <unordered_map>
#include <algorithm>

using namespace std;

class IEventHandlerArray { };

template <typename T>
class EventHandlerArray : public IEventHandlerArray
{
public:
    void add_handler(function<void(T)> handler)
    {
        handlers.push_back(handler);
    }

    void post(const T& ev)
    {
        for (auto& handler : handlers)
        {
            handler(ev);
        }
    }

private:
    vector<function<void(T)>> handlers;
};

class EventManager
{
public:
    EventManager() 
    {

    }

    static shared_ptr<EventManager> get_instance()
    {
        return instance;
    }

    template <typename T>
    void add_handler(function<void(T)> handler)
    {
        EventID event_id = EvType<T>::get_id();

        // Initialize handler array if not already done
        if (!handler_store[event_id])
        {
            handler_store[event_id] = make_shared<EventHandlerArray<T>>();
        }

        auto handler_array = static_pointer_cast<EventHandlerArray<T>>(handler_store[event_id]);
        handler_array->add_handler(handler);
    }

    template <typename T>
    void post(T&& ev)
    {
        EventID event_id = EvType<T>::get_id();
        if (handler_store.find(event_id) != handler_store.end())
        {
            auto handlers = static_pointer_cast<EventHandlerArray<T>>(handler_store[event_id]);
            handlers->post(ev);
        }
    }

private:
    static shared_ptr<EventManager> instance;
    unordered_map<EventID, shared_ptr<IEventHandlerArray>> handler_store;
};

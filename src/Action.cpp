/**
 * @file   Action.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Sun Jun  3 10:54:22 2012
 *
 * @brief
 *
 *
 */

#include "Action.h"

#include <iostream>
#include "QQDebug.h"

int Action::n_actions = 0;

Adapter::Adapter()
{

}

Adapter::~Adapter()
{
    if ( ! event_map.empty())
    {
        event_map.clear();
    }
}

void Adapter::trigger( const QQEvent &event, const std::string data)
{
    if ( ! is_event_registered(event))
    {
        debug_error("Event is not registered ... (%s,%d)", __FILE__, __LINE__ );
        return;
    }
    Action action = event_map[event];
    Caller caller ;
    caller.setAction(&action);
    caller.call(data);
}

void Adapter::register_event_handler(QQEvent event, EventListener el)
{
    if ( event_map.count(event) != 0 )
    {
        debug_info("An event handler has been loaded, reload new handler. (%s,%d)", __FILE__, __LINE__);
        //delete event_map[event];
        event_map[event] = Action(el);
        return ;
    }
    event_map[event] = Action(el);
    debug_info("Size of event map is %d", event_map.size());
    debug_info("Register event handler success. (%s,%d)", __FILE__, __LINE__);
}

bool Adapter::is_event_registered( const QQEvent & event)
{
    return event_map.count( event) != 0;
}

void Adapter::delete_event_handler(QQEvent event)
{
    if ( event_map.count(event) == 0 )
    {
        debug_info( "Unregistered event handle! (%s,%d)" );
        return ;
    }

    std::map<QQEvent, Action>::iterator it;
    it = event_map.find(event);
    //delete it->second;
    event_map.erase(it);
    debug_info("Delete event handler success. (%s,%d)", __FILE__, __LINE__);
}

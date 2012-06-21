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

Adapter::Adapter()
{

}

Adapter::~Adapter()
{
    if ( ! event_map.empty())
    {
        for(  std::map<QQEvent, void *>::iterator it = event_map.begin() ; it != event_map.end(); it ++)
        {
            delete it->second;
            it->second = NULL;
        }
    }
}

void Adapter::register_event_handler(QQEvent event, void * ptr)
{
    if ( event_map.count(event) != 0 )
    {
        debug_info("An event handler has been loaded, reload new handler. (%s,%d)", __FILE__, __LINE__);
        delete event_map[event];
        event_map[event] = ptr;
        return ;
    }
    event_map[event] = ptr;
    debug_info("Register event handler success. (%s,%d)", __FILE__, __LINE__);
}

void Adapter::delete_event_hander(QQEvent event)
{
    if ( event_map.count(event) == 0 )
    {
        debug_info( "Unregistered event handle! (%s,%d)" );
        return ;
    }

    std::map<QQEvent, void *>::iterator it;
    it = event_map.find(event);
    delete it->second;
    it->second = NULL;
    event_map.erase(it);
    debug_info("Delete event handler success. (%s,%d)", __FILE__, __LINE__);
}

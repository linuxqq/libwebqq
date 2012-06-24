/**
 * @file   Action.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Sun Jun  3 10:46:11 2012
 *
 * @brief
 *
 *
 */

#ifndef __ACTION_H__
#define __ACTION_H__
#include <map>
#include "QQTypes.h"
#include <iostream>
#include "SmartPtr.h"
#include <string>
class Action
{
    std::string data;
public:
    virtual void run()=0;
    Action (){
        n_actions++;
    }
    virtual void load(const std::string & data)
    {
        this->data = data;
    }

    virtual ~Action(){ std::cout<<"Destruct Action"<<std::endl; n_actions --; }
    static int n_actions;
};

class Caller {
private:
    Action *_callback;
public:
    Caller(): _callback(0) {}
    ~Caller() { delAction(); }
    void delAction() { delete _callback; _callback = 0; }
    void setAction(Action*cb) { delAction(); _callback = cb; }
    void call() { if (_callback)  _callback -> run() ;}
};

class Adapter{
    std::map<QQEvent, SmartPtr<Action> > event_map;

public:
    Adapter();
    ~Adapter();
    void trigger( const QQEvent &event, const std::string data);
    void register_event_handler(QQEvent event, SmartPtr<Action> action);
    bool is_event_registered(const QQEvent & event);
    void delete_event_handler(QQEvent event);
};
#endif

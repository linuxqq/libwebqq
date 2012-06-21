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

template<class T>
class Action
{
    T  data;
public:
    typedef  T DataType;
    virtual void run()=0;
    virtual ~Action(){}
    virtual void load(T data){
        this->data = data;
    }
};

template<class T>
class Caller {
private:
    Action<T> *_callback;
public:
    Caller(): _callback(0) {}
    ~Caller() { delAction(); }
    void delAction() { delete _callback; _callback = 0; }
    void setAction(Action<T> *cb) { delAction(); _callback = cb; }
    void call() { if (_callback)  _callback -> run() ;}
};

class Adapter{
    std::map<QQEvent, void *> event_map;
public:
    Adapter();
    ~Adapter();
    template<class T>
    void trigger(QQEvent event, T data){
        if ( event_map.count( event) == 0)
        {
            return;
        }
        else{
            Action<T> * ptr = reinterpret_cast<Action<T> >(event_map[event]);
            ptr->load(data);
            Caller<T> caller ;
            caller.setAction(ptr);
            caller.call();
        }
    }
    void register_event_handler(QQEvent event, void * action);
    void delete_event_hander(QQEvent event);
};
#endif

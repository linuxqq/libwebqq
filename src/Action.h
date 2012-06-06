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



class Action
{
public:
    virtual void run()=0;
    virtual ~Action(){}
};

class Caller {
private:
    Action *_callback;
public:
    Caller(): _callback(0) {}
    ~Caller() { delAction(); }
    void delAction() { delete _callback; _callback = 0; }
    void setAction(Action *cb) { delAction(); _callback = cb; }
    void call() { if (_callback)  _callback -> run() ;}
};

#endif

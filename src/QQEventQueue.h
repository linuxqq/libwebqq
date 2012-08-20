/**
 * @file   QQEventQueue.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jul 25 11:32:10 2012
 *
 * @brief
 *
 *
 */


#ifndef __QQ_EVENT_QUEUE__
#define __QQ_EVENT_QUEUE__

#include <queue>
#include "QQTypes.h"
#include <utility>
#include <string>
#include "Thread.h"

class QQEventQueue{

    std::queue< std::pair<QQEvent , std::string > > event_queue;

    ThreadPool::TMutex mutex;

    const size_t KMaxSize;

public:

    QQEventQueue();

    QQEventQueue ( const QQEventQueue &);

    QQEventQueue operator=(const QQEventQueue&);

    QQEventQueue( size_t max_size);

    std::pair<QQEvent , std::string> pop();

    void push( std::pair <QQEvent, std::string >);

    size_t size();

    bool empty();
};

#endif

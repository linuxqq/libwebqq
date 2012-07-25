/**
 * @file   QQEventQueue.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jul 25 12:17:16 2012
 *
 * @brief
 *
 *
 */

#include "QQEventQueue.h"
#include "QQDebug.h"

QQEventQueue::QQEventQueue():KMaxSize(1000)
{

}


QQEventQueue::QQEventQueue(size_t max ):KMaxSize(max)
{

}

QQEventQueue::QQEventQueue(const QQEventQueue & other):KMaxSize(other.KMaxSize)
{
    this->event_queue  = other.event_queue;
}

QQEventQueue  QQEventQueue::operator=(const  QQEventQueue&other)
{
    QQEventQueue  queue(other);
    return queue;
}


void QQEventQueue::push(std::pair<QQEvent, std::string> item)
{
    ThreadPool::TScopedLock lock(mutex);
    if ( event_queue.size() >= KMaxSize)
    {
        debug_info("Event queue has acquired a max size , and will pop the front item ...(%s,%d)", __FILE__, __LINE__);
        event_queue.pop();
    }
    event_queue.push(item);
}


std::pair<QQEvent, std::string> QQEventQueue::pop()
{
    ThreadPool::TScopedLock lock(mutex);
    std::pair<QQEvent, std::string> item;
    if ( ! event_queue.empty())
    {
        item = event_queue.front();
        event_queue.pop();
    }
    return item;
}

std::size_t QQEventQueue::size()
{
    ThreadPool::TScopedLock lock(mutex);
    return event_queue.size();
}


bool QQEventQueue::empty()
{
    ThreadPool::TScopedLock lock(mutex);
    return event_queue.empty();
}

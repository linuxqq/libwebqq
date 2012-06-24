/**
 * @file   SmartPtr.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Sat Jun 23 22:34:59 2012
 *
 * @brief
 *
 *
 */
#ifndef __SMARTPTR_H_
#define __SMARTPTR_H_
template<class T> class SmartPtr
{
public:

    SmartPtr(T *realPtr = 0) { pointee = realPtr; }

    T *operator->() const {
        return pointee;
    }
    T &operator*() const {
        return *pointee;
    }
private:
    T *pointee;
};

#endif

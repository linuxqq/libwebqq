/**
 * @file   Singleton.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 17:46:17 2010
 *
 * @brief  Singleton desin pattern
 *
 *
 */

#ifndef __SINGLETON_H_
#define __SINGLETON_H_

template <typename T>
struct FriendMaker
{
    typedef T Type;
};

template <class T>
class Singleton
{
public:

//    friend class FriendMaker<T>::Type;

    static T *instance()
    //static method that returns only instance of MySingletone
    {
        if (m_pOnlyOneInstance == 0) // if not yet instantiated
        {
            m_pOnlyOneInstance = new T();
            //create one and only object
        }
        return m_pOnlyOneInstance;
    }

    static T * getInstance()
    {
        T * m_instance = Singleton<T>::instance();
        m_instance->reset();
        return m_instance;
    }
    virtual void reset()=0;

protected:

    static T * m_pOnlyOneInstance;
    //holds one and only object of MySingleton

    Singleton<T>(){}

};

template<class T>
T* Singleton<T>::  m_pOnlyOneInstance = 0;
#endif

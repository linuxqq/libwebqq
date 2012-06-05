#ifndef __TTHREAD_H_
#define __TTHREAD_H_


#include <cstdio>
#include <pthread.h>

namespace ThreadPool
{

    class TThread
    {
    protected:
        pthread_t  _thread_id;

        bool       _running;

        int        _thread_no;


    public:
        TThread ( const int thread_no = -1 );

        virtual ~TThread ();

        int thread_no () const { return _thread_no; }

        void set_thread_no ( const int  n );

        bool on_proc ( const int p ) const
        {
            return ((p == -1) || (_thread_no == -1) || (p == _thread_no));
        }

        virtual void run () = 0;

        void create ( const bool  detached = false,
                      const bool  sscope   = false );

        void detach ();

        void join   ();

        void cancel ();

    protected:
        void exit   ();

        void sleep  ( const double sec );

    public:

        void reset_running () { _running = false; }

    };


    class TMutex
    {
    protected:
        pthread_mutex_t      _mutex;
        pthread_mutexattr_t  _mutex_attr;


    public:

        TMutex ()
        {
            pthread_mutexattr_init( & _mutex_attr );
            pthread_mutex_init( & _mutex, & _mutex_attr );
        }

        ~TMutex ()
        {
            pthread_mutex_destroy( & _mutex );
            pthread_mutexattr_destroy( & _mutex_attr );
        }

        void  lock    () { pthread_mutex_lock(   & _mutex ); }

        void  unlock  () { pthread_mutex_unlock( & _mutex ); }

        bool is_locked ()
        {
            if ( pthread_mutex_trylock( & _mutex ) != 0 )
                return true;
            else
            {
                unlock();
                return false;
            }
        }
    };


    class TScopedLock
    {
    private:
        TMutex *  _mutex;

        TScopedLock ( TScopedLock & );
        void operator = ( TScopedLock & );


    public:
        explicit TScopedLock ( TMutex &  m )
        : _mutex( & m )
        {
            _mutex->lock();
        }

        ~TScopedLock ()
        {
            _mutex->unlock();
        }
    };

    class TCondition : public TMutex
    {
    private:
        pthread_cond_t  _cond;

    public:
        TCondition  () { pthread_cond_init(    & _cond, NULL ); }

        ~TCondition () { pthread_cond_destroy( & _cond ); }

        void wait      () { pthread_cond_wait( & _cond, & _mutex ); }

        void signal    () { pthread_cond_signal( & _cond ); }

        void broadcast () { pthread_cond_broadcast( & _cond ); }
    };

}

#endif

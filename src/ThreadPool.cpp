/**
 * @file   ThreadPool.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Tue Jun  5 18:24:39 2012
 *
 * @brief
 *
 *
 */

#include <pthread.h>

#include "ThreadPool.h"

namespace ThreadPool
{

    namespace
    {

#define THR_SEQUENTIAL  0

        TPool * thread_pool = NULL;

    }

    class TPoolThr : public TThread
    {
    protected:
        TPool *        _pool;

        TPool::TJob *  _job;
        void *         _data_ptr;

        bool           _del_job;

        TCondition     _work_cond;

        bool           _end;

        TMutex         _del_mutex;

    public:
        TPoolThr ( const int n, TPool * p )
            : TThread(n), _pool(p), _job(NULL), _data_ptr(NULL), _del_job(false), _end(false)
        {}

        ~TPoolThr () {}

        void run ()
        {
            TScopedLock  del_lock( _del_mutex );

            while ( ! _end )
            {

                _pool->append_idle( this );

                {
                    TScopedLock  work_lock( _work_cond );

                    while (( _job == NULL ) && ! _end )
                        _work_cond.wait();
                }



                if ( _job != NULL )
                {
                    _job->run( _data_ptr );
                    _job->unlock();

                    if ( _del_job )
                        delete _job;

                    TScopedLock  work_lock( _work_cond );

                    _job      = NULL;
                    _data_ptr = NULL;
                }
            }
        }

        void run_job  ( TPool::TJob * j, void * p, const bool del = false )
        {
            TScopedLock  lock( _work_cond );

            _job      = j;
            _data_ptr = p;
            _del_job  = del;

            _work_cond.signal();
        }

        TMutex & del_mutex  ()
        {
            return _del_mutex;
        }

        void quit ()
        {
            TScopedLock  lock( _work_cond );

            _end      = true;
            _job      = NULL;
            _data_ptr = NULL;

            _work_cond.signal();
        }
    };

    TPool::TPool ( const unsigned int  max_p )
    {

        _max_parallel = max_p;

        _threads = new TPoolThr*[ _max_parallel ];

        if ( _threads == NULL )
        {
            _max_parallel = 0;
            std::cerr << "(TPool) TPool : could not allocate thread array" << std::endl;
        }

        for ( unsigned int  i = 0; i < _max_parallel; i++ )
        {
            _threads[i] = new TPoolThr( i, this );

            if ( _threads == NULL )
                std::cerr << "(TPool) TPool : could not allocate thread" << std::endl;
            else
                _threads[i]->create( true, true );
        }
    }

    TPool::~TPool ()
    {
        sync_all();

        for ( unsigned int  i = 0; i < _max_parallel; i++ )
            _threads[i]->quit();

        for ( unsigned int  i = 0; i < _max_parallel; i++ )
        {
            _threads[i]->del_mutex().lock();
            delete _threads[i];
        }

        delete[] _threads;
    }

    void TPool::run ( TPool::TJob * job, void * ptr, const bool del )
    {
        if ( job == NULL )
            return;

#if THR_SEQUENTIAL == 1

        job->run( ptr );

        if ( del )
            delete job;

#else

        TPoolThr * thr = get_idle();

        job->lock();

        thr->run_job( job, ptr, del );
#endif
    }

    void  TPool::sync ( TJob * job )
    {
        if ( job == NULL )
            return;

        job->lock();
        job->unlock();
    }

    void  TPool::sync_all ()
    {
        while ( true )
        {
            {
                TScopedLock  lock( _idle_cond );

                // wait until next thread becomes idle
                if ( _idle_threads.size() < _max_parallel )
                    _idle_cond.wait();
                else
                {
                    break;
                }
            }
        }
    }

    TPoolThr *  TPool::get_idle ()
    {
        while ( true )
        {

            TScopedLock  lock( _idle_cond );

            while ( _idle_threads.empty() )
                _idle_cond.wait();


            if ( ! _idle_threads.empty() )
            {
                TPoolThr * t = _idle_threads.front();

                _idle_threads.pop_front();

                return t;
            }
        }
    }

    void TPool::append_idle ( TPoolThr * t )
    {
        TScopedLock  lock( _idle_cond );

        for ( std::list< TPoolThr * >::iterator  iter = _idle_threads.begin();
              iter != _idle_threads.end();
              ++iter )
        {
            if ( (*iter) == t )
            {
                return;
            }
        }

        _idle_threads.push_back( t );

        _idle_cond.signal();
    }


    void  init ( const unsigned int  max_p )
    {
        if ( thread_pool != NULL )
            delete thread_pool;

        if ((thread_pool = new TPool( max_p )) == NULL)
            std::cerr << "(init_thread_pool) could not allocate thread pool" << std::endl;
    }

    void run ( TPool::TJob * job, void * ptr, const bool del )
    {
        if ( job == NULL )
            return;

        thread_pool->run( job, ptr, del );
    }

    void sync ( TPool::TJob * job )
    {
        thread_pool->sync( job );
    }

    void  sync_all ()
    {
        thread_pool->sync_all();
    }

    void done ()
    {
        delete thread_pool;
    }

}

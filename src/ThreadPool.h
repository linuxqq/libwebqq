/**
 * @file   ThreadPool.h
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Tue Jun  5 18:22:50 2012
 *
 * @brief
 *
 *
 */

#ifndef __TTHREADPOOL_H_
#define __TTHREADPOOL_H_

#include <iostream>
#include <list>
#include "Thread.h"

namespace ThreadPool
{

    const int  NO_PROC = -1;

    class TPoolThr;

    class TPool
    {
        friend class TPoolThr;

    public:

        class TJob
        {
        protected:
            const int  _job_no;
            TMutex     _sync_mutex;

        public:
            TJob ( const int  n = NO_PROC )
                : _job_no(n)
            {}

            virtual ~TJob ()
            {
                if ( _sync_mutex.is_locked() )
                    std::cerr << "(TJob) destructor : job is still running!" << std::endl;
            }

            virtual void run ( void * ptr ) = 0;

            int  job_no () const { return _job_no; }
            void lock   () { _sync_mutex.lock(); }
            void unlock () { _sync_mutex.unlock(); }
            bool on_proc ( const int  p ) const
            {
                return ((p == NO_PROC) || (_job_no == NO_PROC) || (p == _job_no));
            }
        };

    protected:
        unsigned int             _max_parallel;
        TPoolThr **              _threads;
        std::list< TPoolThr * >  _idle_threads;
        TCondition               _idle_cond;

    public:
        TPool ( const unsigned int  max_p );

        ~TPool ();

        unsigned int  max_parallel () const { return _max_parallel; }

        void  run  ( TJob *      job,
                     void *      ptr = NULL,
                     const bool  del = false );

        void  sync ( TJob * job );

        void  sync_all ();

    protected:

        TPoolThr * get_idle ();

        void append_idle ( TPoolThr * t );
    };

    void  init      ( const unsigned int   max_p );

    void  run       ( TPool::TJob *        job,
                      void *               ptr = NULL,
                      const bool           del = false );

    void  sync      ( TPool::TJob *        job );

    void  sync_all  ();

    void  done      ();

}

#endif

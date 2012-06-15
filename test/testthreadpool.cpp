/**
 * @file   testthreadpool.cpp
 * @author Xiang Wang <xiang_wang@trendmicro.com.cn>
 * @date   Wed Jun  6 09:47:05 2012
 *
 * @brief
 *
 *
 */

#include "ThreadPool.h"
#include <iostream>
#include <cstdlib>

static int status = 1;

class TBenchJob1: public ThreadPool::TPool::TJob
{
public:
    virtual void run (void *){
        while(status)
        {
            std::cout<<"Test1"<<std::endl;
            sleep(1);
        }
    }
};

class TBenchJob2: public ThreadPool::TPool::TJob
{
public:
    virtual void run (void *){
    	while( 1)
	{
		std::cout<<"Test2"<<std::endl;
		sleep(1);
	}
    }
};

int main()
{

    ThreadPool::init(4);
    TBenchJob1 *job1 = new TBenchJob1 ();
    TBenchJob2 *job2 = new TBenchJob2();
    ThreadPool::run(job1, NULL, true);
    sleep(10);
    ThreadPool::run(job2, NULL, true);

    ThreadPool::sync_all();
    ThreadPool::done();
}

# Introduction
This Project aims at implementation of web qq protocol with C++ back end and other language extended which used boost-python and swig for high level wrapping.

Current, I have finished emesene integration with boost wrapper. Source code has been merged by emesene master branch.

# Dependency Packages
* pkg-config
* gcc-c++
* libtool
* boost-devel
* python-devel
* libcurl-devel

# Architecture

## Communication
Libwebqq is a HTTP based IM client engine. In order to get a higher performance of HTTP traffic exchanging with the Internet, I wrapped libcurl as a low level communication library. Web QQ HTTP traffic only use GET and POST method. In additional, Cookie manager is required for session management. After hand shake with web qq server , the cookie should be stored for further communication.

## Threading
A Light weight Thread Pool is implemented in libwebqq. The default count of threads is 8. We can not set this thread count too higher, or the system will pay a lot of effort on thread context exchanging. Usually , thread count is CPU count * 2 +1 , 8 is enough for consumer users to get high performance of task handling. Tasks are categorized in the module.

## Tasks
There are several kinds of tasks for deal with different tasks:

1. Fetch buddy misc information , long nick, memo etc.
1. Fetch buddy face,
1. Fetch qq group misc information.
1. Fetch qq group face.
1. Send buddy message.
1. Send group message.
1. Poll notification from remote web qq server.
1. Update Long Nick message.
1. Send shaking notification .

## Json Parser
Most communication data between webbqq client engine and webqq server is json format. I used jsoncpp as json parser and constructor.

## Event Queue
Event Queue is used for back end and front end communication which should be thread safe. Poll thread is always polling event data from webqq server , and fill the event queue. Anther front end thread is always pulling data from event queue and trigger callbacks registered as to related event. Event Queue is not necessary for your extension, cause for some other non-C/C++ family langue is not easy to extend on virtual functions in a class structure. Event queue is just an alternative choice.

## Wrapper
Boost and Swig are both wrappers for libwebqq . It depends on the programming language you used in your own project. Detail information of wrapper work , please refer to the code in wrapper folder of project hosted.

## Extensions
As a featured request, I have finished emesene extension . Next generation of emesene will support web qq session soon. Emesene is a wildly used instance message client. Here are some important component for your extension project.

### Boost or Swig
Both Boost-Python and Swig are excellent method for python C++ extensions. Use Boost python or swig is depends on your project that extends libwebqq.

If you use C++ family language -- QT, swig is better . You can define call back class structure implements Action Class.

> class Action
> {

>     EventListener _callback;

> public:
>     Action (){
>         n_actions++;
>     }

>     Action(const EventListener & cb ){
>         setCallback(cb);
>     }

>     virtual void operator()(std::string data) {
>         _callback(data);
>     }
>     void setCallback(const EventListener & cb){
>         _callback = cb;
>     }

>     const EventListener getCallback(){
>         return _callback;
>     }

>     virtual ~Action(){ std::cout<<"Destruct Action"<<std::endl; n_actions --; }
>     static int n_actions;
> };

If you used python as for extension language , boost-python is a good choice. Event Queue is accompanied with boost-python.

> class QQEventQueue{

>     std::queue< std::pair<QQEvent , std::string > > event_queue;

>     ThreadPool::TMutex mutex;

>     const size_t KMaxSize;

> public:

>     QQEventQueue();

>     QQEventQueue ( const QQEventQueue &);

>     QQEventQueue operator=(const QQEventQueue&);

>     QQEventQueue( size_t max_size);

>     std::pair<QQEvent , std::string> pop();

>     void push( std::pair <QQEvent, std::string >);

>     size_t size();

>     bool empty();
> };

No matter what kinds of extension method you chose, all of the list call back function related to the event should be implemented in your high level extension project.

> enum  QQEvent{

>     ON_BUDDY_MESSAGE = 512,

>     ON_GROUP_MESSAGE,

>     ON_SEND_MESSAGE,

>     ON_AVATAR_UPDATE,

>     ON_BUDDY_STATUS_CHANGE,

>     ON_NICK_CHANGE,

>     ON_SHAKE_MESSAGE

> };

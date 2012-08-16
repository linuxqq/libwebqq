lib_LTLIBRARIES=libwebqq.la
libwebqq_la_CPPFLAGS=-I../jsoncpp/include $(CURL_CFLAGS) $(CXXFLAGS)
libwebqq_la_LIBADD=$(CURL_LIBS) ../jsoncpp/src/libjsoncpp.a 
libwebqq_la_SOURCES= Thread.h Thread.cpp \
		     ThreadPool.h ThreadPool.cpp\
		     Singleton.h \
		     HttpClient.h HttpClient.cpp\
		     QQDebug.h QQDebug.cpp\
		     QQMD5.h QQMD5.cpp \
		     QQAction.h QQAction.cpp\
		     QQEventQueue.h QQEventQueue.cpp\
		     QQAuthentication.h QQAuthentication.cpp\
		     QQUtil.h QQUtil.cpp\
		     QQPlugin.h QQPlugin.cpp\
		     QQTask.h QQTask.cpp \
		     QQTaskManager.h QQTaskManager.cpp\
   		     QQTypes.h

AM_CXXFLAGS = -fPIC -ggdb -O0  -Wall -D_REENTRANT  -Wall -Wno-long-long  -Wpacked -Wundef -Wchar-subscripts  -pedantic -ansi 
AM_LDFLAGS =  



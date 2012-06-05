lib_LTLIBRARIES=libwebqq.la
libwebqq_la_CPPFLAGS=-I../jsoncpp/include  $(CURLPP_CFLAGS)
libwebqq_la_LIBADD=$(CURLPP_LIBS)  -L../jsoncpp/src/ -ljsoncpp
libwebqq_la_SOURCES= Thread.h Thread.cpp ThreadPool.h ThreadPool.cpp\
		     Singleton.h HttpClient.h HttpClient.cpp\
		     qq_debug.h qq_debug.cpp\
		     md5.h md5.c \
		     Action.h Action.cpp

AM_CFLAGS = -g  -Wall -Werror -D_REENTRANT  -Wall -Wno-long-long -Wshadow -Wpacked -Wundef -Wchar-subscripts  -pedantic -ansi


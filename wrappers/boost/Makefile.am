pyexec_LTLIBRARIES= libwebqqboost.la
libwebqqboost_la_SOURCES= libwebqqboost.cpp
libwebqqboost_la_LDFAGS= 
libwebqqboost_la_CPPFLAGS=-I../../jsoncpp/include -I/usr/include/python2.7/ -I../../src  $(CURL_CFLAGS) $(CXXFLAGS)

AM_CPPFLAGS= 
AM_CXXFLAGS = -fPIC -ggdb -O0  -Wall
AM_LDFLAGS =  $(CURL_LIBS) -lpython2.7 -lboost_python  -L../../src/.libs/ -lwebqq

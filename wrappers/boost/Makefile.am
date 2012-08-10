lib_LTLIBRARIES=libwebqqboost.la
libwebqqboost_la_CPPFLAGS=-I../../jsoncpp/include -I/usr/include/python2.7/ -I../../src  $(CURL_CFLAGS) $(CFLAGS)
libwebqqboost_la_LIBADD=$(CURL_LIBS) -lpython2.7 -lboost_python  -L../../src/.libs/ -lwebqq
libwebqqboost_la_SOURCES= libwebqqboost.cpp

AM_CXXFLAGS = -fPIC -ggdb -O0  -Wall
AM_LDFLAGS = 

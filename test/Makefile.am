noinst_PROGRAMS=testhttpclient testmd5 testjsoncpp

testhttpclient_SOURCES=testhttpclient.cpp

testmd5_SOURCES=testmd5.c

testjsoncpp_SOURCES=jsontest.h jsontest.cpp testjsoncpp.cpp

AM_LDFLAGS = -L${srcdir}/../src -lwebqq
AM_CPPFLAGS = -I${srcdir}/../src -I${srcdir}/../jsoncpp/include

noinst_PROGRAMS=testhttpclient testmd5 testjsoncpp testthreadpool testutil testplugin

testhttpclient_SOURCES=testhttpclient.cpp

testutil_SOURCES=testutil.cpp

testmd5_SOURCES=testmd5.cpp

testthreadpool_SOURCES=testthreadpool.cpp

testjsoncpp_SOURCES=jsontest.h jsontest.cpp testjsoncpp.cpp

testplugin_SOURCES=testplugin.cpp


AM_LDFLAGS = -L${srcdir}/../src -lwebqq
AM_CPPFLAGS = -I${srcdir}/../src -I${srcdir}/../jsoncpp/include

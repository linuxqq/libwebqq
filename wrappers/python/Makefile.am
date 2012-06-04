AM_CPPFLAGS= ${PYTHON_CFLAGS} -I../../src
EXTRA_DIST = libwebqq.i
python_PYTHON =  libwebqqpython.py

pyexec_LTLIBRARIES= libwebqqpython.la

libwebqqpython_la_SOURCES= libwebqqpython.cpp
libwebqqpython_la_LDFAGS=  -module
libwebqqpython_la_LIBADD=../../src/libwebqq.la -L../../jsoncpp/src/ -ljsoncpp


libwebqqpython.cpp: libwebqq.i
	swig -c++ -python -I../../src -o $@ $<

clean-local:
	rm -f libwebqq.py  libwebqqpython.h libwebqqpython.cpp *.pyc

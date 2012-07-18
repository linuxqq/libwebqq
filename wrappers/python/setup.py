#!/usr/bin/env python
from distutils.core  import setup, Extension

libwebqqpython = Extension(
	'_libwebqqpython',
	sources = ['libwebqq.i',],
	include_dirs = ['../../src','../../jsoncpp/include'],
	libraries =['curl'],
	define_macros=[('USE_EVENT_QUEUE', None),],
	extra_link_args= ['../../src/.libs/libwebqq.a',
	   		  '../../jsoncpp/src/libjsoncpp.a',],
	swig_opts = ['-modern' ,'-c++','-I../../src','-I../../jsoncpp/include',],
	)
setup ( name = 'libwebqqpython',
	ext_modules = [libwebqqpython,],
	py_modules=['libwebqqpython'],
	)

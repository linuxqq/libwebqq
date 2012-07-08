#!/usr/bin/env python
from distutils.core  import setup, Extension

libwebqqpython = Extension(
	'_libwebqqpython',
	sources = ['libwebqq.i',],
	include_dirs = ['../../src','../../jsoncpp/include'],
	library_dirs= ['../../src/.libs',],
	libraries = ['webqq'],
	runtime_library_dirs=['../../src/.libs',],
	swig_opts = ['-modern' ,'-c++', '-I../../src','-I../../jsoncpp/include',],
	)
setup ( name = 'libwebqqpython',
	ext_modules = [libwebqqpython,],
	py_modules=['libwebqqpython'],
	)

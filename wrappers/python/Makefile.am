EXTRA_DIST = libwebqq.i

all: clean-local libwebqq.i 
	python setup.py build_ext --inplace 
clean-local:
	rm -rf  *.pyc build/ *.pyc *_wrap.* *.so libwebqqpython.py

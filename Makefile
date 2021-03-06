PYTHON?=python
PYTHON_INCLUDE=-I$(shell $(PYTHON) -c "from distutils import sysconfig; print sysconfig.get_python_inc()")
CCACHE?=
CC  = $(CCACHE) gcc -Wno-deprecated -O2 -I. $(PYTHON_INCLUDE) -fPIC
CXX = $(CCACHE) g++ -Wno-deprecated -O2 -I. $(PYTHON_INCLUDE) -fPIC -std=gnu++0x

all:: t1 gg.so

clean::
	rm -f t1 *.o *.so *.pyc

t1: t1.o gglib.o
	$(CXX) -o t1 t1.o gglib.o

t1.o: t1.cc gglib.h
	$(CXX) -Wall -o t1.o -c t1.cc

gglib.o: gglib.cc gglib.h
	$(CXX) -Wall -o gglib.o -c gglib.cc

gg.o: gg.cc gglib.h
	$(CXX) -o gg.o -c gg.cc

gg.cc: gg.pyx
	cython --cplus gg.pyx -o gg.cc

gg.so: gg.o gglib.o
	$(CXX) -o gg.so -shared gg.o gglib.o

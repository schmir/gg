PYTHON?=python
PYTHON_INCLUDE=-I$(shell $(PYTHON) -c "from distutils import sysconfig; print sysconfig.get_python_inc()")

CC  = gcc -Wall -Wno-deprecated -O2 -I. $(PYTHON_INCLUDE)
CXX = g++ -Wall -Wno-deprecated -O2 -I. $(PYTHON_INCLUDE)

all:: t1

clean::
	rm -f t1 *.o

t1: t1.o gglib.o
	$(CXX) -o t1 t1.o gglib.o

t1.o: t1.cc gglib.h
	$(CXX) -o t1.o -c t1.cc

gglib.o: gglib.cc gglib.h
	$(CXX) -o gglib.o -c gglib.cc

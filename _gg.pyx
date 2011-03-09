from libcpp.vector cimport vector
from cython.operator import dereference

version = "0.1.0"
version_info = (0, 1, 0)

cdef extern from "gglib.h" namespace "gg":
    cdef cppclass dgraph:
        dgraph()
        void add_link(int s, int e)
        void remove_link(int s, int e)
        void dump()
        void get_links_to(int s, vector[int] result)
        void get_links_from(int s, vector[int] result)

        void remove_links_to(int s)
        void remove_links_from(int s)

        void get_reachable_from(vector[int] &nodes)

cdef class intvector(object):
    cdef vector[int] * _ptr

    def __cinit__(self, lst=None):
        self._ptr = new vector[int]()
        if lst is not None:
            for x in lst:
                if not isinstance(x, (int, long)):
                    raise TypeError("a list of integers is required")
                self.append(x)

    def __dealloc__(self):
        del self._ptr

    def __len__(self):
        return self._ptr.size()

    def __getitem__(self, int idx):
        if idx<0 or idx>=self._ptr.size():
            raise IndexError("list index out of range")

        return dereference(self._ptr)[idx]

    def append(self, int item):
        self._ptr.push_back(item)


cdef class graph(object):
    cdef dgraph * _ptr

    def __cinit__(self):
        self._ptr = new dgraph()

    def __dealloc__(self):
        del self._ptr

    def add_link(self, s, e):
        self._ptr.add_link(s, e)

    def remove_link(self, s, e):
        self._ptr.remove_link(s, e)

    def dump(self):
        self._ptr.dump()

    def get_links_to(self, s):
        cdef intvector result = intvector()
        self._ptr.get_links_to(s, dereference(result._ptr))
        return result

    def get_links_from(self, s):
        cdef intvector result = intvector()
        self._ptr.get_links_from(s, dereference(result._ptr))
        return result

    def get_reachable_from(self, intvector nodes):
        self._ptr.get_reachable_from(dereference(nodes._ptr))
        return nodes

    def remove_links_to(self, s):
        self._ptr.remove_links_to(s)

    def remove_links_from(self, s):
        self._ptr.remove_links_from(s)

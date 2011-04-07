
# cython: embedsignature=True

from libcpp.vector cimport vector
from cython.operator import dereference, preincrement
from libcpp cimport bool
from cpython cimport bool as pybool, int as pyint
from cpython cimport cobject
from libcpp.pair cimport pair

version = "0.1.1"
version_info = (0, 1, 1)


cdef extern from "gglib.h" namespace "gg":
    cdef cppclass dgraph_iterator:
        dgraph_iterator(dgraph_iterator &)
        pair[int, int] operator*()
        dgraph_iterator &operator++()
        bool operator==(dgraph_iterator &)

    cdef cppclass dgraph:
        dgraph_iterator begin()
        dgraph_iterator end()

        dgraph()
        void add_link(int s, int e)
        void remove_link(int s, int e)
        void dump()
        void get_links_to(int s, vector[int] result)
        void get_links_from(vector[int] nodes, vector[bool] result)
        void get_links_from(vector[int] nodes, vector[bool] result, vector[bool] allowed)
        void get_links_from(int s, vector[int] result)

        void remove_links_to(int s)
        void remove_links_from(int s)

        void get_reachable_from(vector[int] &nodes)

        int size()
        int maxstartnode()
        int maxendnode()


cdef class boolvector(object):
    cdef vector[bool] * _ptr

    def __cinit__(self, lst=None):
        self._ptr = new vector[bool]()

        if lst is not None:
            for x in lst:
                if not isinstance(x, pybool):
                    raise TypeError("a list of bools is required")
                self.append(x)

    def __dealloc__(self):
        del self._ptr

    def __len__(self):
        return self._ptr.size()

    def __getitem__(self, pyint idx):
        if idx < 0 or idx >= self._ptr.size():
            raise IndexError("list index out of range")

        return dereference(self._ptr)[idx]

    def __setitem__(self, pyint idx, pybool val):
        if idx < 0 or idx >= self._ptr.size():
            raise IndexError("list index out of range")

        dereference(self._ptr)[idx] = val

    def resize(self, int size):
        self._ptr.resize(size)

    def append(self, pybool item):
        self._ptr.push_back(item)

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

    def __getitem__(self, pyint idx):
        if idx < 0 or idx >= self._ptr.size():
            raise IndexError("list index out of range")

        return dereference(self._ptr)[idx]

    def __setitem__(self, pyint idx, pyint val):
        if idx < 0 or idx >= self._ptr.size():
            raise IndexError("list index out of range")

        dereference(self._ptr)[idx] = val

    def resize(self, int size):
        self._ptr.resize(size)

    def append(self, pyint item):
        self._ptr.push_back(item)


cdef class graph(object):
    cdef dgraph * _ptr
    cdef dgraph_iterator *_it
    cdef dgraph_iterator *_it_end

    def __cinit__(self):
        self._ptr = new dgraph()
        self._it = NULL
        self._it_end = NULL

    def __iter__(self):
        del self._it
        del self._it_end
        self._it = NULL
        self._it_end = NULL
        return self

    def __next__(self):
        if self._it == NULL:
            self._it = new dgraph_iterator(self._ptr.begin())
            self._it_end = new dgraph_iterator(self._ptr.end())

        if dereference(self._it) == dereference(self._it_end):
            self.__iter__()
            raise StopIteration()

        cdef pair[int, int] p = dereference(dereference(self._it))
        preincrement(dereference(self._it))
        return (p.first, p.second)

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

    def get_links_from(self, s, result=None, allowed=None):
        cdef intvector _result

        if result is None:
            assert allowed is None
            _result = intvector()
            self._ptr.get_links_from(<int>s, dereference(_result._ptr))
            return _result

        cdef boolvector boolv = result
        cdef boolvector _allowed = allowed

        cdef vector[int] v
        if isinstance(s, (int, long)):
            assert allowed is None
            v.push_back(s)
            self._ptr.get_links_from(v, dereference(boolv._ptr))
        else:
            if _allowed is not None:
                self._ptr.get_links_from(dereference((<intvector>s)._ptr),
                                         dereference(boolv._ptr),
                                         dereference(_allowed._ptr))
            else:
                self._ptr.get_links_from(dereference((<intvector>s)._ptr), dereference(boolv._ptr))

        return boolv

    def get_reachable_from(self, intvector nodes):
        self._ptr.get_reachable_from(dereference(nodes._ptr))
        return nodes

    def remove_links_to(self, s):
        self._ptr.remove_links_to(s)

    def remove_links_from(self, s):
        self._ptr.remove_links_from(s)

    def __len__(self):
        return self._ptr.size()

    def maxstartnode(self):
        return self._ptr.maxstartnode()

    def maxendnode(self):
        return self._ptr.maxendnode()

    def compute_paths(self, node, reverse=False, allow_cycles=False):
        if reverse:
            get = self.get_links_to
        else:
            get = self.get_links_from

        def _computePaths(node, path):
            nodes = list(get(node))
            if not nodes:
                return [(False, path)]

            retval = []
            for n in list(get(node)):
                if n in path:
                    if allow_cycles:
                        retval.append((True, path + [n]))
                    continue
                retval.extend(_computePaths(n, path + [n]))
            return retval

        return _computePaths(node, [node])

#! /usr/bin/env python

import time
import _gg


class build_graph(object):
    def __init__(self):

        self.g = _gg.graph()
        self.cid = 0
        self.make_container(self.getcid(), 6)

    def getcid(self):
        self.cid += 1
        return self.cid

    def make_container(self, parent, depth):
        if depth <= 0:
            return
        for i in range(10):
            c = self.getcid()
            self.g.add_link(parent, c)
            self.make_container(c, depth - 1)


def perf_reachable():
    g = build_graph().g
    print len(g)
    stime = time.time()
    res = g.get_reachable_from(_gg.intvector([1]))

    print len(res), time.time() - stime

perf_reachable()

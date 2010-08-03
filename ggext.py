#! /usr/bin/env python

import time
from gg import graph

def readcsv(fn, g=None):
    stime=time.time()
    if g is None:
        g = graph()

    count=0
    add_link = g.add_link
    for x in open(fn, "rb"):
        try:
            s, e = map(int, x.split())
        except ValueError:
            pass
        else:
            add_link(s,e)
            count += 1

    print "read %s links in %ss" % (count,time.time()-stime)
    return g

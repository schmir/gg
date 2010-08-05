#! /usr/bin/env py.test

import gg

def pytest_funcarg__gr(request):
    gr = gg.graph()
    for i in range(100):
        for j in range(i):
            gr.add_link(i, j)
    return gr

def test_get_links_from(gr):
    for i in range(100):
        assert set(gr.get_links_from(i))==set(range(i))

def test_get_links_to(gr):
    for i in range(100):
        set(gr.get_links_to(i))==range(1, 100-i)

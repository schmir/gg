#! /usr/bin/env py.test
import py

import _gg


def pytest_funcarg__gr(request):
    gr = _gg.graph()
    for i in range(100):
        for j in range(i):
            gr.add_link(i, j)
    return gr


def test_get_links_from(gr):
    for i in range(100):
        assert set(gr.get_links_from(i)) == set(range(i))


def test_get_links_to(gr):
    for i in range(100):
        set(gr.get_links_to(i)) == range(1, 100 - i)


def test_intvector_append():
    g = _gg.intvector()
    for i in range(1000):
        g.append(i)

    assert list(g) == range(1000)


def test_intvector_empty_init():
    g = _gg.intvector()
    assert list(g) == []


def test_intvector_init_from_list():
    g = _gg.intvector(range(100))
    assert list(g) == range(100)


def test_intvector_init_from_list_with_float():
    py.test.raises(TypeError, "_gg.intvector([0, 1.5])")


def test_get_reachable_from_empty_graph():
    g = _gg.graph()
    res = g.get_reachable_from(_gg.intvector(range(10)))
    assert list(res) == range(10)


def test_get_reachable_from():
    g = _gg.graph()
    g.add_link(1, 2)
    g.add_link(1, 3)
    g.add_link(2, 4)
    res = g.get_reachable_from(_gg.intvector([1]))
    assert set(res) == set([1, 2, 3, 4])

    res = g.get_reachable_from(_gg.intvector([4]))
    assert set(res) == set([4])


def test_get_reachable_from_cyclic():
    g = _gg.graph()
    g.add_link(1, 2)
    g.add_link(2, 3)
    g.add_link(3, 1)
    res = g.get_reachable_from(_gg.intvector([1]))
    assert set(res) == set([1, 2, 3])


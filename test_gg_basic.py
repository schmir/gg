#! /usr/bin/env py.test

import py
import gg


def pytest_funcarg__gr(request):
    gr = gg.graph()
    for i in range(100):
        for j in range(i):
            gr.add_link(i, j)
    return gr


small_links = set([(1, 2), (3, 4), (5, 1), (5, 4)])


def pytest_funcarg__small(request):
    small = gg.graph()
    for x in small_links:
        small.add_link(*x)
    return small

# -- tests


def test_iter(small):
    assert set(small) == small_links


def test_iter_gr(gr):
    expected = set()
    for i in range(100):
        for j in range(i):
            expected.add((i, j))

    assert set(gr) == expected


def test_iter_empty():
    assert list(gg.graph()) == []


def test_iter_restart(small):
    try:
        for i, x in enumerate(small):
            print "iterate, link:", x
            if i == 1:
                raise RuntimeError()
    except RuntimeError:
        pass

    links = set(small)
    assert links == small_links


def test_get_links_from(gr):
    for i in range(100):
        assert set(gr.get_links_from(i)) == set(range(i))


def test_get_links_to(gr):
    for i in range(100):
        set(gr.get_links_to(i)) == range(1, 100 - i)


def test_graph_len():
    g = gg.graph()
    assert len(g) == 0

    g.add_link(1, 2)
    assert len(g) == 1

    g.add_link(1, 2)
    assert len(g) == 1

    g.add_link(2, 1)
    assert len(g) == 2


def test_intvector_append():
    g = gg.intvector()
    for i in range(1000):
        g.append(i)

    assert list(g) == range(1000)


def test_intvector_empty_init():
    g = gg.intvector()
    assert list(g) == []


def test_intvector_init_from_list():
    g = gg.intvector(range(100))
    assert list(g) == range(100)


def test_intvector_init_from_list_with_float():
    py.test.raises(TypeError, "gg.intvector([0, 1.5])")


def test_get_reachable_from_empty_graph():
    g = gg.graph()
    res = g.get_reachable_from(gg.intvector(range(10)))
    assert list(res) == range(10)


def test_get_reachable_from():
    g = gg.graph()
    g.add_link(1, 2)
    g.add_link(1, 3)
    g.add_link(2, 4)
    res = g.get_reachable_from(gg.intvector([1]))
    assert set(res) == set([1, 2, 3, 4])

    res = g.get_reachable_from(gg.intvector([4]))
    assert set(res) == set([4])


def test_get_reachable_from_cyclic():
    g = gg.graph()
    g.add_link(1, 2)
    g.add_link(2, 3)
    g.add_link(3, 1)
    res = g.get_reachable_from(gg.intvector([1]))
    assert set(res) == set([1, 2, 3])


def test_get_reachable_from_multiple_parents():
    g = gg.graph()
    g.add_link(1, 3)
    g.add_link(2, 3)
    res = g.get_reachable_from(gg.intvector([1, 2]))
    assert list(res) == [1, 2, 3]


def test_maxstartnode():
    g = gg.graph()
    g.add_link(1, 3)

    assert g.maxstartnode() == 1
    assert g.maxendnode() == 3


def test_get_links_from_boolvector_return(gr):
    allowed = gg.boolvector()
    cids = gg.intvector()
    cids.append(1)
    res = gr.get_links_from(cids, allowed)
    assert res is allowed


def test_get_links_from_boolvector_empty(small):
    cids = gg.intvector()
    allowed = gg.boolvector()
    small.get_links_from(cids, allowed)
    assert list(allowed) == [False] * 5

    allowed[1] = True
    small.get_links_from(cids, allowed)
    assert list(allowed) == [False, True, False, False, False]


def test_get_links_from_boolvector(small):
    cids = gg.intvector([3, 5])
    allowed = gg.boolvector()
    small.get_links_from(cids, allowed)
    assert list(allowed) == [False, True, False, False, True]
    assert list(cids) == [3, 5]

    allowed = gg.boolvector()
    cids.append(1)
    small.get_links_from(cids, allowed)

    assert list(allowed) == [False, True, True, False, True]


def test_get_links_from_allowed(small):
    cids = gg.intvector([3, 5])
    result = gg.boolvector()
    allowed = gg.boolvector()
    small.get_links_from(cids, result, allowed)
    assert list(result) == [False, False, False, False, False]

    allowed.resize(10)
    small.get_links_from(cids, result, allowed)
    assert list(result) == [False, False, False, False, False]

    allowed[1] = True
    small.get_links_from(cids, result, allowed)
    assert list(result) == [False, True, False, False, False]

    for i in range(10):
        allowed[i] = True
    small.get_links_from(cids, result, allowed)
    assert list(result) == [False, True, False, False, True]

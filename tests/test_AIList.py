import numpy as np
import pickle


test_first_wps = np.array([-2., -2., -2., -2., -2., -1., -1., -2., -3., -3.])
test_last_wps = np.array([ 2.,  2.,  2.,  0.,  0.,  0.,  0.,  1., -1., -1.])

test_ld = np.array([1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                    0, 0, 0, 1])

test_nhits = np.array([1, 4, 4, 2, 2, 2, 2, 2, 2, 2])

def test_AIList():
    from AIList import AIList, Interval

    # Test AIList construction
    i = AIList()
    i.add(10, 11)
    i.add(15, 19)
    i.add(11, 13)
    i.add(19, 22)
    i.add(20, 25)
    i.add(30, 100)
    i.add(30, 95)
    i.construct()
    assert len(i) == 7

    # Test iteration
    is_Interval = 0
    for x in i:
        is_Interval += isinstance(x, Interval)
    assert is_Interval == 7

    # Test intersection
    o = i.intersect(3,15)
    assert o.size == 2 and o.first == 10 and o.last == 13

    # Test WPS calculation
    w = i.wps(4)
    assert (w.values[:10] == test_first_wps).all() and (w.values[-10:] == test_last_wps).all()

    # Test merging
    m = i.merge()
    assert m.size == 5 and m.first == 10 and m.last == 100

    # Test filtering
    f = i.filter(3, 10)
    assert f.size == 2 and f.first == 15 and f.last == 25

    # Test length distribution
    ld = i.length_dist()
    assert (ld == test_ld).all()

    # Test nhits from arrray
    starts = np.arange(1,100,10)
    ends = starts + 10
    nhits = i.nhits_from_array(starts, ends)
    assert (nhits == test_nhits).all()

    # Test pickling
    d = pickle.dumps(i)
    i2 = pickle.loads(d)
    assert i2.size == 7 and i2.first == 10 and i2.last == 100

    # Test pickle adding
    i2.add(40, 60)
    assert len(i2) == 8

    # Test pickle iteration
    is_Interval = 0
    for x in i2:
        is_Interval += isinstance(x, Interval)
    assert is_Interval == 7
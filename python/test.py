import inspect
import struct
from unittest import TestCase

import matplotlib.pyplot as plt
import numpy as np

import libspot
from libspot import Spot  # pylint: disable=E0611


class Test(TestCase):
    def test_version(self):
        print(libspot.__version__)

    def test_init(self):
        Spot(1e-6)

    def test_globals(self):
        assert libspot.NORMAL == 0
        assert libspot.EXCESS == 1
        assert libspot.ANOMALY == 2

    def test_unpack(self):
        # structures packing format
        # see https://docs.python.org/3/library/struct.html
        UBEND_FLAT_PACKING_FORMAT = "LLdiP"
        PEAKS_FLAT_PACKING_FORMAT = "dddd" + UBEND_FLAT_PACKING_FORMAT
        TAIL_FLAT_PACKING_FORMAT = "dd" + PEAKS_FLAT_PACKING_FORMAT
        SPOT_FLAT_PACKING_FORMAT = "ddiidddLL" + TAIL_FLAT_PACKING_FORMAT
        s = Spot(1e-6)
        X = np.random.standard_normal(10_000)
        s.fit(X)
        out = struct.unpack(SPOT_FLAT_PACKING_FORMAT, s.raw())
        print(out)

    def test_members(self):
        s = Spot(1e-6, max_excess=1_000)
        X = np.random.standard_normal(100_000)
        s.fit(X)
        s.step(0.0)

        print("level:", s.level)
        print("t:", s.excess_threshold)
        print("z:", s.anomaly_threshold)
        print("Nt:", s.Nt)
        print("n:", s.n)
        print("gamma:", s.gamma)
        print("sigma:", s.sigma)

    # def test_dump(self):
    #     s = Spot(1e-6, max_excess=1_000)
    #     X = np.random.standard_normal(100_000)
    #     s.fit(X)
    # print(s.dump())

    def test_excesses(self):
        max_excess = 800
        s = Spot(1e-6, level=0.98, max_excess=max_excess)
        X = np.random.standard_normal(100_000)
        s.fit(X)
        assert len(s.excesses()) == max_excess

    def test_doc(self):
        # print(Spot.fit.__text_signature__)
        print(inspect.signature(Spot))
        print(inspect.signature(Spot.fit))

    def test_as_dict(self):
        s = Spot(1e-6)
        X = np.random.standard_normal(30_000)
        s.fit(X)
        s.as_dict()

    def test_stream(self):
        s = Spot(1e-5, level=0.99, max_excess=20_000)
        s.fit(np.random.standard_normal(size=20_000))

        X = np.random.standard_normal(size=100_000)
        T = np.zeros(X.size)
        Ax = []
        Ay = []
        for i, x in enumerate(X):
            # x += 0.0001 * i
            r = s.step(x)
            if r == libspot.ANOMALY:
                Ax.append(i)
                Ay.append(x)
            # elif r == libspot.EXCESS:
            #     tail = s.as_dict()["tail"]
            #     peaks = tail["peaks"]
            #     ubend = peaks["container"]
            # print(
            #     tail["gamma"],
            #     tail["sigma"],
            #     peaks["e"],
            #     peaks["e2"],
            #     ubend["last_erased_data"],
            # )
            T[i] = s.anomaly_threshold

        print(np.min(T), np.max(T))
        plt.plot(X)
        plt.plot(T, ls="--", lw=2)
        plt.scatter(Ax, Ay, color="red")
        plt.savefig("/tmp/stream.png")

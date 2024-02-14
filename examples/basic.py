import matplotlib.pyplot as plt
import numpy as np

from libspot import ANOMALY, Spot

# Fancyness ---------------------------------------------------------------- #
colors = {
    "bg": "#242933",
    "stream": "#88c0d0",
    "threshold": "#ebcb8b",
    "anomaly": "#bf616a",
    "axes": "#eceff4",
}
style = {
    "figure.facecolor": colors["bg"],
    "axes.facecolor": colors["bg"],
    "axes.edgecolor": colors["axes"],
    "xtick.color": colors["axes"],
    "ytick.color": colors["axes"],
    "font.family": "monospace",
    "font.monospace": "IBM Plex Mono",
    "font.size": 20,
    "svg.fonttype": "none",
    "lines.markersize": 10.0,
}
# -------------------------------------------------------------------------- #


TRAIN = np.random.standard_normal(size=10_000)
STREAM = np.random.standard_normal(size=100_000)
THRESHOLD = np.zeros(STREAM.size)

spot = Spot(q=5e-6, max_excess=2000, level=0.99)
spot.fit(TRAIN)

Ax = []
Ay = []
for i, x in enumerate(STREAM):
    r = spot.step(x)
    if r == ANOMALY:
        Ax.append(i)
        Ay.append(x)
    THRESHOLD[i] = spot.anomaly_threshold


with plt.rc_context(style):
    fig, ax = plt.subplots(figsize=(14, 6))
    ax.plot(STREAM, color=colors["stream"])
    ax.plot(THRESHOLD, ls="--", lw=2, color=colors["threshold"])
    ax.scatter(Ax, Ay, color=colors["anomaly"])
    fig.tight_layout()
    fig.savefig("../docs/img/basic.svg")

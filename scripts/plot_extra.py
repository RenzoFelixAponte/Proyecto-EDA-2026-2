"""Graficos de E1 (correctitud) y E4 (Skip List con p = 1/4).

Uso: python scripts/plot_extra.py
Lee results/e1_tests.txt, results/summary.csv y results/p025/summary.csv.
"""
import re
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

OUT = Path("figures")
COLORS = {"SkipList p=1/2": "#1f77b4", "SkipList p=1/4": "#7fb3e0", "AVL": "#d62728"}


def plot_e1():
    text = Path("results/e1_tests.txt").read_text(encoding="utf8")
    blocks = re.findall(r"== (.+?)\n.*?comprobaciones: (\d+)\s+fallos: (\d+)", text, re.S)
    names = [b[0] for b in blocks]
    checks = [int(b[1]) for b in blocks]
    fails = [int(b[2]) for b in blocks]

    fig, ax = plt.subplots(figsize=(6.5, 2.8))
    bars = ax.bar(names, checks, color=[COLORS.get(n, "gray") for n in names])
    for bar, f in zip(bars, fails):
        ax.annotate(f"{bar.get_height():,} comprobaciones\n{f} fallos",
                    (bar.get_x() + bar.get_width() / 2, bar.get_height()),
                    ha="center", va="bottom", fontsize=8)
    ax.set_ylabel("comprobaciones")
    ax.set_ylim(0, max(checks) * 1.3)
    ax.set_title("E1: comprobaciones contra std::set e invariantes")
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    fig.savefig(OUT / "e1_correctitud.pdf")
    plt.close(fig)


def plot_e4():
    main = pd.read_csv("results/summary.csv")
    p025 = pd.read_csv("results/p025/summary.csv")
    sel = lambda df, s: df[(df.structure == s) & (df.n == 1000000) & (df.order == "aleatorio")]
    series = {
        "SkipList p=1/2": sel(main, "SkipList"),
        "SkipList p=1/4": sel(p025, "SkipList(p=0.25)"),
        "AVL": sel(main, "AVL"),
    }
    ops = [("search", "Búsqueda"), ("insert", "Inserción"), ("remove", "Eliminación")]
    width = 0.27
    fig, ax = plt.subplots(figsize=(6.5, 3))
    for i, (label, df) in enumerate(series.items()):
        df = df.set_index("operation")
        xs = [j + (i - 1) * width for j in range(len(ops))]
        ax.bar(xs, [df.loc[o, "mean_ns"] for o, _ in ops], width,
               yerr=[df.loc[o, "std_ns"] for o, _ in ops], capsize=3,
               label=label, color=COLORS[label])
    ax.set_xticks(range(len(ops)), [name for _, name in ops])
    ax.set_ylabel("ns por operación")
    ax.set_title("E4: efecto de p (n = 1,000,000, orden aleatorio)")
    ax.set_ylim(0, ax.get_ylim()[1] * 1.2)
    ax.legend(fontsize=8, ncol=3, loc="upper center")
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    fig.savefig(OUT / "e4_p025.pdf")
    plt.close(fig)


if __name__ == "__main__":
    OUT.mkdir(exist_ok=True)
    plot_e1()
    plot_e4()
    print("Graficos E1 y E4 guardados en figures/")

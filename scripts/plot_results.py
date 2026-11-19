"""Genera los graficos G1-G4 a partir de results/summary.csv.

Uso: python scripts/plot_results.py [results/summary.csv] [figures/]
"""
import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

ORDERS = ["aleatorio", "ordenado", "inverso"]
OPS = {"search": "Búsqueda", "insert": "Inserción", "remove": "Eliminación"}
STYLE = {"SkipList": dict(color="#1f77b4", marker="o"), "AVL": dict(color="#d62728", marker="s")}


def plot_vs_n(df, op, title, out):
    fig, axes = plt.subplots(1, 3, figsize=(11, 3.2), sharey=True)
    for ax, order in zip(axes, ORDERS):
        sub = df[(df.operation == op) & (df.order == order)]
        for structure, g in sub.groupby("structure"):
            g = g.sort_values("n")
            ax.errorbar(g.n, g.mean_ns, yerr=g.std_ns, label=structure, capsize=3,
                        **STYLE.get(structure, {}))
        ax.set_xscale("log")
        ax.set_title(f"orden {order}")
        ax.set_xlabel("n")
        ax.grid(alpha=0.3)
    axes[0].set_ylabel("ns por operación")
    axes[0].legend()
    fig.suptitle(title)
    fig.tight_layout()
    fig.savefig(out)
    plt.close(fig)


def plot_order_effect(df, out):
    n = df.n.max()
    sub = df[df.n == n]
    fig, axes = plt.subplots(1, 3, figsize=(11, 3.2))
    for ax, (op, label) in zip(axes, OPS.items()):
        g = sub[sub.operation == op].pivot(index="order", columns="structure", values="mean_ns")
        e = sub[sub.operation == op].pivot(index="order", columns="structure", values="std_ns")
        g.loc[ORDERS].plot.bar(ax=ax, yerr=e.loc[ORDERS], capsize=3, rot=0,
                               color=[STYLE[c]["color"] for c in g.columns])
        ax.set_title(label)
        ax.set_xlabel("")
        ax.grid(axis="y", alpha=0.3)
    axes[0].set_ylabel("ns por operación")
    fig.suptitle(f"G4: efecto del orden de los datos (n = {n:,})")
    fig.tight_layout()
    fig.savefig(out)
    plt.close(fig)


def main():
    csv = Path(sys.argv[1] if len(sys.argv) > 1 else "results/summary.csv")
    out = Path(sys.argv[2] if len(sys.argv) > 2 else "figures")
    out.mkdir(parents=True, exist_ok=True)
    df = pd.read_csv(csv)
    for i, (op, label) in enumerate(OPS.items(), start=1):
        plot_vs_n(df, op, f"G{i}: {label}", out / f"g{i}_{op}.pdf")
    plot_order_effect(df, out / "g4_orden.pdf")
    print(f"Gráficos guardados en {out}/")


if __name__ == "__main__":
    main()

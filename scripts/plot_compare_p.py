"""Genera figuras comparando AVL vs SkipList(p=0.25) vs SkipList(p=0.80).

Ejecuta el benchmark dos veces, combina los resultados y guarda 4 figuras
en figures/ con el mismo estilo que G1-G4.

Uso: python scripts/plot_compare_p.py [--exe ruta/benchmark] [--sizes 1000,10000,100000]
"""
import sys
sys.stdout.reconfigure(encoding="utf-8")
sys.stderr.reconfigure(encoding="utf-8")

import subprocess
import argparse
import shutil
import tempfile
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

# ── Estilos por estructura ────────────────────────────────────────────────────
STYLE = {
    "AVL":        dict(color="#d62728", marker="s", linestyle="-"),
    "SL p=0.25":  dict(color="#1f77b4", marker="o", linestyle="-"),
    "SL p=0.80":  dict(color="#2ca02c", marker="^", linestyle="--"),
}
BAR_COLORS = {
    "AVL":       "#d62728",
    "SL p=0.25": "#1f77b4",
    "SL p=0.80": "#2ca02c",
}
ORDERS = ["aleatorio", "ordenado", "inverso"]
OPS    = {"search": "Busqueda", "insert": "Insercion", "remove": "Eliminacion"}


# ── Helpers de compilacion/benchmark ─────────────────────────────────────────
def find_exe(hint):
    for c in [hint, "build/benchmark", "build/benchmark.exe",
              "build/Release/benchmark.exe"]:
        if c and shutil.which(c):
            return Path(c)
        if c and Path(c).is_file():
            return Path(c)
    sys.exit("No se encontro el binario 'benchmark'. Compilalo primero.")


def run_benchmark(exe, p, sizes, out_dir):
    out_dir.mkdir(parents=True, exist_ok=True)
    cmd = [str(exe), "--p", str(p), "--sizes", sizes,
           "--reps", "7", "--warmup", "1",
           "--orders", "aleatorio,ordenado,inverso",
           "--out", str(out_dir)]
    print(f"  benchmark p={p} -> {out_dir.name}/summary.csv")
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(f"Error al correr benchmark:\n{r.stderr}")
    return out_dir / "summary.csv"


def build_combined(csv25, csv80):
    d25 = pd.read_csv(csv25)
    d80 = pd.read_csv(csv80)

    avl  = d25[d25["structure"] == "AVL"].copy(); avl["label"]  = "AVL"
    sl25 = d25[d25["structure"].str.startswith("SkipList")].copy(); sl25["label"] = "SL p=0.25"
    sl80 = d80[d80["structure"].str.startswith("SkipList")].copy(); sl80["label"] = "SL p=0.80"

    return pd.concat([avl, sl25, sl80], ignore_index=True)


# ── Figuras ───────────────────────────────────────────────────────────────────
def plot_vs_n(df, op, title, out):
    """Lineas ns/op vs n, un subplot por orden — igual que G1-G3."""
    fig, axes = plt.subplots(1, 3, figsize=(13, 3.6), sharey=False)
    for ax, order in zip(axes, ORDERS):
        sub = df[(df.operation == op) & (df.order == order)]
        for lbl, g in sub.groupby("label"):
            g = g.sort_values("n")
            ax.errorbar(g.n, g.mean_ns, yerr=g.std_ns,
                        label=lbl, capsize=3, **STYLE[lbl])
        ax.set_xscale("log")
        ax.set_title(f"orden {order}", fontsize=10)
        ax.set_xlabel("n")
        ax.grid(alpha=0.3)
        ax.tick_params(labelsize=8)
    axes[0].set_ylabel("ns por operacion")
    axes[0].legend(fontsize=8, framealpha=0.7)
    fig.suptitle(title, fontsize=12, fontweight="bold")
    fig.tight_layout()
    fig.savefig(out, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  Guardado: {out.name}")


def plot_order_bars(df, out):
    """Barras agrupadas por orden para n_max — estilo G4 con 3 estructuras."""
    n_max = df.n.max()
    sub = df[df.n == n_max]
    labels = ["AVL", "SL p=0.25", "SL p=0.80"]

    fig, axes = plt.subplots(1, 3, figsize=(14, 4))
    for ax, (op, op_label) in zip(axes, OPS.items()):
        rows = sub[sub.operation == op]
        pivot_m = rows.pivot(index="order", columns="label", values="mean_ns")
        pivot_e = rows.pivot(index="order", columns="label", values="std_ns")

        # Reordenar columnas y filas
        pivot_m = pivot_m.reindex(columns=labels, index=ORDERS)
        pivot_e = pivot_e.reindex(columns=labels, index=ORDERS)

        pivot_m.plot.bar(
            ax=ax, yerr=pivot_e, capsize=4, rot=0,
            color=[BAR_COLORS[c] for c in pivot_m.columns],
            edgecolor="white", linewidth=0.5,
        )
        ax.set_title(op_label, fontsize=11, fontweight="bold")
        ax.set_xlabel("")
        ax.set_ylabel("ns por operacion" if ax == axes[0] else "")
        ax.grid(axis="y", alpha=0.35)
        ax.tick_params(axis="x", labelsize=9)
        ax.legend(fontsize=8, framealpha=0.7)

    fig.suptitle(
        f"Efecto del orden de los datos  (n = {n_max:,})\n"
        f"AVL  vs  SkipList p=0.25  vs  SkipList p=0.80",
        fontsize=11, fontweight="bold",
    )
    fig.tight_layout()
    fig.savefig(out, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  Guardado: {out.name}")


def plot_summary_bars(df, out):
    """Un panel con 3 grupos (insert/search/remove), barras por estructura
    promediando todos los tamanos y ordenes. Vista global rapida."""
    labels = ["AVL", "SL p=0.25", "SL p=0.80"]
    op_labels = list(OPS.values())
    op_keys   = list(OPS.keys())

    means = {lbl: [df[(df.label == lbl) & (df.operation == op)]["mean_ns"].mean()
                   for op in op_keys]
             for lbl in labels}

    x = range(len(op_keys))
    width = 0.25
    fig, ax = plt.subplots(figsize=(8, 4))
    for i, lbl in enumerate(labels):
        offset = (i - 1) * width
        bars = ax.bar([xi + offset for xi in x], means[lbl], width,
                      label=lbl, color=BAR_COLORS[lbl], edgecolor="white")
        # Etiqueta encima de cada barra
        for bar in bars:
            h = bar.get_height()
            ax.text(bar.get_x() + bar.get_width() / 2, h * 1.02,
                    f"{h:.0f}", ha="center", va="bottom", fontsize=7)

    ax.set_xticks(list(x))
    ax.set_xticklabels(op_labels, fontsize=10)
    ax.set_ylabel("ns por operacion (promedio global)")
    ax.set_title("Resumen global: AVL vs SkipList p=0.25 vs SkipList p=0.80",
                 fontweight="bold")
    ax.legend(fontsize=9)
    ax.grid(axis="y", alpha=0.35)
    fig.tight_layout()
    fig.savefig(out, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  Guardado: {out.name}")


# ── Main ──────────────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe",   default=None)
    parser.add_argument("--sizes", default="1000,10000,100000")
    parser.add_argument("--out",   default="figures")
    args = parser.parse_args()

    exe     = find_exe(args.exe)
    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"\nBinario : {exe}")
    print(f"Tamanos : {args.sizes}")
    print(f"Figuras : {out_dir}/\n")

    with tempfile.TemporaryDirectory(prefix="cmp_p_") as tmp:
        tmp = Path(tmp)
        print("[1/2] Benchmark p=0.25 ...")
        csv25 = run_benchmark(exe, 0.25, args.sizes, tmp / "p025")
        print("[2/2] Benchmark p=0.80 ...")
        csv80 = run_benchmark(exe, 0.80, args.sizes, tmp / "p080")
        df = build_combined(csv25, csv80)

    print("\nGenerando figuras...")
    for i, (op, label) in enumerate(OPS.items(), start=1):
        plot_vs_n(df, op, f"C{i}: {label} - comparativa de p", out_dir / f"cmp_p_c{i}_{op}.pdf")

    plot_order_bars(df, out_dir / "cmp_p_orden.pdf")
    plot_summary_bars(df, out_dir / "cmp_p_resumen.pdf")

    print(f"\nListo. 5 figuras guardadas en {out_dir}/")


if __name__ == "__main__":
    main()

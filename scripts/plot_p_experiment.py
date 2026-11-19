"""Genera figuras C1-C4 para el experimento de variacion de p.

Combina:
  results/summary.csv      -> AVL + SkipList p=0.50  (n 1K-1M, 10 reps)
  results/p025/summary.csv -> SkipList p=0.25        (n 1K-1M, 10 reps)
  results/p080/summary.csv -> SkipList p=0.80        (n 1K-100K, 10 reps)

Figuras generadas en figures/:
  c1_search.pdf   - Busqueda: ns/op vs n por orden (G1-style, 4 series)
  c2_insert.pdf   - Insercion: ns/op vs n por orden
  c3_remove.pdf   - Eliminacion: ns/op vs n por orden
  c4_orden.pdf    - Efecto del orden a n=100K (G4-style, 4 series)

Uso: python scripts/plot_p_experiment.py
"""
import sys
sys.stdout.reconfigure(encoding="utf-8")

from pathlib import Path
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# ── Rutas ─────────────────────────────────────────────────────────────────────
ROOT    = Path(__file__).parent.parent
CSV_050 = ROOT / "results" / "summary.csv"
CSV_025 = ROOT / "results" / "p025" / "summary.csv"
CSV_080 = ROOT / "results" / "p080" / "summary.csv"
OUTDIR  = ROOT / "figures"

# ── Paleta y estilos ──────────────────────────────────────────────────────────
STYLES = {
    "AVL":        dict(color="#d62728", marker="s", ls="-",  lw=1.8, ms=5),
    "SL p=0.25":  dict(color="#1f77b4", marker="o", ls="-",  lw=1.8, ms=5),
    "SL p=0.50":  dict(color="#ff7f0e", marker="D", ls="--", lw=1.6, ms=5),
    "SL p=0.80":  dict(color="#2ca02c", marker="^", ls=":",  lw=1.6, ms=5),
}
BAR_COLORS = {k: v["color"] for k, v in STYLES.items()}
ORDERS  = ["aleatorio", "ordenado", "inverso"]
OPS     = {"search": "Busqueda", "insert": "Insercion", "remove": "Eliminacion"}
LABELS  = list(STYLES.keys())


# ── Carga y etiquetado ────────────────────────────────────────────────────────
def load_and_label() -> pd.DataFrame:
    frames = []
    df_050 = pd.read_csv(CSV_050)
    avl = df_050[df_050["structure"] == "AVL"].copy()
    avl["label"] = "AVL"
    frames.append(avl)

    for csv, skip_label in [(CSV_050, "SL p=0.50"),
                            (CSV_025, "SL p=0.25"),
                            (CSV_080, "SL p=0.80")]:
        df = pd.read_csv(csv)
        skip = df[df["structure"].str.startswith("SkipList")].copy()
        skip["label"] = skip_label
        frames.append(skip)

    return pd.concat(frames, ignore_index=True)


# ── Figura estilo G1-G3 ───────────────────────────────────────────────────────
def plot_vs_n(df: pd.DataFrame, op: str, title: str, out: Path) -> None:
    fig, axes = plt.subplots(1, 3, figsize=(14, 4), sharey=False)

    for ax, order in zip(axes, ORDERS):
        sub = df[(df["operation"] == op) & (df["order"] == order)]
        for lbl in LABELS:
            g = sub[sub["label"] == lbl].sort_values("n")
            if g.empty:
                continue
            st = STYLES[lbl]
            ax.errorbar(g["n"], g["mean_ns"], yerr=g["std_ns"],
                        label=lbl, capsize=3,
                        color=st["color"], marker=st["marker"],
                        linestyle=st["ls"], linewidth=st["lw"],
                        markersize=st["ms"])
        ax.set_xscale("log")
        ax.set_title(f"orden {order}", fontsize=10)
        ax.set_xlabel("n", fontsize=9)
        ax.grid(alpha=0.3)
        ax.tick_params(labelsize=8)

    axes[0].set_ylabel("ns por operacion", fontsize=9)
    axes[0].legend(fontsize=8, framealpha=0.8, loc="upper left")
    fig.suptitle(title, fontsize=12, fontweight="bold")
    fig.tight_layout()
    fig.savefig(out, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  Guardado: {out.name}")


# ── Figura estilo G4 ──────────────────────────────────────────────────────────
def plot_order_effect(df: pd.DataFrame, out: Path) -> None:
    # Usamos n=100K (el maximo que tienen las 4 series)
    n_ref = 100_000
    sub   = df[df["n"] == n_ref]

    fig, axes = plt.subplots(1, 3, figsize=(14, 4.2))
    bar_width = 0.18
    x = range(len(ORDERS))

    for ax, (op, op_label) in zip(axes, OPS.items()):
        rows = sub[sub["operation"] == op]
        for i, lbl in enumerate(LABELS):
            g = rows[rows["label"] == lbl].set_index("order")
            means = [g.loc[o, "mean_ns"] if o in g.index else float("nan") for o in ORDERS]
            errs  = [g.loc[o, "std_ns"]  if o in g.index else 0            for o in ORDERS]
            offset = (i - 1.5) * bar_width
            ax.bar([xi + offset for xi in x], means, bar_width,
                   label=lbl, color=BAR_COLORS[lbl],
                   yerr=errs, capsize=3, edgecolor="white", linewidth=0.5)

        ax.set_xticks(list(x))
        ax.set_xticklabels(ORDERS, fontsize=9)
        ax.set_title(op_label, fontsize=11, fontweight="bold")
        ax.set_xlabel("")
        ax.grid(axis="y", alpha=0.3)
        ax.tick_params(labelsize=8)
        if ax is axes[0]:
            ax.set_ylabel("ns por operacion", fontsize=9)
            ax.legend(fontsize=8, framealpha=0.8)

    fig.suptitle(
        f"C4: efecto del orden de los datos  (n = {n_ref:,})\n"
        "AVL  |  SkipList p=0.25  |  SkipList p=0.50  |  SkipList p=0.80",
        fontsize=10, fontweight="bold",
    )
    fig.tight_layout()
    fig.savefig(out, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"  Guardado: {out.name}")


# ── Tabla resumen (stdout) ────────────────────────────────────────────────────
def print_table(df: pd.DataFrame) -> None:
    print("\nResumen a n=100,000 (mean_ns):")
    n_ref = 100_000
    sub = df[df["n"] == n_ref]
    hdr = f"{'orden':<12} {'op':<8} {'AVL':>10} {'SL 0.25':>10} {'SL 0.50':>10} {'SL 0.80':>10}"
    print(hdr)
    print("-" * len(hdr))
    for op in OPS:
        for order in ORDERS:
            row = sub[(sub["operation"] == op) & (sub["order"] == order)]
            vals = {}
            for lbl in LABELS:
                r = row[row["label"] == lbl]
                vals[lbl] = f"{r['mean_ns'].values[0]:>10.1f}" if not r.empty else f"{'---':>10}"
            print(f"{order:<12} {op:<8} {vals['AVL']} {vals['SL p=0.25']} {vals['SL p=0.50']} {vals['SL p=0.80']}")
    print()


# ── Main ──────────────────────────────────────────────────────────────────────
def main() -> None:
    for csv in (CSV_050, CSV_025, CSV_080):
        if not csv.exists():
            sys.exit(f"No se encuentra {csv}. Corre los benchmarks primero.")

    OUTDIR.mkdir(parents=True, exist_ok=True)
    df = load_and_label()

    print(f"Datos cargados: {len(df)} filas")
    print(f"Etiquetas:      {sorted(df['label'].unique())}")
    print(f"Tamanos:        {sorted(df['n'].unique())}\n")

    print("Generando figuras...")
    for i, (op, label) in enumerate(OPS.items(), start=1):
        plot_vs_n(df, op, f"C{i}: {label} - variacion de p", OUTDIR / f"c{i}_{op}.pdf")

    plot_order_effect(df, OUTDIR / "c4_orden.pdf")
    print_table(df)
    print(f"Listo. Figuras en {OUTDIR}/")


if __name__ == "__main__":
    main()

"""Compara Skip List con p=0.25 vs p=0.80.

Uso: python scripts/compare_p.py [--exe ruta/al/benchmark] [--sizes 1000,10000,100000]
Requiere que el binario 'benchmark' ya esté compilado.
"""
import subprocess
import sys
import shutil
import tempfile
import argparse
from pathlib import Path

sys.stdout.reconfigure(encoding='utf-8')
sys.stderr.reconfigure(encoding='utf-8')

import pandas as pd

# ── Colores ANSI ──────────────────────────────────────────────────────────────
GRN = "\033[32m"
RED = "\033[31m"
YEL = "\033[33m"
RST = "\033[0m"
BLD = "\033[1m"


def find_exe(hint: str | None) -> Path:
    candidates = [
        hint,
        "build/benchmark",
        "build/benchmark.exe",
        "build/Release/benchmark.exe",
        "build/Debug/benchmark.exe",
    ]
    for c in candidates:
        if c and shutil.which(c):
            return Path(c)
        if c and Path(c).is_file():
            return Path(c)
    sys.exit(
        f"{RED}No se encontró el binario 'benchmark'. "
        f"Compila primero con:\n  cmake -B build && cmake --build build --config Release{RST}"
    )


def run_benchmark(exe: Path, p: float, sizes: str, out_dir: Path) -> Path:
    out_dir.mkdir(parents=True, exist_ok=True)
    cmd = [
        str(exe),
        "--p", str(p),
        "--sizes", sizes,
        "--reps", "7",
        "--warmup", "1",
        "--orders", "aleatorio,ordenado,inverso",
        "--out", str(out_dir),
    ]
    print(f"  Ejecutando p={p}  →  {out_dir}/summary.csv")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(f"{RED}Error al ejecutar benchmark:\n{result.stderr}{RST}")
    return out_dir / "summary.csv"


def load_all(csv: Path) -> pd.DataFrame:
    return pd.read_csv(csv)


def winner_label(vals: dict) -> str:
    best_key = min(vals, key=vals.get)
    best_val = vals[best_key]
    labels = []
    for k, v in vals.items():
        pct = (v - best_val) / best_val * 100 if best_val > 0 else 0
        if pct < 2:
            labels.append(f"{GRN}{k}{RST}")
        else:
            labels.append(f"{k}")
    return " | ".join(labels)


def compare(df25: pd.DataFrame, df80: pd.DataFrame) -> None:
    # df25 y df80 tienen AVL + SkipList de cada corrida; AVL es idéntico en ambas
    # Tomamos AVL de df25 y renombramos las SkipList para distinguirlas
    avl = df25[df25["structure"] == "AVL"].copy()
    sl25 = df25[df25["structure"].str.startswith("SkipList")].copy()
    sl80 = df80[df80["structure"].str.startswith("SkipList")].copy()

    avl["label"]  = "AVL"
    sl25["label"] = "SL p=0.25"
    sl80["label"] = "SL p=0.80"

    combined = pd.concat([avl, sl25, sl80], ignore_index=True)

    ops    = ["insert", "search", "remove"]
    orders = ["aleatorio", "ordenado", "inverso"]
    labels = ["AVL", "SL p=0.25", "SL p=0.80"]

    print(f"\n{BLD}{'='*80}{RST}")
    print(f"{BLD}  AVL  vs  SkipList p=0.25  vs  SkipList p=0.80   (ns/op, 7 reps){RST}")
    print(f"{BLD}{'='*80}{RST}")

    for op in ops:
        print(f"\n{BLD}  {op.upper()}{RST}")
        hdr = f"  {'n':>10}  {'orden':<12}  {'AVL':>12}  {'SL p=0.25':>12}  {'SL p=0.80':>12}  mejor"
        print(hdr)
        print("  " + "-" * 76)
        sub = combined[combined["operation"] == op]
        for n in sorted(sub["n"].unique()):
            for order in orders:
                row = sub[(sub["n"] == n) & (sub["order"] == order)]
                vals = {}
                for lbl in labels:
                    r = row[row["label"] == lbl]
                    vals[lbl] = r["mean_ns"].values[0] if len(r) else float("nan")

                best = min(v for v in vals.values() if v == v)

                def fmt(v):
                    if v != v:
                        return f"{'---':>12}"
                    pct = (v - best) / best * 100 if best > 0 else 0
                    if pct < 2:
                        return f"{GRN}{v:>12.1f}{RST}"
                    elif pct > 50:
                        return f"{RED}{v:>12.1f}{RST}"
                    else:
                        return f"{v:>12.1f}"

                best_lbl = min(vals, key=vals.get)
                print(
                    f"  {int(n):>10,}  {order:<12}  "
                    f"{fmt(vals['AVL'])}  {fmt(vals['SL p=0.25'])}  {fmt(vals['SL p=0.80'])}"
                    f"  {GRN}{best_lbl}{RST}"
                )

    # Resumen global
    print(f"\n{BLD}{'='*80}{RST}")
    print(f"{BLD}  RESUMEN GLOBAL (promedio de todos los tamanos y ordenes){RST}")
    print(f"{BLD}{'='*80}{RST}")
    print(f"  {'operacion':<10}  {'AVL':>12}  {'SL p=0.25':>12}  {'SL p=0.80':>12}  mejor")
    print("  " + "-" * 60)
    for op in ops:
        sub = combined[combined["operation"] == op]
        vals = {lbl: sub[sub["label"] == lbl]["mean_ns"].mean() for lbl in labels}
        best = min(vals, key=vals.get)

        def fmt_s(lbl):
            v = vals[lbl]
            return f"{GRN}{v:>12.1f}{RST}" if lbl == best else f"{v:>12.1f}"

        print(f"  {op:<10}  {fmt_s('AVL')}  {fmt_s('SL p=0.25')}  {fmt_s('SL p=0.80')}  {GRN}{best}{RST}")
    print()


def main():
    parser = argparse.ArgumentParser(description="Compara AVL vs SkipList p=0.25 vs p=0.80")
    parser.add_argument("--exe", default=None, help="Ruta al binario benchmark")
    parser.add_argument("--sizes", default="1000,10000,100000",
                        help="Tamanos n separados por coma (default: 1000,10000,100000)")
    args = parser.parse_args()

    exe = find_exe(args.exe)
    print(f"\n{BLD}Binario:{RST} {exe}")
    print(f"{BLD}Tamanos:{RST} {args.sizes}\n")

    with tempfile.TemporaryDirectory(prefix="skiplist_cmp_") as tmp:
        tmp = Path(tmp)
        print(f"{BLD}[1/2] Corriendo benchmark con p=0.25...{RST}")
        csv25 = run_benchmark(exe, 0.25, args.sizes, tmp / "p025")

        print(f"\n{BLD}[2/2] Corriendo benchmark con p=0.80...{RST}")
        csv80 = run_benchmark(exe, 0.80, args.sizes, tmp / "p080")

        df25 = load_all(csv25)
        df80 = load_all(csv80)

    compare(df25, df80)


if __name__ == "__main__":
    main()

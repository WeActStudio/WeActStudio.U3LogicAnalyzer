# This file is part of the LogicAnalyzer project.
#
# Copyright (C) 2026 WeAct Studio
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Render doc/usage.md and doc/usage.zh_CN.md into the two PDFs that ship with
the application.

    python packaging/make_usage_pdf.py            # both languages
    python packaging/make_usage_pdf.py --lang zh  # Chinese only

Needs markdown, PyMuPDF and Chrome (used headless for the HTML -> PDF step).
The output is committed to doc/, so the normal build never has to run this.
"""
import argparse
import datetime
import re
import subprocess
import sys
import tempfile
from pathlib import Path

import markdown
import pymupdf

CHROME = r"C:\Program Files\Google\Chrome\Application\chrome.exe"
BRAND = "WeAct Studio"
SITE = "www.WeActStudio.com"

CSS = {
    "zh": """
body { font-family: "Microsoft YaHei", sans-serif; }
""",
    "en": """
body { font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif; }
""",
}

BASE_CSS = """
@page { size: A4; margin: 15mm 14mm 16mm 14mm; }
* { box-sizing: border-box; }
body { font-size: 10pt; line-height: 1.62; color: #1f2328; margin: 0; counter-reset: sec; }
h1 { font-size: 20pt; color: #10233f; margin: 0 0 2mm; }
.brand { font-size: 9pt; color: #5b6472; letter-spacing: 1pt; margin: 0 0 6mm; }
h2 { font-size: 13pt; color: #14315c; margin: 16pt 0 7pt; padding-left: 8pt;
     border-left: 4px solid #1a73e8; page-break-after: avoid; counter-increment: sec; }
h2::before { content: counter(sec) ". "; color: #1a73e8; }
h3 { font-size: 11pt; color: #14315c; margin: 12pt 0 5pt; page-break-after: avoid; }
p { margin: 5pt 0; }
strong { color: #10233f; }
ul, ol { margin: 5pt 0 5pt 18pt; padding: 0; }
li { margin: 2.5pt 0; }
table { border-collapse: collapse; width: 100%; margin: 7pt 0; font-size: 9pt; }
tr { page-break-inside: avoid; }
thead { display: table-header-group; }
th, td { border: 1px solid #d0d7de; padding: 4px 7px; text-align: left; vertical-align: top; }
td:first-child { min-width: 30mm; }
th { background: #f2f6fc; font-weight: 600; }
tr:nth-child(even) td { background: #fafbfc; }
code { background: #f0f2f5; padding: 1px 4px; border-radius: 3px;
       font-family: Consolas, "SF Mono", monospace; font-size: 9pt; }
td code { white-space: nowrap; }
pre { background: #f6f8fa; border: 1px solid #e3e7ec; border-radius: 4px; padding: 7pt 9pt;
      page-break-inside: avoid; overflow: auto; }
pre code { background: none; padding: 0; }
img { max-width: 100%; page-break-inside: avoid; }
.toc { page-break-after: always; }
.toc h1 { margin: 0 0 4mm; }
.toc .rule { height: 3px; background: #1a73e8; width: 30mm; margin-bottom: 8mm; }
.toc-item { display: flex; align-items: baseline; font-size: 10.5pt; margin: 6pt 0; }
.toc-item .d { flex: 1; border-bottom: 1px dotted #c9d1d9; margin: 0 6px 3px; }
.toc-item .p { color: #57606a; }
"""

TOC_TITLE = {"zh": "目录", "en": "Contents"}
FONT = {"zh": "china-s", "en": "helv"}


def print_pdf(html, pdf):
    subprocess.run([CHROME, "--headless=new", "--disable-gpu", "--no-pdf-header-footer",
                    "--run-all-compositor-stages-before-draw", "--virtual-time-budget=6000",
                    f"--print-to-pdf={pdf}", "file:///" + html.replace("\\", "/").lstrip("/")],
                   check=True, capture_output=True)


def split_sections(text):
    """Return the title, the lead paragraphs and the (heading, body) pairs."""
    title = text.splitlines()[0].lstrip("# ").strip()
    chunks = text.split("\n## ")
    # The language switcher and the README link belong to the repository, not
    # to the printed guide.
    lead = "\n".join(
        line for line in chunks[0].splitlines()[1:]
        if line.strip() and not (line.startswith("[") and "README" in line))
    heads, bodies = [], []
    for chunk in chunks[1:]:
        head, _, body = chunk.partition("\n")
        heads.append(head.strip())
        bodies.append(body)
    return title, lead, heads, bodies


def version_of(md_path):
    """Read the release version the guide is printed for."""
    cmake = Path(md_path).resolve().parents[1] / "pulseview" / "CMakeLists.txt"
    try:
        text = cmake.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""
    found = re.search(r'set\(PV_VERSION_STRING\s+"([^"]+)"', text)
    return found.group(1).split("-git-")[0] if found else ""


def build(md_path, out_pdf, lang):
    text = Path(md_path).read_text(encoding="utf-8")
    title, lead, heads, bodies = split_sections(text)
    base = Path(md_path).resolve().parent.as_uri() + "/"
    version = version_of(md_path)
    stamp = " · ".join(x for x in (BRAND, SITE, f"V{version}" if version else "",
                                   datetime.date.today().isoformat()) if x)

    body_html = markdown.markdown(
        "\n\n".join(f"## {h}\n{b}" for h, b in zip(heads, bodies)),
        extensions=["tables", "fenced_code"])
    body_html = markdown.markdown(lead, extensions=["tables", "fenced_code"]) + body_html

    def render(toc):
        items = "".join(
            f'<div class="toc-item"><span class="t">{n + 1}. {h}</span><span class="d"></span>'
            f'<span class="p">{toc.get(h, 0)}</span></div>' for n, h in enumerate(heads))
        return (f'<!DOCTYPE html><html lang="{lang}"><head><meta charset="utf-8">'
                f'<base href="{base}"><title>{title}</title>'
                f'<style>{BASE_CSS}{CSS[lang]}</style></head><body>'
                f'<h1>{title}</h1><div class="brand">{stamp}</div>'
                f'<div class="toc"><h1>{TOC_TITLE[lang]}</h1><div class="rule"></div>{items}</div>'
                f'{body_html}</body></html>')

    with tempfile.TemporaryDirectory() as tmp:
        # First pass numbers the pages with a placeholder TOC; the headings are
        # then located in that PDF and the TOC is written a second time.
        first_html = Path(tmp, "pass1.html")
        first_html.write_text(render({}), encoding="utf-8")
        first_pdf = str(Path(tmp, "pass1.pdf"))
        print_pdf(str(first_html), first_pdf)

        doc = pymupdf.open(first_pdf)
        pages = {}
        for index in range(1, doc.page_count):          # page 0 holds the TOC
            lines = [l.strip() for l in doc[index].get_text().splitlines()]
            for head in heads:
                if head not in pages and f"{heads.index(head) + 1}. {head}" in lines:
                    pages[head] = index + 1
        doc.close()

        second_html = Path(tmp, "pass2.html")
        second_html.write_text(render(pages), encoding="utf-8")
        second_pdf = str(Path(tmp, "pass2.pdf"))
        print_pdf(str(second_html), second_pdf)

        doc = pymupdf.open(second_pdf)
        total = doc.page_count
        for index in range(total):
            page = doc[index]
            page.insert_text((40, page.rect.height - 18), f"{BRAND} · {SITE}",
                             fontname="helv", fontsize=8, color=(0.6, 0.64, 0.7))
            if index:   # the title page carries no page number
                page.insert_text((page.rect.width - 62, page.rect.height - 18),
                                 f"{index + 1} / {total}",
                                 fontname="helv", fontsize=8, color=(0.6, 0.64, 0.7))
        doc.set_toc([[1, f"{i + 1}. {h}", pages.get(h, 1)] for i, h in enumerate(heads)])
        doc.save(str(out_pdf), garbage=4, deflate=True)
        doc.close()

    missing = [h for h in heads if h not in pages]
    print(f"{Path(out_pdf).name}: {total} pages, {len(heads)} sections"
          + (f", headings not found: {missing}" if missing else ""))


def main():
    doc_dir = Path(__file__).resolve().parents[1] / "doc"
    ap = argparse.ArgumentParser(description="Render the usage guide PDFs.")
    ap.add_argument("--lang", choices=("zh", "en", "all"), default="all")
    args = ap.parse_args()

    jobs = [("en", "usage.md", "U3LogicAnalyzer-UsageGuide.pdf"),
            ("zh", "usage.zh_CN.md", "U3LogicAnalyzer-UsageGuide.zh_CN.pdf")]
    for lang, source, target in jobs:
        if args.lang not in (lang, "all"):
            continue
        build(doc_dir / source, doc_dir / target, lang)


if __name__ == "__main__":
    sys.exit(main())

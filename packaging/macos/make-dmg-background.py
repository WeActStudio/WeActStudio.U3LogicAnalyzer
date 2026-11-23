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

"""Render the disk image background: title, drag arrow, languages, brand.

Needs Pillow and the macOS system fonts. Pass PV_MACOS_FONTS to point at a
directory holding copies of them, otherwise the system locations are used.
"""
import glob
import os
import sys

from PIL import Image, ImageDraw, ImageFont

W, H = 600, 400

# Layout is in points on a 600x400 window; Finder draws the two icons on top
# at these same coordinates (see dmg-settings.py).
ICON_Y = 165
ARROW_X1, ARROW_X2 = 238, 362
INK = (29, 29, 31)
SUB = (110, 110, 115)
FAINT = (134, 134, 139)
ARROW = (160, 160, 168)
RULE = (220, 220, 226)


def find_font(name):
    local = os.environ.get("PV_MACOS_FONTS")
    if local:
        path = os.path.join(local, name)
        if os.path.exists(path):
            return path
        sys.exit("font not found: " + path)
    for pattern in ("/System/Library/Fonts/" + name,
                    "/System/Library/Fonts/Supplemental/" + name,
                    "/System/Library/AssetsV2/*/*/AssetData/" + name,
                    os.path.expanduser("~/Library/Fonts/" + name)):
        hit = sorted(glob.glob(pattern))
        if hit:
            return hit[0]
    sys.exit("font not found: " + name)


PINGFANG = find_font("PingFang.ttc")
HELVETICA = find_font("Helvetica.ttc")
ARIAL = find_font("Arial.ttf")
SYMBOLS = find_font("Apple Symbols.ttf")

# PingFang.ttc packs HK, MO, TC and SC faces in that order, five weights each.
# Index 0 is Hong Kong and draws 将 with the traditional left part.
PINGFANG_SC = 3

TITLE_ZH = "将 U3LogicAnalyzer 拖入 Applications 文件夹"
TITLE_EN = "Drag U3LogicAnalyzer into the Applications folder"
LANGS = [
    ("Ziehen Sie U3LogicAnalyzer in den Ordner \u201eApplications\u201c", HELVETICA),
    ("Arrastre U3LogicAnalyzer a la carpeta \u00abApplications\u00bb", HELVETICA),
    ("\u041f\u0435\u0440\u0435\u0442\u0430\u0449\u0438\u0442\u0435 U3LogicAnalyzer \u0432 "
     "\u043f\u0430\u043f\u043a\u0443 \u00abApplications\u00bb", ARIAL),
]
# One shortcut, five verbs: the hint is read on every system language the
# application ships, and repeating "⌘E" five times reads worse than once.
EJECT = ("\u2318", "E", "  \u63a8\u51fa \u00b7 Eject \u00b7 Auswerfen \u00b7 Expulsar "
         "\u00b7 \u0418\u0437\u0432\u043b\u0435\u0447\u044c")
BRAND = "WeAct Studio"
TAGLINE = "Open-source logic analyzer built on sigrok and PulseView"

OUT = os.environ.get("PV_DMG_BG_OUT", ".")
os.makedirs(OUT, exist_ok=True)


def render(scale):
    w, h = W * scale, H * scale
    img = Image.new("RGB", (w, h))
    d = ImageDraw.Draw(img)

    top, bottom = (250, 250, 253), (236, 236, 241)
    for y in range(h):
        t = y / max(1, h - 1)
        d.line([(0, y), (w, y)],
               fill=tuple(int(top[i] + (bottom[i] - top[i]) * t) for i in range(3)))

    def f(path, size, index=0):
        return ImageFont.truetype(path, int(round(size * scale)), index=index)

    def mid(text, cy, font, fill):
        d.text((w / 2, cy * scale), text, font=font, fill=fill, anchor="mm")

    def runs(parts, cy, fill):
        widths = [d.textlength(t, font=fnt) for t, fnt in parts]
        x = (w - sum(widths)) / 2
        for (t, fnt), tw in zip(parts, widths):
            d.text((x, cy * scale), t, font=fnt, fill=fill, anchor="lm")
            x += tw

    def tracked(text, cy, font, fill, gap):
        widths = [d.textlength(c, font=font) for c in text]
        total = sum(widths) + gap * scale * (len(text) - 1)
        x = (w - total) / 2
        for c, cw in zip(text, widths):
            d.text((x, cy * scale), c, font=font, fill=fill, anchor="lm")
            x += cw + gap * scale

    mid(TITLE_ZH, 46, f(PINGFANG, 20, PINGFANG_SC), INK)
    mid(TITLE_EN, 79, f(HELVETICA, 13), SUB)

    ay = ICON_Y * scale
    x1, x2 = ARROW_X1 * scale, ARROW_X2 * scale
    head = 13 * scale
    d.line([(x1, ay), (x2 - head + 2 * scale, ay)], fill=ARROW,
           width=max(2, int(round(2.2 * scale))))
    d.polygon([(x2, ay), (x2 - head, ay - head * 0.62), (x2 - head, ay + head * 0.62)],
              fill=ARROW)

    y = 272
    for text, path in LANGS:
        mid(text, y, f(path, 10.5), FAINT)
        y += 18

    runs([(EJECT[0], f(SYMBOLS, 10)), (EJECT[1], f(HELVETICA, 10)),
          (EJECT[2], f(PINGFANG, 9.5, PINGFANG_SC))], 328, FAINT)

    d.line([(90 * scale, 350 * scale), (510 * scale, 350 * scale)], fill=RULE,
           width=max(1, scale))

    tracked(BRAND, 366, f(HELVETICA, 12), (72, 72, 74), 1.1)
    mid(TAGLINE, 385, f(HELVETICA, 9.5), FAINT)

    name = "dmg-background.png" if scale == 1 else "dmg-background@2x.png"
    path = os.path.join(OUT, name)
    # 143.99 DPI rounds the 1200 px image to 600.04 pt and Finder then shows a
    # scroll bar; 144.07 lands just under the 600 pt window.
    img.save(path, "PNG", dpi=(144.07 if scale == 2 else 72.04,) * 2)
    print(name, img.size, path)


render(1)
render(2)

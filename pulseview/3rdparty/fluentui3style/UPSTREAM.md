# FluentUI3Style (vendored)

- Upstream: <https://github.com/XHY-ChuJian/FluentUIStyle>
- Revision: `b8a7ce5b36cbc022b86227590deaa033fcc55534` (master, 2026-09-18)
- License: MIT (see `LICENSE`)

## What is taken

`fluentui3style/` only, copied to `src/`:

- `src/`                the style itself, minus its `plugin/` sub-directory
- `FluentUI3Colors/`    `fluentui3colors.h`, which `fluentui3style.h` includes
- `LICENSE`             the MIT license of the upstream project

Not taken: `ExWidgets`, `frameless` (and the vendored QWindowKit below
`3rd/`), `Examples`, `QtCreatorPlugin`, `PyExamples`, `tests`. The style
links nothing but Qt Core, Gui, Widgets and Svg.

## Local changes

Three kinds of change were needed to link the style into an application
that is built against a *static* Qt. Upstream ships it as a shared library
or a plugin, where none of this comes up.

1. `src/fluentui3style_global.h`: a `FLUENTUI3STYLE_STATIC` branch was
   added so the export macros expand to nothing. Upstream only knows the
   shared library and the plugin case; this copy is built as a static
   library. `CMakeLists.txt` defines `FLUENTUI3STYLE_STATIC`.

2. Names for the copies of Qt-internal classes. The style carries its own
   copies of a few classes that live in Qt itself, which is harmless
   inside a shared library but not once the object files are linked into
   an application: the linker then sees two definitions of the same class
   with *different layouts* (it warns that the vtable sections have
   different sizes), and whichever one wins corrupts the other's objects.
   The copies were given names of their own:

   | upstream | here |
   | --- | --- |
   | `QStyleHelper` | `FluentUI3StyleHelper` |
   | `qDrawShadeRect`, `qDrawShadePanel` | `fluentui3DrawShadeRect`, `fluentui3DrawShadePanel` |
   | `QStyleAnimation` | `FluentUI3StyleAnimation` |
   | `QProgressStyleAnimation` | `FluentUI3ProgressStyleAnimation` |
   | `QNumberStyleAnimation` | `FluentUI3NumberStyleAnimation` |
   | `QBlendStyleAnimation` | `FluentUI3BlendStyleAnimation` |
   | `QScrollbarStyleAnimation` | `FluentUI3ScrollbarStyleAnimation` |

   Files touched: `src/qstylehelper_p.h`, `src/qstylehelper.cpp`,
   `src/qstyleanimation_p.h`, `src/qstyleanimation.cpp`,
   `src/fluentui3style.cpp`.

3. `CMakeLists.txt` (this directory): builds a static library instead of a
   shared one, and keeps Qt from adding its plugin-import sources to the
   target (`QT_NO_PLUGINS`), because the application already links the
   plugins it wants and would otherwise register every one of them twice.

4. `src/fluentui3style.cpp`, `unpolish(QWidget *)`: the viewport of a scroll
   area is only put back where `polish()` took it over. Upstream takes the
   viewport over only when the scroll area carries a styled frame, but puts
   it back unconditionally, and reading back the property it never wrote
   yields false. That switches the fill of the viewport off, and a viewport
   that fills its own background -- the wave area of this application, which
   is a `NoFrame` scroll area -- then shows an unpainted block.

5. `src/fluentui3comboboxdelegate_p.{h,cpp}` (new): the rows of a combo box
   popup are handed to the style. Qt shows that popup through a view whose
   delegate it makes itself, `QComboBoxDelegate`, and that one is a
   `QItemDelegate` on purpose -- `qcombobox_p.h` in 5.15 keeps the popup out
   of the styled path, because the desktop it was written for did not theme
   it. A `QItemDelegate` fills the row of the selection with the highlight of
   the palette and frames it with a focus rectangle of its own, and never
   asks the style to draw an item view item, so the popup was the one list in
   the application that followed neither the style nor the palette.
   `polishComboBoxPopupView()` puts a delegate in its place that hands the
   row to the style the way the row of a `QListView` is handed over, and
   keeps the separator rows of a combo box working. It is called from
   `polish(QWidget *)`, next to `polishCalendarWidget()`, which is there for
   the same kind of reason.

6. `src/fluentui3style.cpp`, `polish(QWidget *)` and `unpolish(QWidget *)`:
   the height of a spin box is pinned to the one this style measures, and
   given back when the style is taken off the widget. A spin box is as high
   as the line edit it holds plus its own frame, and a spin box that is in
   the scope of a style sheet -- one that carries a style sheet itself, or
   one below a widget that does, which every spin box of the tool bar of
   this application is -- is measured by the style sheet style. That one
   does not ask the style in use how high a spin box is: it takes the
   height of one line of text, so such a spin box came out about half as
   high as the widgets beside it. The height is measured by
   `spinBoxHeight()` and put on the widget with `setMinimumHeight()`, and
   the height the widget had is kept in a property of its own so that
   `unpolish()` can put it back.

7. `src/fluentui3styleproperties.h`, `src/menupopupanimation_p.cpp`,
   `src/fluentui3style.cpp`: the length of the menu open animation is no
   longer the fixed 400 ms of upstream, it is read from a property, and the
   built-in default is 150 ms. Upstream opens every menu over 400 ms, which
   is a whole two fifths of a second of a menu that does not answer the
   mouse either: while the animation runs, the event filter of the animator
   swallows the mouse and key events of that menu. The property is
   `menuPopupAnimationDuration`, it is read on every popup -- from the menu
   first, then from `QApplication`, then from what `polish()` put on the
   animator -- so a value set at run time takes effect on the next menu, and
   a value of 0 or less skips the animation altogether. The same is already
   true of upstream's `menuPopupAnimationEnabled`, which switches it off
   altogether.

## Why it is built as a static library

Upstream ships the style as a Qt style plugin so that an application can
say `app.setStyle("FluentUI3")`. That cannot work here: this application is
linked against a static Qt, which has no plugin loader at run time, so
plugins have to be linked into the binary. The style is therefore built as
a static library and instantiated directly, which is what upstream
describes for platforms where loading the plugin does not work.

## Updating

Replace `src/` with the new `fluentui3style/` (again minus `plugin/`) and
`FluentUI3Colors/fluentui3colors.h`, then re-apply the local changes listed
above and update the revision here. `src/fluentui3comboboxdelegate_p.{h,cpp}`
is a file of this copy alone, so it is lost by the replacement and has to be
put back along with the call to it.

## Name of the entry it fills

In the widget style setting this style is listed as **FluentUI3**, after
the style itself, and not after the version of Windows whose look it imitates.
The upstream project reproduces that look on its own and publishes it under
the MIT license, which permits the copying but carries no right to the name
of the desktop. The two are kept apart here on purpose: do not rename the
entry back, and do not put the name of a Windows version on any control,
dialog or documentation entry that hands this style to the user.

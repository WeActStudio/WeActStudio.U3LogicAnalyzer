#pragma once

class QComboBox;

/*
 * Hands the rows of the popup of `combo` over to the style.
 *
 * Qt shows the popup of a combo box through a view of its own, and it gives
 * that view a delegate of its own as well: `QComboBoxDelegate`, which is a
 * `QItemDelegate` rather than a `QStyledItemDelegate`.  Qt 5.15 says why in
 * `qcombobox_p.h` -- the desktop it was written for did not theme the popup,
 * so the popup was kept out of the styled path -- but the effect of that
 * choice is larger than the reason for it: a `QItemDelegate` fills the row of
 * the selection with the highlight of the palette and frames it with a focus
 * rectangle of its own, and it never asks the style to draw an item view item
 * at all, so the popup follows neither the style nor the palette in use.
 *
 * The rows of every other list in an application do go through the style, so
 * a popup that does not looks like a piece of a different application.  The
 * delegate below hands the row over the way the rows of a `QListView` are
 * handed over, and this function puts it in place of the one Qt installed.
 *
 * A delegate that the application put on the view itself is left alone.
 */
void polishComboBoxPopupView( QComboBox* combo );

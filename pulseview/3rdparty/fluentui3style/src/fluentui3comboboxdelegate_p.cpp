#include "fluentui3comboboxdelegate_p.h"

#include <QAbstractItemDelegate>
#include <QAbstractItemView>
#include <QComboBox>
#include <QModelIndex>
#include <QPainter>
#include <QStyle>
#include <QStyledItemDelegate>

namespace {
/*
 * Draws the rows of a combo box popup through the style.
 *
 * The separator rows are the one thing the delegate Qt installs handles
 * itself, and apps that use them expect them to be drawn and sized the way
 * they were, so that part is kept.  Every other row is handed to the style,
 * which draws it the way it draws the row of a list view.
 */
class ComboBoxPopupDelegate : public QStyledItemDelegate
{
public:
    explicit ComboBoxPopupDelegate( QObject* parent = nullptr ) : QStyledItemDelegate( parent ) {}

    static bool isSeparator( const QModelIndex& index )
    {
        // Qt marks a separator row of a combo box this way, and reads the
        // mark back the same way in the delegate it installs by itself.
        return index.data( Qt::AccessibleDescriptionRole ).toString() == QLatin1String( "separator" );
    }

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const override
    {
        if ( !isSeparator( index ) )
        {
            QStyledItemDelegate::paint( painter, option, index );
            return;
        }

        QRect rect = option.rect;
        if ( const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>( option.widget ) )
        {
            if ( view->viewport() )
            {
                rect.setWidth( view->viewport()->width() );
            }
        }

        QStyleOption separator;
        separator.rect  = rect;
        separator.state = option.state;
        if ( QStyle* style = option.widget ? option.widget->style() : nullptr )
        {
            style->drawPrimitive( QStyle::PE_IndicatorToolBarSeparator, &separator, painter, option.widget );
        }
    }

    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const override
    {
        if ( !isSeparator( index ) )
        {
            return QStyledItemDelegate::sizeHint( option, index );
        }

        QStyle* style   = option.widget ? option.widget->style() : nullptr;
        const int width = style ? style->pixelMetric( QStyle::PM_DefaultFrameWidth, nullptr, option.widget ) : 1;
        return QSize( width, width );
    }
};
}  // namespace

void polishComboBoxPopupView( QComboBox* combo )
{
    if ( !combo )
    {
        return;
    }

    QAbstractItemView* view = combo->view();
    if ( !view )
    {
        return;
    }

    /*
     * Only the delegate Qt installed is replaced.  A delegate that the
     * application put there is left alone, and one of ours is not put in
     * twice, because polish() runs again on every change of the style.
     */
    QAbstractItemDelegate* delegate = view->itemDelegate();
    if ( delegate && delegate->inherits( "QComboBoxDelegate" ) )
    {
        view->setItemDelegate( new ComboBoxPopupDelegate( view ) );
    }
}

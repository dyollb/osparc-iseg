#include <QtGui>

#include <QFont>
#include "qlogtable.h"

/*!
    \class QLogTable
    \brief The QLogTable class is a very basic widget
    that is used to display text in a table (like QTableWidget).
    
    - The widget can only use a proportionnal font
    - Scrollbars are automatically updated when text is added
    - Copy/paste/cur and selection are not implemented
    - The number of columns can't be changed after creation
*/

/*!
    Constructs an empty QLogTable with parent \a
    parent and \a numCols columns.
    
    The default font will be a 10pt Courier.
*/
QLogTable::QLogTable(int numCols,  QWidget * parent ) : QAbstractScrollArea(parent), numberOfColumns(numCols)
{
    curFont = QFont("Courier", 10, QFont::Normal);
    rowHeight = QFontMetrics(curFont).lineSpacing() + 3;

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    content = new QList< QList<QString> >;
    colWidth = new int[numberOfColumns];
    for (int i = 0; i < numberOfColumns; i++) { colWidth[i] = 0; }
    
    verticalScrollBar ()->setMaximum(content->size() * rowHeight);
    verticalScrollBar ()->setSingleStep(rowHeight);
    horizontalScrollBar ()->setSingleStep(QFontMetrics(curFont).maxWidth());
}

/*!
    Destructor.
*/
QLogTable::~QLogTable()
{
    delete[] colWidth;
}

/*!
    Remove all text in the table and clear the widget
*/
void QLogTable::clearBuffer() 
{
    content->clear();
    update();
}

/*!
    Set the font used by the widget
    This font must be a proportionnal font (like Courier)
*/ 
void QLogTable::setFont(QFont &f)
{
    if (f.styleHint() == QFont::TypeWriter || f.styleHint() == QFont::Courier) {
        curFont = f;
        rowHeight = QFontMetrics(curFont).lineSpacing() + 3;
        verticalScrollBar()->setSingleStep(rowHeight);
    }
}

/*!
    Append a row with \a str data
    \a str is a QStringList where each item is a cell of the row.
    
    So number of columns should equal to str.size()
*/ 
void QLogTable::appendRow(const QStringList &str)
{
    if (!str.isEmpty()) {
        (*content) << str;

        for (int i = 0; (i < numberOfColumns) && (i < str.size()); i++) {
            if (str[i].size() > colWidth[i]) colWidth[i] = str[i].size();
        }
        
        updateScrollbar();
    }
}

/*! \internal
    Update scrollbar values (current and maximum)
*/
void QLogTable::updateScrollbar()
{
    // Update the scrollbar
    int max = (content->size() * rowHeight) - viewport()->height();
    verticalScrollBar()->setMaximum(max < 0 ? 0 : max);

    int maxlinelength = 0;

    for (int i = 0; i < numberOfColumns; i++) {
        maxlinelength += colWidth[i] * QFontMetrics(curFont).maxWidth() + 5;
    }
    
    max = (maxlinelength) - viewport()->width();
    horizontalScrollBar()->setMaximum(max < 0 ? 0 : max);

    if (!verticalScrollBar()->isSliderDown() && !horizontalScrollBar()->isSliderDown()) {
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        update();
    }
}

/*! \internal
    All input event are ignored
*/
void QLogTable::keyPressEvent ( QKeyEvent * e )
{
    e->ignore();
}

/*! \internal
*/
void QLogTable::resizeEvent ( QResizeEvent * event )
{
    QPainter p(viewport());
}

/*! \internal
*/
void QLogTable::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());
    paint(&p, event);
}

/*! \internal
    To paint the widget.
    Only lines displayed by the widget are painted. The very low QTextLayout API is used
    to optimize the display.
*/
void QLogTable::paint (QPainter *p,  QPaintEvent * e)
{
    bool lineDrawn = false;
    const int xOffset = horizontalScrollBar ()->value();
    const int yOffset = verticalScrollBar ()->value();

    QPen textPen(Qt::black);
    QPen linesPen(Qt::gray);
    linesPen.setStyle(Qt::DotLine);

    QRect r = e->rect();
    p->translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);
    p->setClipRect(r);

    for (int i = (r.top() / rowHeight); i < (r.bottom() / rowHeight) + 1; i++) {
        if (i < content->size()) {
            int x = 0;
            for (int j = 0; j < numberOfColumns /*(*content)[i].size()*/; j++) {
                
                QTextLayout textLayout((*content)[i][j], curFont);
                textLayout.beginLayout();
                textLayout.createLine();
                textLayout.endLayout();

                p->setPen(textPen);
                textLayout.draw (p, QPointF(x, i * rowHeight) );

                x += colWidth[j] * QFontMetrics(curFont).maxWidth() + 5;

                if (!lineDrawn) { // Draw the vertical line but only once
                    p->setPen(linesPen);
                    p->drawLine(x - 3, r.top(), x - 3, (content->size() * rowHeight < r.bottom() ? content->size() * rowHeight - 2 : r.bottom()));
                }
                
            }
            lineDrawn = true;

            // Draw horizontal lines (normally x = right limit of the table)
            p->setPen(linesPen);
            p->drawLine(r.left(), (i + 1) * rowHeight - 2, ((x - 3) < r.right() ? (x - 3) : r.bottom()), (i + 1) * rowHeight - 2);
        }
    }
}

#include <QtGui>

#include <QFont>
#include "qlogtext.h"

/*!
    \class QLogText
    \brief The QLogText class is a very basic widget
    that is used to display only plain text (like a log).
    
    - The widget can only use a proportionnal font
    - Scrollbars are automatically updated when text is added
    - Copy/paste/cur and selection are not implemented

    This widget was tested with around 100,000 lines.

*/

/*!
    \enum QLogText::Limit
    Not yet implemented
*/

/*!
    \var QLogText::Limit QLogText::NoLimit
    Not yet implemented
*/

/*!
    Constructs an empty QTextEdit with parent \a
    parent.
    
    The default font will be a 10pt Courier and the text will be wrapped.
*/
QLogText::QLogText( QWidget * parent ) : QAbstractScrollArea(parent), wrapMode(true), numberOfLines(0), numberOfColumns(0), limit(NoLimit)
{
    curFont = QFont("Courier", 10, QFont::Normal);
    fontsize = QFontMetrics(curFont).lineSpacing();

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    if (!wrapMode)
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    else 
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    verticalScrollBar ()->setMaximum(content.size() * fontsize);
    verticalScrollBar ()->setSingleStep(fontsize);
}

/*!
    Destructor.
*/
QLogText::~QLogText()
{
}

/*!
    Sets the wrapping mode.
*/
void QLogText::setWrapMode(bool w)
{
    wrapMode = w;
    if (!wrapMode)
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    else 
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

/*!
    Set the content limit
    \todo Not yet implemented
*/
void QLogText::setLimit(int l)
{
    limit = l;
}

/*!
    Remove all text and clear the widget
*/
void QLogText::clearBuffer() 
{
    content.clear();
    updateNumberOfLines();
    numberOfColumns = 0;
    update();
}

/*!
    Set the font used by the widget
    This font must be a proportionnal font (like Courier)
*/ 
void QLogText::setFont(QFont &f)
{
    if (f.styleHint() == QFont::TypeWriter || f.styleHint() == QFont::Courier) {
        curFont = f;
        fontsize = QFontMetrics(curFont).lineSpacing();
        verticalScrollBar ()->setSingleStep(fontsize);
    }
}

/*!
    Append text to the widget.
    If \a str contains several lines, \a str is splitted.
    Scrollbars are updated.
*/
void QLogText::append(const QString &str)
{
    if (!str.isEmpty()) {
        QStringList subList;
        int previous_index;
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
    
        subList = str.split(QChar(0x000D));

        /*! \todo Real special case : manage backspace only if it's the first char
                  (which is usually the case) */
        if ((subList[0])[0].unicode() == 0x0008) { // Backspace
            removeLastChar(); 
            (subList[0]).remove(0, 1);
        }
        
        previous_index = content.size() == 0 ? 0 : content.size() - 1;
        if (content.size() == 0) {
            content << subList;
        } else {
            if (wrapMode) {
                numberOfLines -= (content[previous_index].size() / charPerLines) + 1;
            }
            content[content.size() - 1].append(subList.takeFirst());
            content << subList;
        }
    
        if (!wrapMode) {
            numberOfLines = content.size();
            for (int i = previous_index; i < content.size(); i++)
                if (content[i].size() > numberOfColumns) numberOfColumns = content[i].size();
        } else {
            for (int i = previous_index; i < content.size(); i++)
                numberOfLines += (content[i].size() / charPerLines) + 1;
        }
    
        updateScrollbar();
    }
}

/*!
    Append only one text line to the widget.
    Scrollbars are updated.
*/
void QLogText::appendLine(const QString &str)
{
    if (!str.isEmpty()) {
        //QStringList subList;
        int previous_index;
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
    
        previous_index = content.size() == 0 ? 0 : content.size() - 1;

        content << str;
    
        if (!wrapMode) {
            numberOfLines = content.size();
            if (str.size() > numberOfColumns) numberOfColumns = str.size();
        } else {
            numberOfLines += (str.size() / charPerLines) + 1;
        }
    
        updateScrollbar();
    
    }
}

/*!
    Remove the last char in the last line of the widget content.
    Not very useful in usual cases...
*/
void QLogText::removeLastChar()
{
    if (content.size() != 0) {
        content.last().remove ( content.last().size() - 1, 1 );
    }
}

/*! \internal
    Update scrollbar values (current and maximum)
*/
void QLogText::updateScrollbar()
{
    // Update the scrollbar
    int max = (numberOfLines * fontsize) - viewport()->height();
    verticalScrollBar ()->setMaximum(max < 0 ? 0 : max);
    if (!wrapMode) {
        max = (numberOfColumns * QFontMetrics(curFont).maxWidth()) - viewport()->width();
        horizontalScrollBar()->setMaximum(max < 0 ? 0 : max);
    }
    if (!verticalScrollBar()->isSliderDown() && !horizontalScrollBar()->isSliderDown()) {
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
        if (!wrapMode) {
            int val = content[content.size() - 1].size() * QFontMetrics(curFont).maxWidth() - viewport()->width();
            horizontalScrollBar()->setValue(val < 0 ? 0 : val);
        }
        update();
    }
}

/*! \internal
    Update the number of lines and update scrollbars
*/
void QLogText::updateNumberOfLines()
{
    if (!wrapMode) {
        numberOfLines = content.size();
    } else {
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
        numberOfLines = 0;
        for (int i = 0; i < content.size(); ++i) {
            numberOfLines += (content[i].size() / charPerLines) + 1;
        }
    }
    
    int max = (numberOfLines * fontsize) - viewport()->height();
    verticalScrollBar ()->setMaximum(max < 0 ? 0 : max);
    
}

/*! \internal
    All input event are ignored
*/
void QLogText::keyPressEvent ( QKeyEvent * e )
{
    e->ignore();
}

/*! \internal
*/
void QLogText::resizeEvent ( QResizeEvent * event )
{
    QPainter p(viewport());

    updateNumberOfLines();
}

/*! \internal
*/
void QLogText::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());
    paint(&p, event);
}

/*! \internal
    To paint the widget.
    Only lines displayed by the widget are painted. The very low QTextLayout API is used
    to optimize the display.
*/
void QLogText::paint (QPainter *p,  QPaintEvent * e)
{
    const int xOffset = horizontalScrollBar ()->value();
    const int yOffset = verticalScrollBar ()->value();

    QRect r = e->rect();
    p->translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);
    p->setClipRect(r);

    p->setPen(Qt::black);
    
    if (!wrapMode) {
        for (int i = (r.top() / fontsize); i < (r.bottom() / fontsize) + 1; i++) {
            if (i < content.size()) {

                QTextLayout textLayout(content[i], curFont);
                textLayout.beginLayout();
                textLayout.createLine();
                textLayout.endLayout();

                textLayout.draw (p, QPointF(0, i * fontsize) );
            }
        }
    } else {
        int charPerLines = viewport()->width() / QFontMetrics(curFont).maxWidth();
        int i = 0, nbOfLines = 0, beginline = (r.top() / fontsize), endline = (r.bottom() / fontsize) + 1;
        // Search for the first line to display
        while (nbOfLines < beginline && i < content.size()) {
            nbOfLines += (content[i].size() / charPerLines) + 1;
            ++i;
        }
        
        if (i) {
            --i;
            nbOfLines -= (content[i].size() / charPerLines) + 1;
        }
        
        // Repaint the part of the screen
        while (nbOfLines < endline && i < content.size()) {
            int n = (content[i].size() / charPerLines) + 1;

            for (int j = 0; j < n; j++) {
                if ((nbOfLines + j >= beginline) && (nbOfLines + j + 1 <= endline)) {

                    QTextLayout textLayout(content[i].mid(j * charPerLines, charPerLines), curFont);
                    textLayout.beginLayout();
                    textLayout.createLine();
                    textLayout.endLayout();

                    textLayout.draw (p, QPointF(0, (nbOfLines + j /*! + 1 : \todo To check*/) * fontsize) );
                }
            }
            nbOfLines += n;
            
            ++i;
        }
    }

}

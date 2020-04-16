#ifndef QLOGTABLE_H
#define QLOGTABLE_H

#include <QAbstractScrollArea>

class QLogTable : public QAbstractScrollArea
{
    Q_OBJECT

    public:
        QLogTable (int numCols, QWidget * parent = 0 );
        ~QLogTable();

        void setFont(QFont &);
    
    protected:
        virtual void paintEvent ( QPaintEvent * event );
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void QLogTable::keyPressEvent ( QKeyEvent * e );
        virtual void paint (QPainter *p,  QPaintEvent * e );
        
        void updateScrollbar();
    
    public slots:
        void clearBuffer();
        void appendRow(const QStringList &);
        
    protected:
        QList< QList<QString> > *content;  // widget data
        int *colWidth; // columns width
        QFont curFont; // current font
        int rowHeight; // column height get from curFont (internally used)
        int numberOfLines; // number of lines/rows
        int numberOfColumns; // number of columns
};

#endif // QLOGTEXT_H

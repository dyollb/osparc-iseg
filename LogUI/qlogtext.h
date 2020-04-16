#ifndef QLOGTEXT_H
#define QLOGTEXT_H

#include <QAbstractScrollArea>

class QLogText : public QAbstractScrollArea
{
    Q_OBJECT

    enum Limit {
        NoLimit = -1
    };

    public:
        QLogText ( QWidget * parent = 0 );
        ~QLogText();

        void setFont(QFont &);
        void setLimit(int l);
        void setWrapMode(bool w);
        void removeLastChar();
    
    protected:
        virtual void paintEvent ( QPaintEvent * event );
        virtual void resizeEvent ( QResizeEvent * event );
        virtual void keyPressEvent ( QKeyEvent * e );

        virtual void paint (QPainter *p,  QPaintEvent * e );
        
        void updateNumberOfLines();
        void updateScrollbar();
    
    public slots:
        void clearBuffer();
        void append(const QString &);
        void appendLine(const QString &);
        
    protected:
        QStringList content; // widget data
        QFont curFont; // font currently used by the widget
        int fontsize; // font size get from curFont (internally used)
        bool wrapMode; // wrap or not
        int numberOfLines; // number of lines (computed from content)
        int numberOfColumns; // number of columns (computed from content)
        int limit; // limit for number of lines (not implemented)
};

#endif // QLOGTEXT_H

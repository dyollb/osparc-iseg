#include <QtGui>
 

class CustomGraphicsView : public QGraphicsView
{
public:
    CustomGraphicsView(QGraphicsScene* scene, QWidget* parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent *event);

private:
    const double scaleFactor = 1.2;
};

CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene, QWidget *parent) 
	: QGraphicsView(scene, parent)
{

}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    if (event->delta() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1/scaleFactor, 1/scaleFactor);
    }
}

int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	CustomGraphicsView view(new QGraphicsScene );
	view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view.scene()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	view.show();
	return app.exec(); 
}
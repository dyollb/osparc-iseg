#include <QtGui>
 
#include <iostream>

class CustomGraphicsView : public QGraphicsView
{
public:
    CustomGraphicsView(QGraphicsScene* scene, QWidget* parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual bool event(QEvent *event)
	{
		if (event->type() == QEvent::Gesture)
			return gestureEvent(static_cast<QGestureEvent*>(event));
		return QWidget::event(event);
	}
	bool gestureEvent(QGestureEvent *event);

private:
    const double scaleFactor = 1.2;

	bool _pan = false;
	int _panStartX,_panStartY;
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

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		_pan = true;
		_panStartX = event->x();
		_panStartY = event->y();
		setCursor(Qt::ClosedHandCursor);
		event->accept();
		return;
	}
	event->ignore(); //?
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		_pan = false;
		setCursor(Qt::ArrowCursor);
		event->accept();
		return;
	}
	event->ignore(); //?
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (_pan)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
		verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
		_panStartX = event->x();
		_panStartY = event->y();
		event->accept();
		return;
	}
	event->ignore(); //?
}

bool CustomGraphicsView::gestureEvent(QGestureEvent *event)
{
	if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
		std::cerr << "swipe\n"; //swipeTriggered(static_cast<QSwipeGesture *>(swipe));
	else if (QGesture *pan = event->gesture(Qt::PanGesture))
		std::cerr << "pan\n"; //panTriggered(static_cast<QPanGesture *>(pan));
	if (QGesture *pinch = event->gesture(Qt::PinchGesture))
		std::cerr << "pinch\n"; //pinchTriggered(static_cast<QPinchGesture *>(pinch));
	return true;
}

int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	CustomGraphicsView view(new QGraphicsScene );
	view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//view.scene()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	view.scene()->addPixmap(QPixmap("E:/VKF/0606.png"));
	view.show();
	return app.exec(); 
}
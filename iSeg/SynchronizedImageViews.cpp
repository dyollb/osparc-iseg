#include "SynchronizedImageViews.h"


CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CustomGraphicsView::setScale(QTransform tr)
{
	setTransform(tr);
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
	auto p0scene = mapToScene(event->pos());

	//setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	if (event->delta() > 0) {
		//zoom = transform().m11() * scaleFactor;
		scale(scaleFactor, scaleFactor);
	}
	else {
		//zoom = transform().m11() / scaleFactor;
		scale(1 / scaleFactor, 1 / scaleFactor);
	}

	auto p1mouse = mapFromScene(p0scene);
	auto move = p1mouse - event->pos(); // The move

	//scaleChanged(transform()); // \todo set scrollbar values via signal?

	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
	//event->accept();
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
	//QGraphicsView::mousePressEvent(event);
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
	//QGraphicsView::mouseReleaseEvent(event);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (_pan)
	{
		// \todo set and trigger signal for external viewer to synchronize ?
		// --> happens below in Widget
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
		verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
		_panStartX = event->x();
		_panStartY = event->y();
		event->accept();
		return;
	}
	event->ignore(); //?
	//QGraphicsView::mouseMoveEvent(event);
}

Widget::Widget(QWidget *parent /*= 0*/) : QWidget(parent)
{
	view1 = new CustomGraphicsView(new QGraphicsScene);
	view2 = new CustomGraphicsView(new QGraphicsScene);

	QVBoxLayout *vertical = new QVBoxLayout(this);
	QWidget *subWidget = new QWidget(this);
	QHBoxLayout *layout = new QHBoxLayout(subWidget);
	layout->addWidget(view1);
	layout->addWidget(view2);

	vertical->addWidget(subWidget);
	auto hbar2 = view2->horizontalScrollBar();
	auto vbar2 = view2->verticalScrollBar();
	hbar2->setRange(view1->horizontalScrollBar()->minimum(), view1->horizontalScrollBar()->maximum());
	vbar2->setRange(view1->verticalScrollBar()->minimum(), view1->verticalScrollBar()->maximum());

	connect(view1->horizontalScrollBar(), SIGNAL(valueChanged(int)), view2->horizontalScrollBar(), SLOT(setValue(int)));
	connect(view2->horizontalScrollBar(), SIGNAL(valueChanged(int)), view1->horizontalScrollBar(), SLOT(setValue(int)));

	connect(view1->verticalScrollBar(), SIGNAL(valueChanged(int)), view2->verticalScrollBar(), SLOT(setValue(int)));
	connect(view2->verticalScrollBar(), SIGNAL(valueChanged(int)), view1->verticalScrollBar(), SLOT(setValue(int)));

	connect(view1->horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), view2->horizontalScrollBar(), SLOT(setRange(int, int)));

	// \bug this signal-slot connection seems to break zooming -> anchor ?
	//connect(view1, SIGNAL(scaleChanged(QTransform)), view2, SLOT(setScale(QTransform)));
	//connect(view2, SIGNAL(scaleChanged(QTransform)), view1, SLOT(setScale(QTransform)));
}

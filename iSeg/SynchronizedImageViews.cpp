#include "SynchronizedImageViews.h"


CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
	//setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setMouseTracking(true);
}

void CustomGraphicsView::setScale(double s)
{
	scale(s, s);
}

void CustomGraphicsView::setMove(QPoint move)
{
	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
	auto p0scene = mapToScene(event->pos());

	//auto oldAnchor = transformationAnchor();
	//setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	double s = (event->delta() > 0) ? scaleFactor : 1/scaleFactor;
	scale(s, s);

	auto p1mouse = mapFromScene(p0scene);
	auto move = p1mouse - event->pos(); // The move

	scaleChanged(s);

	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
	moveChanged(move);

	//event->accept();

	//setTransformationAnchor(oldAnchor);
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		_pan = true;
		_panStart = event->pos();
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
		auto move = _panStart - event->pos(); // The move
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + move.x());
		verticalScrollBar()->setValue(verticalScrollBar()->value() + move.y());
		moveChanged(move);
		_panStart = event->pos();
		event->accept();
		return;
	}
	event->ignore(); //?
	//QGraphicsView::mouseMoveEvent(event);
}

SynchronizedImageViews::SynchronizedImageViews(QWidget *parent /*= 0*/) : QWidget(parent)
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

	connect(view1, SIGNAL(scaleChanged(double)), view2, SLOT(setScale(double)));
	connect(view2, SIGNAL(scaleChanged(double)), view1, SLOT(setScale(double)));

	connect(view1, SIGNAL(moveChanged(QPoint)), view2, SLOT(setMove(QPoint)));
	connect(view2, SIGNAL(moveChanged(QPoint)), view1, SLOT(setMove(QPoint)));

	//connect(view1->horizontalScrollBar(), SIGNAL(valueChanged(int)), view2->horizontalScrollBar(), SLOT(setValue(int)));
	//connect(view2->horizontalScrollBar(), SIGNAL(valueChanged(int)), view1->horizontalScrollBar(), SLOT(setValue(int)));

	//connect(view1->verticalScrollBar(), SIGNAL(valueChanged(int)), view2->verticalScrollBar(), SLOT(setValue(int)));
	//connect(view2->verticalScrollBar(), SIGNAL(valueChanged(int)), view1->verticalScrollBar(), SLOT(setValue(int)));

	connect(view1->horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), view2->horizontalScrollBar(), SLOT(setRange(int, int)));
}

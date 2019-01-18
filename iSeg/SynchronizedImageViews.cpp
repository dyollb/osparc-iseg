#include "SynchronizedImageViews.h"

#include <cmath>

CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setMouseTracking(true);
}

void CustomGraphicsView::scale_by_factor(float s)
{
	scale(s, s);
}

void CustomGraphicsView::set_shift(QPoint move)
{
	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
	auto p0scene = mapToScene(event->pos());

	float s = (event->delta() > 0) ? scaleFactor : 1/scaleFactor;
	//float s = std::pow(scaleFactor, event->delta() / 120.f);
	scale(s, s);

	auto p1mouse = mapFromScene(p0scene);
	auto move = p1mouse - event->pos(); // The move

	// trigger signal for external viewer to synchronize
	scale_factor_changed(s);

	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
	// trigger signal for external viewer to synchronize
	shift_changed(move);

	event->accept(); //?
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
		auto move = _panStart - event->pos(); // The move
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + move.x());
		verticalScrollBar()->setValue(verticalScrollBar()->value() + move.y());

		// trigger signal for external viewer to synchronize
		emit shift_changed(move);
		_panStart = event->pos();
		event->accept();
		return;
	}
	event->ignore(); //?
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

	connect(view1, SIGNAL(scale_factor_changed(float)), view2, SLOT(scale_by_factor(float)));
	connect(view2, SIGNAL(scale_factor_changed(float)), view1, SLOT(scale_by_factor(float)));

	connect(view1, SIGNAL(shift_changed(QPoint)), view2, SLOT(set_shift(QPoint)));
	connect(view2, SIGNAL(shift_changed(QPoint)), view1, SLOT(set_shift(QPoint)));

	//connect(view1->horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), view2->horizontalScrollBar(), SLOT(setRange(int, int)));
}

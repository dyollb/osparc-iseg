#include "SynchronizedImageViews.h"

#include <cmath>

CustomGraphicsView::CustomGraphicsView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	setMouseTracking(true);
}

void CustomGraphicsView::OnZoomChanged(float s, QPoint move)
{
	scale(s, s);
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

	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());

	// trigger signal for external viewer to synchronize
	emit ZoomChanged(s, move);

	event->accept(); //?
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		m_Pan = true;
		m_PanStart = event->pos();
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
		m_Pan = false;
		setCursor(Qt::ArrowCursor);
		event->accept();
		return;
	}
	event->ignore(); //?
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_Pan)
	{
		auto move = m_PanStart - event->pos(); // The move
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + move.x());
		verticalScrollBar()->setValue(verticalScrollBar()->value() + move.y());

		// trigger signal for external viewer to synchronize
		emit ZoomChanged(1.f, move);
		m_PanStart = event->pos();
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

	connect(view1, SIGNAL(ZoomChanged(float,QPoint)), view2, SLOT(OnZoomChanged(float,QPoint)));
	connect(view2, SIGNAL(ZoomChanged(float,QPoint)), view1, SLOT(OnZoomChanged(float,QPoint)));

	connect(view1->horizontalScrollBar(), SIGNAL(sliderMoved(int)), view2->horizontalScrollBar(), SLOT(setValue(int)));
	connect(view1->verticalScrollBar(), SIGNAL(sliderMoved(int)), view2->verticalScrollBar(), SLOT(setValue(int)));
	connect(view2->horizontalScrollBar(), SIGNAL(sliderMoved(int)), view1->horizontalScrollBar(), SLOT(setValue(int)));
	connect(view2->verticalScrollBar(), SIGNAL(sliderMoved(int)), view1->verticalScrollBar(), SLOT(setValue(int)));
}

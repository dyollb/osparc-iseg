#pragma once

#include <QtGui>

#include <iostream>

class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	CustomGraphicsView(QGraphicsScene* scene, QWidget* parent = 0);

	const float scaleFactor = 1.2f;

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void OnZoomChanged(float s, QPoint move);
	void ZoomIn(float factor=1.2f) { centerOn(viewport()->rect().center()); scale(factor, factor); }
	void ZoomOut(float factor=1.2f) { centerOn(viewport()->rect().center()); scale(1.f/factor, 1.f/factor); }

signals:
	void ZoomChanged(float s, QPoint m);

private:
	bool m_Pan = false;
	QPoint m_PanStart;
};

class SynchronizedImageViews : public QWidget
{
public:
	SynchronizedImageViews(QWidget *parent = 0);

	QGraphicsScene* scene1() { return view1->scene(); }
	QGraphicsScene* scene2() { return view2->scene(); }

private:
	QGraphicsView *view1;
	QGraphicsView *view2;
};

#pragma once

#include <QtGui>

#include <iostream>

class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	CustomGraphicsView(QGraphicsScene* scene, QWidget* parent = 0);

	void zoomIn() { centerOn(viewport()->rect().center()); scale(scaleFactor, scaleFactor); }
	void zoomOut() { centerOn(viewport()->rect().center()); scale(1.f/scaleFactor, 1.f/scaleFactor); }

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void setScale(float s);
	void setMove(QPoint p);

signals:
	void scaleChanged(float s);
	void moveChanged(QPoint p);

private:
	//double zoom = 1.0;

	const float scaleFactor = 1.2f;

	bool _pan = false;
	QPoint _panStart;
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

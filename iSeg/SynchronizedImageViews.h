#pragma once

#include <QtGui>

#include <iostream>

class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	CustomGraphicsView(QGraphicsScene* scene, QWidget* parent = 0);

	const float scaleFactor = 1.2f;

	void zoom_in() { centerOn(viewport()->rect().center()); scale(scaleFactor, scaleFactor); }
	void zoom_out() { centerOn(viewport()->rect().center()); scale(1.f/scaleFactor, 1.f/scaleFactor); }

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void scale_by_factor(float s);
	void set_shift(QPoint p);

signals:
	void scale_factor_changed(float s);
	void shift_changed(QPoint p);

private:
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

#pragma once

#include <QtGui>

#include <iostream>

class CustomGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	CustomGraphicsView(QGraphicsScene* scene, QWidget* parent = 0);

protected:
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void setScale(QTransform tr);

signals:
	void scaleChanged(QTransform tr);

private:
	//double zoom = 1.0;

	const double scaleFactor = 1.2;

	bool _pan = false;
	int _panStartX, _panStartY;
};

class Widget : public QWidget
{
public:
	Widget(QWidget *parent = 0);

	QGraphicsScene* scene1() { return view1->scene(); }
	QGraphicsScene* scene2() { return view2->scene(); }

private:
	QGraphicsView *view1;
	QGraphicsView *view2;
};

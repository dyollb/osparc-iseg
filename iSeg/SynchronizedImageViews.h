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

class ImageViewer : public QWidget
{
	Q_OBJECT
public:
	ImageViewer(QWidget* parent = 0);

	QImage _image;

protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;

	QPoint mapToScene(QPoint x);
	QPoint mapFromScene(QPoint x);

private:
	QPoint _anchor;
	QPoint _shift;
	double _scale = 1.0;

	bool _pan = false;
	int _panStartX, _panStartY;
};
#include "SynchronizedImageViews.h"


int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	//SynchronizedImageViews view;
	////view.scene()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	//view.scene1()->addPixmap(QPixmap("E:/VKF/0606.png"));
	//view.scene2()->addPixmap(QPixmap("E:/VKF/0795.png"));
	//view.show();

	ImageViewer view;
	view._image.load("E:/VKF/0606.png");
	view.show();

	return app.exec(); 
}

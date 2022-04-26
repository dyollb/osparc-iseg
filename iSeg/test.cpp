#include "SynchronizedImageViews.h"


int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	SynchronizedImageViews view;
	//view.scene1()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	//view.scene2()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	view.scene1()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	view.scene2()->addPixmap(QPixmap("/Users/lloyd/bryn-lloyd.png"));
	view.show();
	view.raise();

	return app.exec(); 
}
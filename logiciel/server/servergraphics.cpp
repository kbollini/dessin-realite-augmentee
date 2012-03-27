#include "servergraphics.hpp"

ServerGraphics::ServerGraphics()
{
	setFixedSize(640, 480);
	setSceneRect(0, 0, 640, 480);
	
	sceneTab = new QGraphicsScene();
	setScene(sceneTab);
}

void ServerGraphics::addPoint(QPoint p)
{
	sceneTab->addEllipse(p.x(), p.y(), 5, 5, QPen(), QBrush(Qt::SolidPattern));
}

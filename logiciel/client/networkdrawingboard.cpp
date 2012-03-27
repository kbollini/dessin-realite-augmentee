#include "networkdrawingboard.hpp"

NetworkDrawingBoard::NetworkDrawingBoard(QString h, int p)
{
	socket = new QTcpSocket(this);
	host = h;
	port = p;
	
	setFixedSize(640, 480);
	setSceneRect(0, 0, 640, 480);
	scene = new QGraphicsScene();
	setScene(scene);
	
	// Connexion au serveur
	socket->abort();
	socket->connectToHost(host, port);
	
	// Signal levé lorsque la connexion est active
	connect(socket, SIGNAL(connected()), this, SLOT(connectionActive()));
	
	// Signal levé lors de la réception de données
	connect(socket, SIGNAL(readyRead()), this, SLOT(dataIncoming()));
	
	// Déconnexion
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}		

void NetworkDrawingBoard::drawPoint(int x, int y)
{
	// Envoie l'odre de dessin au serveur
}		
		
void NetworkDrawingBoard::drawQPoint(QPoint)
{}

void NetworkDrawingBoard::drawLine(int fromX, int fromY, int toX, int toY)
{}

void NetworkDrawingBoard::connectionActive()
{
	qDebug() << "Connexion active";
}

void NetworkDrawingBoard::dataIncoming()
{
	qDebug() << "Réception de données";
	
	// Le QDataStream est lié à la socket
	QDataStream stream(socket);
	
	// On regarde le message
	QString command;
	stream >> command;
	
	if(command == "item")
	{	
		QString type;
		stream >> type;
		if(type == "qpixmap")
		{
			// Synchronisation de la scène avec le serveur
			QPixmap pixmap;
			
			stream >> pixmap;
			pixmap.save("image.png");
			
			// On mets le pixmap dans la scène locale
			scene->addPixmap(pixmap);
		}
	}
}

void NetworkDrawingBoard::disconnected()
{
	qDebug() << "Déconnexion du serveur";
}



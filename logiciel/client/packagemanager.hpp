#ifndef HPP_CLIENTPACKAGEMANAGER
#define HPP_CLIENTPACKAGEMANAGER

/*
 * 	Classe gérant les paquets envoyés par le serveur, le message est 
 * 	identifié en amont, et le PackageManager agit suivant la commande
 */

using namespace std;

#include <string>
#include <QDataStream>
#include <QGraphicsScene>
#include <QPen>
#include <QPoint>
#include <QPen>
#include <iostream>
#include <string>
#include "drawingboard.hpp"

class PackageManager
{
	public :
		// Envoi d'un ordre de dessin d'une ligne au serveur
		static QString sendLine(QDataStream &stream, QLine line, QPen pen, QString old);
		
		// Envoi d'un ordre de dessin au serveur
		static QString sendPoint(QDataStream &stream, QPoint point, QPen pen, QString old);

		// Envoie une demande de vidage de scène
		static void flushScene(QDataStream &stream);
	
		// Reçoit un objet et le traite en conséquence
		static void item(QDataStream &stream, QGraphicsScene* scene);
		
		// Reçoit un ordre du serveur
		static void order(QStringList list, QGraphicsScene* scene, DrawingBoard* view);
};

#endif

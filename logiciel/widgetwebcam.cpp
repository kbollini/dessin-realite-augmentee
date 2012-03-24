#include "widgetwebcam.hpp"

WidgetWebcam::WidgetWebcam()
{
	this->setPixmap(pixmapCourante);
	
	// De base, la classe n'est pas prête à l'étalonnage
	readyToCalibrate = false;
	calibrationIsDone = false;
}

WidgetWebcam::WidgetWebcam(QString text) : QLabel(text)
{
	// On appelle le constructeur de QLabel pour afficher le texte
	// Puis on appelle le constructeur par défaut
	WidgetWebcam();
}

void WidgetWebcam::calibrate(IplImage* iImage)
{
	// Sauvegarde de l'iplImage pour initialiser
	imageInit = cvCloneImage(iImage);
	cvFlip(iImage,imageInit,1);
	// Conversion
	QImage qImage = iplToQimage(imageInit);

	// Affichage
	this->setPixmap(QPixmap::fromImage(qImage));
	this->show();
	
	// Etalonnage maintenant possible
	readyToCalibrate = true;
}

bool WidgetWebcam::calibrationDone() { return calibrationIsDone; }

void WidgetWebcam::mousePressEvent(QMouseEvent * event)
{
	/*
	// Si la classe est prête à recevoir le clic d'étalonnage
	if (readyToCalibrate && !calibrationIsDone)
	{
		// Initialisation du tracking
		cursor = initNaiveColorTrack(imageInit, event->x(), event->y());
		calibrationIsDone = true;
	}
	*/
}

QImage WidgetWebcam::iplToQimage(IplImage* image)
{
	int h = image->height;
	int w = image->width;
	int channels = image->nChannels;
	QImage qimg(w, h, QImage::Format_ARGB32);
	char *data = image->imageData;
	
	for (int y = 0; y < h; y++, data += image->widthStep)
	{
		for (int x = 0; x < w; x++)
		{
			char r = 0;
			char g = 0;
			char b = 0;
			char a = 0;
			if (channels == 1)
			{
				r = data[x * channels];
				g = data[x * channels];
				b = data[x * channels];
			}
			else if (channels == 3 || channels == 4)
			{
				r = data[x * channels + 2];
				g = data[x * channels + 1];
				b = data[x * channels];
			}

			if (channels == 4)
			{
				a = data[x * channels + 3];
				qimg.setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				qimg.setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
	return qimg;
}

QPoint WidgetWebcam::newImageFromWebcam(IplImage* img)
{
	/*
	if (calibrationIsDone)
	{
		// Flip de l'image pour améliorer l'IHM
		IplImage *mirrorImage = cvCloneImage(img);
		cvFlip(mirrorImage, mirrorImage,1);
		
		// Affichage de l'image sur le widget Qt
		IplImage * imageBinarise = binarisation(mirrorImage,&cursor);
		QImage qim = iplToQimage(imageBinarise);
		this->setPixmap(QPixmap::fromImage(qim));
		
		// Appel de la librairie pour le tracking
		naiveColorTrack(mirrorImage, &cursor);
		
		// Libération
		cvReleaseImage(&mirrorImage);
		cvReleaseImage(&imageBinarise);
		
		return QPoint(cursor.coord.x, cursor.coord.y);
	}
	return QPoint();
	*/
}




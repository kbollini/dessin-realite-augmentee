#include "libtrack.hpp"
using namespace std;

// Retourne l'image binarisée de 'source' en fonction des informations contenues dans le 'oldCursor' (coord et coul)
IplImage * binarisation(IplImage * source, Cursor *oldPix)
{
	IplImage *hsv;
	hsv = cvCloneImage(source);
	cvCvtColor(source, hsv, CV_BGR2HSV); // on travaille sur l'image en hsv => permet d'ignorer la luminosité
	CvScalar pixel =  oldPix->color; // permet de récupérer la couleur à traquer.
	
	int h = (int)pixel.val[0];
	int s = (int)pixel.val[1];
	int v = (int)pixel.val[2];
	int tolerance = 14; // à passer en paramètre d'ajustement??
	
	IplImage *mask = NULL;
	mask = cvCreateImage(cvGetSize(source), source->depth, 1);
	cvInRangeS(hsv, cvScalar(h - tolerance -1, s - tolerance, 0,0), cvScalar(h + tolerance -1, s + tolerance, 255,0), mask);
	
	//Il convient ensuite d'appliquer une ouverture (dilatation puis érosion) à notre image, 
	//afin d'éliminer les zones non pertinentes tout en améliorant la perception de l'objet
	IplConvKernel *structurant;
/*	structurants possibles : 
	-CV_SHAPE_RECT
    	-CV_SHAPE_CROSS
    	-CV_SHAPE_ELLIPSE
   	-CV_SHAPE_CUSTOM ==> int* à passer dans le paramètre value (dernier param) 
*/    
	structurant = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, NULL); // la le 5,5 représente le structurant (kernel) utilisé pour l'ouverture
	cvErode(mask, mask, structurant, 1);
	structurant = cvCreateStructuringElementEx(4, 4, 1, 1, CV_SHAPE_ELLIPSE, NULL);
	cvDilate(mask, mask, structurant, 1);
	
	//cvShowImage("binarisation", mask);
	//cvWaitKey (0);
	cvReleaseImage(&hsv);
	//cvReleaseImage(&source);
	return mask;
}
/**
 * A partir d'une image binaire, calcule et retourne sous forme d'un CvPoint* le barycentre des pixels à 1 (blancs),
 *  cencés représenter l'objet à traquer (aux erreurs de traitement près).
 */
CvPoint * getNewCoord(const IplImage* imgBin, Cursor * oldPix)
{
	if(imgBin->nChannels != 1 )
	{
		perror("Invalid IplImage number of channels.");
		assert(imgBin->nChannels != 1);
	}
	
	int sommeX = 0;
	int sommeY = 0;
	int nbPixels = 0;
	for(int x = 0; x < imgBin->width; x++)
		for(int y = 0; y < imgBin->height; y++)
			if(((uchar *)(imgBin->imageData + y*imgBin->widthStep))[x] == 255)
			{
				sommeX += x;
				sommeY += y;
				nbPixels++;
			}
			
	CvPoint * bary;
	bary->x = (int)(sommeX / nbPixels);
	bary->y = (int)(sommeY / nbPixels);
	return bary;
}

/*Met à jour le Cursor représentant le barycentre du curseur à tracker.
 * 0 en cas de succès
 * -1 en cas d'erreur : à définir
 */
int setNewCoord(const IplImage* imgBin, Cursor * oldPix)
{
	if(imgBin->nChannels != 1 )
	{
		perror("Invalid IplImage number of channels.");
		return -1;
	}
	
	int sommeX = 0;
	int sommeY = 0;
	int nbPixels = 0;
	for(int x = 0; x < imgBin->width; x++)
		for(int y = 0; y < imgBin->height; y++)
			if(((uchar *)(imgBin->imageData + y*imgBin->widthStep))[x] == 255)
			{
				sommeX += x;
				sommeY += y;
				nbPixels++;
			}
	if (nbPixels > 0)
	{
		oldPix->coord.x = (int)(sommeX / nbPixels);
		oldPix->coord.y = (int)(sommeY / nbPixels);
	}
	
	return 0;
}

/*
 *	getNewCoord(image binaire,ancienne position) retourne nouvelle position.
 * Méthode envisagée : chercher la zone correspondant à l'objet traquée la plus proche du pixel donnée
 * => méthode non précise
 * \TODO
 */
/* void setNewCoord(const IplImage* binaryImg, Cursor * oldPix)
{
	Cursor * c = oldPix;
	
	//Vérif que l'image donnée soit binarisée
	assert (binaryImg->nChannels == 1 );
	//Quoiquonveut? trouver le centre de la zone blanche la plus proche de oldPix->points
	//Commentkonfé?
	// on fait la zone la plus proche? ca implique de déterminer des zones... <= tu m'avais parlé de composantes connexe?
	
	return c;
}*/


Cursor initNaiveColorTrack(IplImage * source, int x, int y)
{
	Cursor pixel;
	CvPoint points;
	points.x = x;
	points.y = y;
	IplImage * hsv;
	
	hsv = cvCloneImage(source);
	cvCvtColor(source, hsv, CV_BGR2HSV);
	
	CvScalar color = cvGet2D(hsv,y,x);
	cvReleaseImage(&hsv);
	pixel.coord = points;
	pixel.color = color;
	naiveColorTrack(source,&pixel); 
	return pixel;
}

// 
int naiveColorTrack(IplImage * source, Cursor * clickedPix)
{

	return setNewCoord(binarisation(source, clickedPix), clickedPix);

}


//Initialise la structure de suivi par matching de forme&couleur
IplImage initNaiveShapeTrack(IplImage * source, CvPoint a, CvPoint b)
{

}


//Met à jour la structure de suivi par forme.
int naiveShapeTrack(IplImage * source, IplImage * cursor)
{
	IplImage * result; // If image is W * H and templ is w * h then result must be (W-w+1)* (H-h+1) 
	// Allocate Output Images:
	int iwidth = source->width - cursor->width + 1;
	int iheight = source->height - cursor->height + 1;
	result= cvCreateImage( cvSize( iwidth, iheight ), 32, 1 );
	cvMatchTemplate(source, cursor ,result, /*CV_TM_CCORR*/CV_TM_CCORR_NORMED);
	cvNormalize( result, result, 1, 0, CV_MINMAX );
	cvShowImage("result", result); 
	cvWaitKey(0);
	
	double minVal, maxVal;
	CvPoint maxLoc, minLoc;
	cvMinMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, NULL); 
	cvReleaseImage(&result);
	int x = maxLoc.x ;
	int y= maxLoc.y ;
	
	//recentrage
	x += (int)(cursor->width /2);
	y += (int)(cursor->height /2);
	
	cout << x << "--" << y << endl;
	return 0;
}


//Initialise la structure de suivi malin
IplImage initSmartTrack(IplImage * source, CvPoint a, CvPoint b)
{
	
}

//Met à jour la structure de suivi malin. //param a définir
int smartSmartTrack(IplImage * source, IplImage * cursor)
{

}

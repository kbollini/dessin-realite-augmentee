#include "client.hpp"

Client::Client() : ui(new Ui::Client)
{
	// Construction de l'interface
	ui->setupUi(this);
	mdiArea = new QMdiArea();
	this->setCentralWidget(mdiArea);
	
	// Instanciation des classes utiles
	camManager = new WebcamManager();
	camWidget = new WidgetWebcam();
	
	// Listage des webcams
	actionGroup = new QActionGroup(ui->menuWebcam);
	actionGroup->setExclusive(true);
	
	int nbCams = camManager->getNumberOfWebcams();
	int i=0;
	for (i=0; i<nbCams; i++)
	{
		// Ajout au menu
		QAction *action = new QAction("webcam" + QString::number(i+1), ui->menuWebcam);
		action->setCheckable(true);
		actionGroup->addAction(action);
		
		// Si c'est la première webcam, on l'active par défaut
		if (i == 0) {action->setChecked(true);}
		
		ui->menuWebcam->addAction(action);
	}
	connect(ui->actionD_marrer,SIGNAL(triggered()),this,SLOT(slotStart()));
}

void Client::calibration()
{
	// Temps avant de prendre une image pour l'étalonnage
	calibrationCounter = 5;
	
	// Affichage du compteur
	camWidget = new WidgetWebcam("Etalonnage dans : <br/><span style=\"font-size:100px;\">"+QString::number(calibrationCounter)+"</span>");
	calibrationCounter--;
	// Construction de l'interface
	camWidget->setAlignment(Qt::AlignCenter); camWidget->setFixedSize(640, 480);
	mdiArea->addSubWindow(camWidget);
	camWidget->show();
	
	// Démarrage du timer
	timerCounter = new QTimer(this);
	connect(timerCounter, SIGNAL(timeout()), this, SLOT(slotCounterChange()));
	timerCounter->start(1000);
}

void Client::slotCounterChange()
{
	// Si le compteur est > 0, on met à jour l'affichage
	if (calibrationCounter > 0)
	{
		camWidget->setText("Etalonnage dans : <br/><span style=\"font-size:100px;\">"+QString::number(calibrationCounter)+"</span>");
		camWidget->update();
		calibrationCounter--;
	}
	// Lorsqu'il atteint 0, on arrete le timer et on poursuit l'étalonnage
	else
	{
		// Arrêt du timer
		timerCounter->stop();
			
		// Récupère l'image d'initialisation
		IplImage *image;
		image = camManager->getImageInit(webcamActive);

		// Appel WidgetWebcam avec l'image récupérée
		camWidget->calibrate(image);
	}
}

void Client::slotStart()
{
	// On désactive le choix des webcams
	ui->menuWebcam->setEnabled(false);
	
	// On récupère la webcam sélectionnée
	QList<QAction*> listActionsWebcam = actionGroup->actions();
	webcamActive = 0;
	int i=0;
	for (i=0; i<listActionsWebcam.length(); i++)
	{
		if (listActionsWebcam[i]->isChecked())
		{ webcamActive = listActionsWebcam[i]->text().remove("webcam").toInt(); }
	}
	// La numérotation commence à 0
	webcamActive--;
	
	// On débute l'étalonnage
	calibration();
}
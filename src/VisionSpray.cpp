#include "VisionSpray.h"
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QApplication>
#include <QTimer>

VisionSpray::VisionSpray()
{
       qRegisterMetaType< cv::Mat >("cv::Mat"); 
//    modi = new algoritm();
#ifdef USE_CAMERA
       this->camera = new QTGIGE("Basler-21272795");
     this->camera->startAquisition();
    connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat, qint64)), &exg, SLOT(newBayerGRImage(cv::Mat, qint64)), Qt::QueuedConnection);
#endif
    
  #ifdef USE_GPS
      this->gps = new gpsReader();
//       this->loadGPS();
  #endif
   
#ifdef USE_DATALOGGER
//    this->log = new dataLogger();
//     connect(this->modi->timeKeeper, SIGNAL(spray(int)), this->log, SLOT(valve1Logger(int)));
//     connect(this->modi, SIGNAL(weedAmount(float)), this->log, SLOT(weedAmountLogger(float)));
//     connect(this->modi, SIGNAL(weedPressure(float)), this->log, SLOT(weedPressureLogger(float)));
//     connect(this->modi, SIGNAL(runtime(qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64)),
// 	    this->log, SLOT(runtimeLogger(qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64,qint64)));
  #ifdef USE_CAMERA
/*      connect(this->camera, SIGNAL(newImage(void*)), this->log, SLOT(rawImageLogger(void*)));
      connect(this->modi, SIGNAL(showImage(cv::Mat*)), this->log, SLOT(pngImageLogger(cv::Mat*))); */     
  #endif

  #ifdef USE_GPS
//       connect(this->gps, SIGNAL(newGGA(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)),
// 	      this->log, SLOT(GGALogger(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)));
//       connect(this->gps, SIGNAL(newNMEASentence(QByteArray,QByteArray,QList<QByteArray>)),
// 	      this->log, SLOT(NMEALogger(QByteArray,QByteArray,QList<QByteArray>)));
  #endif
#endif
      
    drawGui();   
    connect(&exg, SIGNAL(newImage(cv::Mat, qint64)), view, SLOT(showImage(cv::Mat, qint64)));

      


 //   connect(this->modi, SIGNAL(showImage(cv::Mat*)), this->view, SLOT(showImage(cv::Mat*)));
//     connect(Valve1Btn, SIGNAL(pressed()), this, SLOT(turnValve1On()));
//     connect(Valve1Btn, SIGNAL(released()), this, SLOT(turnValve1Off()));
//     connect(Valve1Btn, SIGNAL(pressed()), this, SLOT(turnValve2On()));
//     connect(Valve1Btn, SIGNAL(released()), this, SLOT(turnValve2Off()));
//     connect(modi, SIGNAL(nozzleOn()), this, SLOT(turnValve1On()));
//     connect(modi, SIGNAL(nozzleOff()), this, SLOT(turnValve1Off()));
//     connect(modi, SIGNAL(statusText(QString)), this->modicoviText, SLOT(setText(QString)));
    connect(cameraSettingsBtn, SIGNAL(pressed()), camera, SLOT(showCameraSettings()));
}



void VisionSpray::drawGui(void )
{
    this->globalWidget = new QWidget(this);
    this->Layout = new QGridLayout(this->globalWidget);
    this->view = new CQtOpenCVViewerGl(this);
    this->Valve1Btn = new QPushButton("Valve 1");
    this->Valve2Btn = new QPushButton("Valve 2");
    this->cameraSettingsBtn = new QPushButton("Camera settings");
    this->imageSelect = new QComboBox(globalWidget);
    //connect(this->imageSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentViewChanged(QString)));
    //connect(this->imageSelect, SIGNAL(currentIndexChanged(QString)), modi, SLOT(imshowSelector(QString)));
    this->imageSelect->addItem("Input");
    this->imageSelect->addItem("Excess Green");
    this->imageSelect->addItem("Segmented");
    this->imageSelect->addItem("Segmented Morphology");
    this->imageSelect->addItem("Symmetry kernel magnitude");
    this->imageSelect->addItem("Symmetry kernel threshold");
    this->imageSelect->addItem("Edges thinned");
    this->modicoviText = new QLabel("Modicovi Score:");
    this->sideWidget = new QWidget(globalWidget);
    this->sideLayout = new QGridLayout(this->sideWidget);
    this->view->setMinimumHeight(768);
    this->view->setMinimumWidth(1024);
    this->Layout->addWidget(view, 1,1);
    this->Layout->addWidget(imageSelect, 2,1);
    this->Layout->addWidget(sideWidget, 1,2);
    this->sideLayout->addWidget(Valve1Btn, 2,1);
    this->sideLayout->addWidget(cameraSettingsBtn, 3,1);
    this->sideLayout->addWidget(modicoviText, 1,1);
#ifdef USE_GPS
//     this->drawGPSGui();
    this->gpswidget = new gpsWidget(gps);
    this->Layout->addWidget(gpswidget, 1,3);
#endif
    this->sideWidget->setLayout(this->sideLayout);
    this->globalWidget->setLayout(this->Layout);
    setCentralWidget(this->globalWidget);
}

// #ifdef USE_GPS
// void VisionSpray::loadGPS(void )
// {
//    this->gps = new gpsReader();
//     connect(this->gps, SIGNAL(satellitesUpdated(SatList)), this, SLOT(updateSatlist(SatList)));
//     connect(this->gps, SIGNAL(newGGA(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)),
//     this, SLOT(updateSatStatus(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)));
//     connect(this->gps, SIGNAL(newVTG(QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,float,QByteArray)),
// 	    this, SLOT(VTGReceiver(QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,float,QByteArray)));
//     //connect(this, SIGNAL(velocity(float)), this->modi, SLOT(velocityReceiver(float)));
// }

// void VisionSpray::drawGPSGui(void )
// {
//     this->gpsWidget = new QWidget(this->globalWidget);
//     this->gpsLayout = new QGridLayout;
//     this->satWidget = new SatView();
//     
//     this->gpsQuality = new QButtonGroup(this->gpsWidget);
//     this->gpsQualityInvalid = new QRadioButton("Invalid");
//     this->gpsQualityGPSFix = new QRadioButton("GPS Fix");
//     this->gpsQualityDGPSFix = new QRadioButton("DGPS fix");
//     this->gpsQualityPPSFix = new QRadioButton("PPS fix");
//     this->gpsQualityRTKFix = new QRadioButton("RTK fix");
//     this->gpsQualityFRTKFix = new QRadioButton("Float RTK");
//     this->gpsQualityEstimated = new QRadioButton("Dead reckoning");
//     this->gpsQualityManual = new QRadioButton("Manual input");
//     this->gpsQualitySimulation = new QRadioButton("Simulation");
//     this->gpsQuality->addButton(this->gpsQualityInvalid);
//     this->gpsQuality->addButton(this->gpsQualityGPSFix);
//     this->gpsQuality->addButton(this->gpsQualityDGPSFix);
//     this->gpsQuality->addButton(this->gpsQualityPPSFix);
//     this->gpsQuality->addButton(this->gpsQualityRTKFix);
//     this->gpsQuality->addButton(this->gpsQualityFRTKFix);
//     this->gpsQuality->addButton(this->gpsQualityEstimated);
//     this->gpsQuality->addButton(this->gpsQualityManual);
//     this->gpsQuality->addButton(this->gpsQualitySimulation);
//     this->gpsLayout->addWidget(this->satWidget,0,0);
//     this->gpsLayout->addWidget(this->gpsQualityInvalid, 1,0);
//     this->gpsLayout->addWidget(this->gpsQualityGPSFix, 2,0);
//     this->gpsLayout->addWidget(this->gpsQualityDGPSFix, 3,0);
//     this->gpsLayout->addWidget(this->gpsQualityPPSFix, 4,0);
//     this->gpsLayout->addWidget(this->gpsQualityRTKFix,5,0);
//     this->gpsLayout->addWidget(this->gpsQualityFRTKFix, 6,0);
//     this->gpsLayout->addWidget(this->gpsQualityEstimated, 7,0);
//     this->gpsLayout->addWidget(this->gpsQualityManual, 8,0);
//     this->gpsLayout->addWidget(this->gpsQualitySimulation, 9,0);
//     this->gpsWidget->setLayout(this->gpsLayout);
//     
//     this->gpsQualityInvalid->setChecked(true);
// }


// void VisionSpray::updateSatlist(SatList sats)
// {
//   SatList seenPRNs;
//   for(int i=0;i<sats.count();i++)
//   {
//     if(sats[i].prn != -1)
//       seenPRNs.push_back(sats[i]);
//   }
//   this->satWidget->setSatellites(seenPRNs);
// }

// void VisionSpray::updateSatStatus(QByteArray time, QByteArray latitude, char latitudeHeading, QByteArray longitude, char longitudeHeading, int GPSQuality, int sattelitesInView, float horizontalDilution, float altitude, char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit, float dGPSAge, int dGPSStation)
// {
//   switch(GPSQuality)
//   {
//     case 0:
// //       this->setStyleSheet("{background-color: red; }");
//       this->gpsQualityInvalid->setChecked(true);
//       break;
//     case 1:
// //       this->setStyleSheet("{background-color: yellow; }");
//       this->gpsQualityGPSFix->setChecked(true);
//       break;
//     case 2:
// //       this->setStyleSheet("{background-color: yellow; }");
//       this->gpsQualityDGPSFix->setChecked(true);
//       break;
//     case 3:
//       this->gpsQualityPPSFix->setChecked(true);
//       break;
//     case 4:
// //       this->setStyleSheet("{background-color: green; }");
//       this->gpsQualityRTKFix->setChecked(true);
//       break;
//     case 5:
// //       this->setStyleSheet("{background-color: green; }");
//       this->gpsQualityFRTKFix->setChecked(true);
//       break;
//     case 6:
//       this->gpsQualityEstimated->setChecked(true);
//       break;
//     case 7:
//       this->gpsQualityManual->setChecked(true);
//       break;
//     case 8:
//       this->gpsQualitySimulation->setChecked(true);
//       break;
//     default:
//       this->gpsQualityInvalid->setChecked(true);
//       break;
//     
//   }
// }

// void VisionSpray::VTGReceiver(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator, QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator, QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit, float GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit)
// {
//   emit(velocity(GroundSpeedInKmh));
// }

// #endif

void VisionSpray::currentViewChanged(const QString& text)
{
  std::cout << "Received new view " << text.toLocal8Bit().data() << std::endl;
  disconnect(this->view, SLOT(showImage(cv::Mat*)));
}

void VisionSpray::turnValve1Off(void )
{
#ifdef USE_CAMERA
//  this->camera->resetUserOutput0();
#endif
}

void VisionSpray::turnValve1On(void )
{
#ifdef USE_CAMERA
//  this->camera->setUserOutput0();
#endif
}

void VisionSpray::turnValve2Off(void )
{
#ifdef USE_CAMERA
//  this->camera->resetUserOutput1();
#endif
}

void VisionSpray::turnValve2On(void )
{
#ifdef USE_CAMERA
//  this->camera->setUserOutput1();
#endif
}


VisionSpray::~VisionSpray()
{}

#include "VisionSpray.moc"

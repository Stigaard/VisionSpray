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
#ifdef USE_CAMERA
    this->cameraOne = new QTGIGE("Basler-21325585");
    //this->cameraTwo = new QTGIGE("Basler-21322519");
    this->cameraOne->setROI(500, 500, 500, 500);
    this->cameraOne->startAquisition();
    //this->cameraTwo->startAquisition();
//    connect(this->cameraOne, SIGNAL(newBayerGRImage(cv::Mat)), &demOne, SLOT(newBayerGRImage(cv::Mat)), Qt::QueuedConnection);
    connect(this->cameraOne, SIGNAL(newBayerGRImage(cv::Mat)), &demOne, SLOT(newBayerGRImage(cv::Mat)));
    //connect(this->cameraTwo, SIGNAL(newBayerGRImage(cv::Mat)), &demTwo, SLOT(newBayerGRImage(cv::Mat)));
#endif

#ifdef USE_DATALOGGER
    std::cout << "Logger activated" << std::endl;
    this->log = new dataLogger("../VisionSprayDataLog/", " VisionSpray log");
    #ifdef USE_CAMERA
    connect(&demOne, SIGNAL(newImage(cv::Mat)), this->log, SLOT(pngImageLoggerCameraOne(cv::Mat)));
//    connect(&demTwo, SIGNAL(newImage(cv::Mat)), this->log, SLOT(pngImageLoggerCameraTwo(cv::Mat)));
//    connect(this->modi, SIGNAL(showImage(cv::Mat*)), this->log, SLOT(pngImageLogger(cv::Mat*)));
    #endif
#endif

    drawGui();
    connect(&demOne, SIGNAL(newImage(cv::Mat)), view, SLOT(showImage(cv::Mat)));

#ifndef USE_CAMERA
    init_CameraSimulator();
    QTimer * camSimTimer = new QTimer(this);
    connect(camSimTimer, SIGNAL(timeout()), this, SLOT(cameraSimulator()));
    connect(this, SIGNAL(newSimulatedImage(cv::Mat*)), this->modi, SLOT(evaluateImage(cv::Mat*)));
    this->imageSelect->setCurrentIndex(3);
    camSimTimer->start(1000);
#pragma message "compiling simulator"
#endif



    connect(cameraSettingsBtn, SIGNAL(pressed()), cameraOne, SLOT(showCameraSettings()));
}

#ifndef USE_CAMERA
void VisionSpray::init_CameraSimulator(void )
{
    QDir simdir("/home/morten/Dropbox/Ph.D./MoDiCoVi field trials/Spray/2012-10-08 14:40:07.001 MoDiCoVi log/converted_png/");
    simdir.setSorting(QDir::Name);
    simulationFiles = new QFileInfoList();
    *simulationFiles = simdir.entryInfoList();
    fileptr = 2;
//  connect(this, SIGNAL(newSimulatedImage(cv::Mat*)), this->view, SLOT(showImage(cv::Mat*)));
}

void VisionSpray::cameraSimulator(void )
{
    fileptr++;
    if(fileptr>=simulationFiles->size())
    {
        fileptr--;
        return;
        fileptr = 2;
    }
    std::cout << "file:" << simulationFiles->at(fileptr).absoluteFilePath().toLocal8Bit().constData() << std::endl;
    cv::Mat img;
    img = cv::imread(simulationFiles->at(fileptr).absoluteFilePath().toLocal8Bit().constData());
    cv::Mat RGB161616(768,1024, cv::DataType<uint16_t>::type);
    img.convertTo(RGB161616, RGB161616.type(), 256.0);
    emit(newSimulatedImage(&RGB161616));
}
#endif

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
    this->drawGPSGui();
    this->Layout->addWidget(gpsWidget, 1,3);
#endif
    this->sideWidget->setLayout(this->sideLayout);
    this->globalWidget->setLayout(this->Layout);
    setCentralWidget(this->globalWidget);
}

#ifdef USE_GPS
void VisionSpray::loadGPS(void )
{
    this->gps = new gpsReader();
    connect(this->gps, SIGNAL(satellitesUpdated(SatList)), this, SLOT(updateSatlist(SatList)));
    connect(this->gps, SIGNAL(newGGA(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)),
            this, SLOT(updateSatStatus(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)));
    connect(this->gps, SIGNAL(newVTG(QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,float,QByteArray)),
            this, SLOT(VTGReceiver(QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,float,QByteArray)));
    //connect(this, SIGNAL(velocity(float)), this->modi, SLOT(velocityReceiver(float)));
}

void VisionSpray::drawGPSGui(void )
{
    this->gpsWidget = new QWidget(this->globalWidget);
    this->gpsLayout = new QGridLayout;
    this->satWidget = new SatView();

    this->gpsQuality = new QButtonGroup(this->gpsWidget);
    this->gpsQualityInvalid = new QRadioButton("Invalid");
    this->gpsQualityGPSFix = new QRadioButton("GPS Fix");
    this->gpsQualityDGPSFix = new QRadioButton("DGPS fix");
    this->gpsQualityPPSFix = new QRadioButton("PPS fix");
    this->gpsQualityRTKFix = new QRadioButton("RTK fix");
    this->gpsQualityFRTKFix = new QRadioButton("Float RTK");
    this->gpsQualityEstimated = new QRadioButton("Dead reckoning");
    this->gpsQualityManual = new QRadioButton("Manual input");
    this->gpsQualitySimulation = new QRadioButton("Simulation");
    this->gpsQuality->addButton(this->gpsQualityInvalid);
    this->gpsQuality->addButton(this->gpsQualityGPSFix);
    this->gpsQuality->addButton(this->gpsQualityDGPSFix);
    this->gpsQuality->addButton(this->gpsQualityPPSFix);
    this->gpsQuality->addButton(this->gpsQualityRTKFix);
    this->gpsQuality->addButton(this->gpsQualityFRTKFix);
    this->gpsQuality->addButton(this->gpsQualityEstimated);
    this->gpsQuality->addButton(this->gpsQualityManual);
    this->gpsQuality->addButton(this->gpsQualitySimulation);
    this->gpsLayout->addWidget(this->satWidget,0,0);
    this->gpsLayout->addWidget(this->gpsQualityInvalid, 1,0);
    this->gpsLayout->addWidget(this->gpsQualityGPSFix, 2,0);
    this->gpsLayout->addWidget(this->gpsQualityDGPSFix, 3,0);
    this->gpsLayout->addWidget(this->gpsQualityPPSFix, 4,0);
    this->gpsLayout->addWidget(this->gpsQualityRTKFix,5,0);
    this->gpsLayout->addWidget(this->gpsQualityFRTKFix, 6,0);
    this->gpsLayout->addWidget(this->gpsQualityEstimated, 7,0);
    this->gpsLayout->addWidget(this->gpsQualityManual, 8,0);
    this->gpsLayout->addWidget(this->gpsQualitySimulation, 9,0);
    this->gpsWidget->setLayout(this->gpsLayout);

    this->gpsQualityInvalid->setChecked(true);
}


void VisionSpray::updateSatlist(SatList sats)
{
    SatList seenPRNs;
    for(int i=0; i<sats.count(); i++)
    {
        if(sats[i].prn != -1)
            seenPRNs.push_back(sats[i]);
    }
    this->satWidget->setSatellites(seenPRNs);
}

void VisionSpray::updateSatStatus(QByteArray time, QByteArray latitude, char latitudeHeading, QByteArray longitude, char longitudeHeading, int GPSQuality, int sattelitesInView, float horizontalDilution, float altitude, char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit, float dGPSAge, int dGPSStation)
{
    switch(GPSQuality)
    {
    case 0:
//       this->setStyleSheet("{background-color: red; }");
        this->gpsQualityInvalid->setChecked(true);
        break;
    case 1:
//       this->setStyleSheet("{background-color: yellow; }");
        this->gpsQualityGPSFix->setChecked(true);
        break;
    case 2:
//       this->setStyleSheet("{background-color: yellow; }");
        this->gpsQualityDGPSFix->setChecked(true);
        break;
    case 3:
        this->gpsQualityPPSFix->setChecked(true);
        break;
    case 4:
//       this->setStyleSheet("{background-color: green; }");
        this->gpsQualityRTKFix->setChecked(true);
        break;
    case 5:
//       this->setStyleSheet("{background-color: green; }");
        this->gpsQualityFRTKFix->setChecked(true);
        break;
    case 6:
        this->gpsQualityEstimated->setChecked(true);
        break;
    case 7:
        this->gpsQualityManual->setChecked(true);
        break;
    case 8:
        this->gpsQualitySimulation->setChecked(true);
        break;
    default:
        this->gpsQualityInvalid->setChecked(true);
        break;

    }
}

void VisionSpray::VTGReceiver(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator, QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator, QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit, float GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit)
{
    emit(velocity(GroundSpeedInKmh));
}

#endif

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

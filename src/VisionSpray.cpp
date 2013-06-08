#include "VisionSpray.h"
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QApplication>
#include <QTimer>
#include <QDateTime>


VisionSpray::VisionSpray()
{
      qRegisterMetaType< cv::Mat >("cv::Mat"); 
     QString cameraSerial;
     if(settings.contains("camera1/serial"))
      cameraSerial = settings.value("camera/serial").toString();
     else
     {
       cameraSerial = "Basler-21272795";
       settings.setValue("camera1/serial", "Basler-21272795");
     }

     settings.sync();
     std::cout << "Config file:" << settings.fileName().toLocal8Bit().constData() << std::endl;
     if(settings.status()!=QSettings::NoError)
     {
       std::cout << "Error ocurred with settings" << std::endl;
       if(settings.status()==QSettings::AccessError)
       {
	 std::cout << "Access error" << std::endl;
	std::cout << "Config file:" << settings.fileName().toLocal8Bit().constData() << std::endl;
       }
       else if(settings.status()==QSettings::FormatError)
	 std::cout << "Format error" << std::endl;
       else
	 std::cout << "Unknown error" << std::endl;
     }
#ifdef PLOT_VELOCITY
     initPlots();
#endif
     
#ifdef USE_GPS
    this->gps = new gpsReader();
    connect(this->gps, SIGNAL(velocity(float)), &m_velocityfilter, SLOT(velocitySlot(float)));
    connect(this->gps, SIGNAL(velocity(float)), this, SLOT(velocityEcho(float)));
  #ifdef PLOT_VELOCITY
    connect(this->gps, SIGNAL(velocity(float)), this, SLOT(rawVelPlot(float)));
  #endif
#endif
     drawGui();
     cameraSerial = "Basler-21272794";
     //TODO: Fix camera serial readin from config file
     //return;
     std::cout << "Camera serial:" << cameraSerial.toLocal8Bit().constData() << std::endl;
     this->camera = new QTGIGE(cameraSerial.toLocal8Bit().constData());
     float acqFramerate = 5;
     this->camera->writeBool("AcquisitionFrameRateEnable", true);
     this->camera->writeFloat("AcquisitionFrameRateAbs", acqFramerate);
#ifdef USE_DATALOGGER
     this->imageLog = new ImageLogger("../Logging", "rawImages");
     this->velocityLogger = new LoggerModule("../Logging", "Velocity");
     connect(this->gps, SIGNAL(velocity(float)), this, SLOT(velocityLog(float)));
     connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat,qint64)), this->imageLog, SLOT(pngImageLogger(cv::Mat,qint64)));
#endif
     
     this->spraytimekeeper = new SprayTimeKeeper(this, &nz);
     
     this->camera->startAquisition();
     
     this->camera->loadCorrectionImage("../include/QtGigE/correctionimage.png");
     
     this->modicovi = new modicovi_rt;
     connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat, qint64)), this->camera, SLOT(correctVignetting(cv::Mat, qint64)), Qt::QueuedConnection);
     connect(this->camera, SIGNAL(vignettingCorrectedInImage(cv::Mat, qint64)), &exg, SLOT(newBayerGRImage(cv::Mat, qint64)), Qt::QueuedConnection);

         
    
    connect(this->Valve1Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve2Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve3Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));

    //connect(&exg, SIGNAL(newImage(cv::Mat,qint64)), &m_rowDetect, SLOT(analyze(cv::Mat,qint64)));
    connect(&exg,SIGNAL(newImage(cv::Mat,qint64)),&m_greendetect,SLOT(analyze(cv::Mat,qint64)));
    //connect(&m_rowDetect,SIGNAL(analysisResult(cv::Mat,qint64)),view,SLOT(showImage(cv::Mat,qint64)));
    connect(&m_greendetect,SIGNAL(analysisResult(cv::Mat_<uint8_t>,qint64)),&m_sprayplanner,SLOT(sprayMap(cv::Mat_<uint8_t>,qint64)));
    connect(&m_sprayplanner,SIGNAL(sprayNozzleMap(cv::Mat_<uint8_t>, qint64)),view,SLOT(updateOverlayBuffer(cv::Mat_<uint8_t>,qint64)));
//    connect(&armadillo, SIGNAL(forwardVelocity(float)),&m_sprayplanner,SLOT(velocity(float)));
    //connect(&m_sprayplanner,SIGNAL(sprayNozzleMap(cv::Mat_<uint8_t>,qint64)),view,SLOT(showImage(cv::Mat_<uint8_t>,qint64)));
    connect(this->Valve1Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(this->Valve2Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(this->Valve3Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    
    //connect(&m_rowDetect,SIGNAL(debugImage(cv::Mat,qint64)),view, SLOT(showImage(cv::Mat, qint64)));
    connect(&exg, SIGNAL(newImage(cv::Mat, qint64)), view, SLOT(updateBuffer(cv::Mat,qint64)));
    //connect(&exg, SIGNAL(newImage(cv::Mat, qint64)), view, SLOT(showImage(cv::Mat,qint64)));
    
    connect(cameraSettingsBtn, SIGNAL(pressed()), camera, SLOT(showCameraSettings()));
    
    connect(&m_sprayplanner,SIGNAL(spray(int,qint64,qint64)),spraytimekeeper,SLOT(Spray(int,qint64,qint64)));
    
    //connect(&(this->armadillo),SIGNAL(forwardVelocity(float)), this, SLOT(velocityEcho(float))); 
    connect(&m_velocityfilter, SIGNAL(velocity(float)), &m_sprayplanner, SLOT(velocity(float)));
#ifdef PLOT_VELOCITY
    connect(&m_velocityfilter, SIGNAL(velocity(float)), this, SLOT(filtVelPlot(float)));
    //this->spraytimekeeper->Spray(0, (QDateTime::currentMSecsSinceEpoch()+10000)*1000, (QDateTime::currentMSecsSinceEpoch()+12000)*1000);
    plotTimer = new QTimer(this);
    connect(this->plotTimer, SIGNAL(timeout()), this, SLOT(updatePlots()));
    plotTimer->start(100);
#endif
}

void VisionSpray::initPlots(void )
{
        m_VelPlot = new QwtPlot;
        m_VelPlot->resize(800,600);
        m_filtVelCurve = new QwtPlotCurve();
        m_filtVelCurve->setPen(QPen(QColor("blue")));
        m_filtVelCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
        m_rawVelCurve = new QwtPlotCurve();
        m_rawVelCurve->setPen(QPen(QColor("red")));
        m_filtVelCurve->attach(m_VelPlot);
        m_rawVelCurve->attach(m_VelPlot);
	filtVelocities.reserve(30*20);
	rawVelocities.reserve(30*20);
	velocityTimes.reserve(30*20);
	for(int i=0;i<30*20;i++)
	{
	  velocityTimes.enqueue(((float)((30*20)-i))*(1.0/20.0));
	  filtVelocities.enqueue(0);
	  rawVelocities.enqueue(0);
	}
}

void VisionSpray::filtVelPlot(float v)
{
  filtVelocities.dequeue();
  filtVelocities.enqueue(v);
  m_filtVelCurve->setSamples(velocityTimes.toVector(), filtVelocities.toVector());
}


void VisionSpray::rawVelPlot(float v)
{
  rawVelocities.dequeue();
  rawVelocities.enqueue(v);
  m_rawVelCurve->setSamples(velocityTimes.toVector(), rawVelocities.toVector());
}

void VisionSpray::updatePlots(void )
{
    m_VelPlot->replot();
    m_VelPlot->show();
}



void VisionSpray::velocityLog(float v)
{
  #ifdef USE_DATALOGGER
  this->velocityLogger->log("Velocity", QString::number(v).toLocal8Bit().constData());
  #endif
}

void VisionSpray::velocityEcho(float v)
{
  static int i = 0;
  this->modicoviText->setText(QString::number(round(v*10)/10));
  if(v>0.15)
    if(i++>10)
    {
      std::cout << "Velocity:" << round(v*10)/10 << "m/s" << std::endl;
      i = 0;
    }
}


void VisionSpray::valveButtonMapper()
{
  QPushButton * pb = (QPushButton*)QObject::sender();
  std::cout << "Received order!" << std::endl;
  if(pb == this->Valve1Btn)
    nz.spray(0, pb->isDown());
  if(pb == this->Valve2Btn)
    nz.spray(1, pb->isDown());
  if(pb == this->Valve3Btn)
    nz.spray(2, pb->isDown());
}


void VisionSpray::drawGui(void )
{
    this->globalWidget = new QWidget(this);
    this->Layout = new QGridLayout(this->globalWidget);
    this->view = new CQtOpenCVViewerGl(this);
    this->Valve1Btn = new QPushButton("Valve 1");
    this->Valve2Btn = new QPushButton("Valve 2");
    this->Valve3Btn = new QPushButton("Valve 3");
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
    this->sideLayout->addWidget(Valve2Btn, 3,1);
    this->sideLayout->addWidget(Valve3Btn, 4,1);
    this->sideLayout->addWidget(cameraSettingsBtn, 5,1);
    this->sideLayout->addWidget(modicoviText, 1,1);
#ifdef USE_GPS
    this->gpswidget = new gpsWidget(gps);
    this->Layout->addWidget(gpswidget, 1,3);
#endif
    this->sideWidget->setLayout(this->sideLayout);
    this->globalWidget->setLayout(this->Layout);
    setCentralWidget(this->globalWidget);
}

void VisionSpray::currentViewChanged(const QString& text)
{
  std::cout << "Received new view " << text.toLocal8Bit().data() << std::endl;
  disconnect(this->view, SLOT(showImage(cv::Mat*)));
}

VisionSpray::~VisionSpray()
{}

#include "VisionSpray.moc"

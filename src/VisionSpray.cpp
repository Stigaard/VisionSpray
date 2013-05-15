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

     this->camera = new QTGIGE(cameraSerial.toLocal8Bit().constData());
     this->camera->startAquisition();
     connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat, qint64)), &exg, SLOT(newBayerGRImage(cv::Mat, qint64)), Qt::QueuedConnection);
#ifdef USE_GPS
    this->gps = new gpsReader();
#endif
         
    drawGui();
    connect(this->Valve1Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve2Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve3Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    //connect(&exg, SIGNAL(newImage(cv::Mat)), view, SLOT(showImage(cv::Mat)));
    connect(&exg, SIGNAL(newImage(cv::Mat)), &m_rowDetect, SLOT(analyze(cv::Mat)));
    connect(&m_rowDetect,SIGNAL(analysisResult(cv::Mat)),view,SLOT(showImage(cv::Mat)));
    
    connect(this->Valve1Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(this->Valve2Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(this->Valve3Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    
    connect(&exg, SIGNAL(newImage(cv::Mat, qint64)), view, SLOT(showImage(cv::Mat, qint64)));
    
    connect(cameraSettingsBtn, SIGNAL(pressed()), camera, SLOT(showCameraSettings()));
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

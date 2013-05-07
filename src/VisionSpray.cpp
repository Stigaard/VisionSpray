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
     this->camera = new QTGIGE("Basler-21272795");
     this->camera->startAquisition();
     connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat, qint64)), &exg, SLOT(newBayerGRImage(cv::Mat, qint64)), Qt::QueuedConnection);
#endif
    
  #ifdef USE_GPS
      this->gps = new gpsReader();
  #endif
         
    drawGui();   
    //connect(&exg, SIGNAL(newImage(cv::Mat)), view, SLOT(showImage(cv::Mat)));
    connect(&exg, SIGNAL(newImage(cv::Mat)), &m_rowDetect, SLOT(analyze(cv::Mat)));
    connect(&m_rowDetect,SIGNAL(analysisResult(cv::Mat)),view,SLOT(showImage(cv::Mat)));
    
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

void VisionSpray::currentViewChanged(const QString& text)
{
  std::cout << "Received new view " << text.toLocal8Bit().data() << std::endl;
  disconnect(this->view, SLOT(showImage(cv::Mat*)));
}

VisionSpray::~VisionSpray()
{}

#include "VisionSpray.moc"

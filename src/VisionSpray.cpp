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
    initOpenCV();
    initConfigFile();
    initTreatmentDatabase();
    initCamera();
    initGPS();
    initPlots();
    initDatalogger();
    initModicovi();
    initRowDetect();
    initSprayPlanner();
    initSprayTimeKeeper();
    initParcelReceiver();

    drawGui();

    this->camera->startAquisition();
    
    currentViewChanged("Excess Green");
    currentViewChanged("Input");   
}

void VisionSpray::initOpenCV(void )
{
  qRegisterMetaType< cv::Mat >("cv::Mat");
}

void VisionSpray::initTreatmentDatabase(void )
{
  QString TreatmentDatabaseFile;
  if(settings.contains("TreatmentDescription/file"))
  TreatmentDatabaseFile = settings.value("TreatmentDescription/file").toString();
  else
  {
    TreatmentDatabaseFile = "../experimentalConditions/Flakkebjerg/ParcelTreatmentPlan.csv";
    settings.setValue("TreatmentDescription/file", "../experimentalConditions/Flakkebjerg/ParcelTreatmentPlan.csv");
  }
  std::cout << "Using treasment description:" << TreatmentDatabaseFile.toLocal8Bit().constData() << std::endl;
  tdb = new TreatmentDatabase(TreatmentDatabaseFile);
}

void VisionSpray::initSprayTimeKeeper(void )
{
     this->spraytimekeeper = new SprayTimeKeeper(this, &nz);
     connect(&m_sprayplanner,SIGNAL(spray(int,qint64,qint64)),spraytimekeeper,SLOT(Spray(int,qint64,qint64)));
}


void VisionSpray::initSprayPlanner(void )
{
  connect(&m_velocityfilter, SIGNAL(velocity(float)), &m_sprayplanner, SLOT(velocity(float)));
}


void VisionSpray::initModicovi(void )
{
     this->modicovi_threshold1 = new modicovi_rt(0.1);
     this->modicovi_threshold2 = new modicovi_rt(0.2);
     this->modicovi_threshold3 = new modicovi_rt(0.3);
     this->modicovi_threshold4 = new modicovi_rt(0.4);
     this->modicovi_threshold5 = new modicovi_rt(0.5);
     connect(this->modicovi_threshold1, SIGNAL(sprayMap(cv::Mat_<uint8_t>,qint64)), 
	     &(this->m_sprayplanner), SLOT(sprayMap(cv::Mat_<uint8_t>,qint64)));
     connect(this->modicovi_threshold2, SIGNAL(sprayMap(cv::Mat_<uint8_t>,qint64)), 
	     &(this->m_sprayplanner), SLOT(sprayMap(cv::Mat_<uint8_t>,qint64)));
     connect(this->modicovi_threshold3, SIGNAL(sprayMap(cv::Mat_<uint8_t>,qint64)), 
	     &(this->m_sprayplanner), SLOT(sprayMap(cv::Mat_<uint8_t>,qint64)));
     connect(this->modicovi_threshold4, SIGNAL(sprayMap(cv::Mat_<uint8_t>,qint64)), 
	     &(this->m_sprayplanner), SLOT(sprayMap(cv::Mat_<uint8_t>,qint64)));
     connect(this->modicovi_threshold5, SIGNAL(sprayMap(cv::Mat_<uint8_t>,qint64)), 
	     &(this->m_sprayplanner), SLOT(sprayMap(cv::Mat_<uint8_t>,qint64)));
}

void VisionSpray::initRowDetect(void )
{
     double vegetationInRow = 0.7;
     double ExGThresholdValue = 0.52*255;

     // Value found by this equation 75 cm * 2046 pix / 87 cm =~ 1764.
     double distanceBetweenRows = 1764;  

     this->m_rowDetect1 = new RowDetect(vegetationInRow, ExGThresholdValue, 180, distanceBetweenRows);
     this->m_rowDetect2 = new RowDetect(vegetationInRow, ExGThresholdValue, 190, distanceBetweenRows);
     this->m_rowDetect3 = new RowDetect(vegetationInRow, ExGThresholdValue, 200, distanceBetweenRows);
     this->m_rowDetect4 = new RowDetect(vegetationInRow, ExGThresholdValue, 210, distanceBetweenRows);
     this->m_rowDetect5 = new RowDetect(vegetationInRow, ExGThresholdValue, 220, distanceBetweenRows);
     
     // TODO: I think the above classes should be connected with the spray planner.
}


void VisionSpray::initCamera(void )
{
     QString cameraSerial;
     if(settings.contains("camera1/serial"))
      cameraSerial = settings.value("camera1/serial").toString();
     else
     {
       cameraSerial = "Basler-21272795";
       settings.setValue("camera1/serial", "Basler-21272795");
     }
     std::cout << "Camera serial:" << cameraSerial.toLocal8Bit().constData() << std::endl;
     this->camera = new QTGIGE(cameraSerial.toLocal8Bit().constData());
     float acqFramerate = 3;
     this->camera->writeBool("AcquisitionFrameRateEnable", true);
     this->camera->writeFloat("AcquisitionFrameRateAbs", acqFramerate);
     this->camera->setROI(0, 0, 2046, 1086);
     this->camera->loadCorrectionImage("../include/QtGigE/correctionimageOne.png");
     connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat, qint64)), 
	     this->camera, SLOT(correctVignetting(cv::Mat, qint64)), Qt::QueuedConnection);
     connect(this->camera, SIGNAL(vignettingCorrectedInImage(cv::Mat, qint64)), 
	     &exg, SLOT(newBayerGRImage(cv::Mat, qint64)), Qt::QueuedConnection);
}

void VisionSpray::initGPS(void )
{
#ifdef USE_GPS
    this->gps = new gpsReader();
    connect(this->gps, SIGNAL(velocity(float)), &m_velocityfilter, SLOT(velocitySlot(float)));
//    connect(this->gps, SIGNAL(velocity(float)), this, SLOT(velocityEcho(float)));
  #ifdef PLOT_VELOCITY
    connect(this->gps, SIGNAL(velocity(float)), this, SLOT(rawVelPlot(float)));
  #endif
#endif
}

void VisionSpray::initConfigFile(void )
{
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
}

void VisionSpray::initDatalogger(void )
{
#ifdef USE_DATALOGGER
     this->imageLog = new ImageLogger("../Logging", "rawImages");
     this->velocityLogger = new LoggerModule("../Logging", "Velocity");
     #ifdef USE_GPS
     connect(this->gps, SIGNAL(velocity(float)), this, SLOT(velocityLog(float)));
     #endif 
     connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat,qint64)), this->imageLog, SLOT(pngImageLogger(cv::Mat,qint64)));
#endif
}


void showTreatment(TreatmentType treatment)
{
  switch(treatment)
    {
      case NEVER_SPRAY:
	std::cout << "Never spray" << std::endl;
	break;
      case ALWAYS_SPRAY:
	std::cout << "Always spray" << std::endl;
	break;
      case MODICOVI_THRESHOLD1:
	std::cout << "Modicovi threshold 1" << std::endl;
	break;
      case MODICOVI_THRESHOLD2:
	std::cout << "Modicovi threshold 2" << std::endl;
	break;
      case MODICOVI_THRESHOLD3:
	std::cout << "Modicovi threshold 3" << std::endl;
	break;
      case MODICOVI_THRESHOLD4:
	std::cout << "Modicovi threshold 4" << std::endl;
	break;
      case MODICOVI_THRESHOLD5:
	std::cout << "Modicovi threshold 5" << std::endl;
	break;
      case RULE_OF_THUMB_THRESHOLD1:
	std::cout << "Rule of thumb threshold 1" << std::endl;
	break;
      case RULE_OF_THUMB_THRESHOLD2:
	std::cout << "Rule of thumb threshold 2" << std::endl;
	break;
      case RULE_OF_THUMB_THRESHOLD3:
	std::cout << "Rule of thumb threshold 3" << std::endl;
	break;
      case RULE_OF_THUMB_THRESHOLD4:
	std::cout << "Rule of thumb threshold 4" << std::endl;
	break;
      case RULE_OF_THUMB_THRESHOLD5:
	std::cout << "Rule of thumb threshold 5" << std::endl;
	break;
      default:
	std::cout << "No match" << std::endl;
	assert(1 == 0);
    }
}

void VisionSpray::changeAlgorithm(TreatmentType treatment)
{
   disconnect(&exg,SIGNAL(newImage(cv::Mat,qint64)),0, 0);
    switch(treatment)
      {
	case NEVER_SPRAY:
	  std::cout << "Never spray" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)),&(this->m_always),SLOT(dontSpray(cv::Mat,qint64)));
	  this->algortihm_never_spray_radio->setChecked(true);
	  break;
	case ALWAYS_SPRAY:
	  std::cout << "Always spray" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)),&(this->m_always),SLOT(spray(cv::Mat,qint64)));
	  this->algortihm_always_spray_radio->setChecked(true);
	  break;
	case MODICOVI_THRESHOLD1:
	  connect(&(this->exg), SIGNAL(newImage(cv::Mat,qint64)), this->modicovi_threshold1, SLOT(evaluateImage(cv::Mat,qint64)));
	  this->algorithm_modicovi_threshold1->setChecked(true);
	  std::cout << "Modicovi threshold 1" << std::endl;
	  break;
	case MODICOVI_THRESHOLD2:
	  connect(&(this->exg), SIGNAL(newImage(cv::Mat,qint64)), this->modicovi_threshold2, SLOT(evaluateImage(cv::Mat,qint64)));
	  this->algorithm_modicovi_threshold2->setChecked(true);
	  std::cout << "Modicovi threshold 2" << std::endl;
	  break;
	case MODICOVI_THRESHOLD3:
	  connect(&(this->exg), SIGNAL(newImage(cv::Mat,qint64)), this->modicovi_threshold3, SLOT(evaluateImage(cv::Mat,qint64)));
	  this->algorithm_modicovi_threshold3->setChecked(true);
	  std::cout << "Modicovi threshold 3" << std::endl;
	  break;
	case MODICOVI_THRESHOLD4:
	  connect(&(this->exg), SIGNAL(newImage(cv::Mat,qint64)), this->modicovi_threshold4, SLOT(evaluateImage(cv::Mat,qint64)));
	  this->algorithm_modicovi_threshold4->setChecked(true);
	  std::cout << "Modicovi threshold 4" << std::endl;
	  break;
	case MODICOVI_THRESHOLD5:
	  connect(&(this->exg), SIGNAL(newImage(cv::Mat,qint64)), this->modicovi_threshold5, SLOT(evaluateImage(cv::Mat,qint64)));
	  this->algorithm_modicovi_threshold5->setChecked(true);
	  std::cout << "Modicovi threshold 5" << std::endl;
	  break;
	case RULE_OF_THUMB_THRESHOLD1:
	  this->algorithm_rule_of_thumb_threshold1->setChecked(true);
	  std::cout << "Rule of thumb threshold 1" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)), this->m_rowDetect1, SLOT(analyze(cv::Mat, qint64)));
	  break;
	case RULE_OF_THUMB_THRESHOLD2:
	  this->algorithm_rule_of_thumb_threshold2->setChecked(true);
	  std::cout << "Rule of thumb threshold 2" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)), this->m_rowDetect2, SLOT(analyze(cv::Mat, qint64)));
	  break;
	case RULE_OF_THUMB_THRESHOLD3:
	  this->algorithm_rule_of_thumb_threshold3->setChecked(true);
	  std::cout << "Rule of thumb threshold 3" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)), this->m_rowDetect3, SLOT(analyze(cv::Mat, qint64)));
	  break;
	case RULE_OF_THUMB_THRESHOLD4:
	  this->algorithm_rule_of_thumb_threshold4->setChecked(true);
	  std::cout << "Rule of thumb threshold 4" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)), this->m_rowDetect4, SLOT(analyze(cv::Mat, qint64)));
	  break;
	case RULE_OF_THUMB_THRESHOLD5:
	  this->algorithm_rule_of_thumb_threshold5->setChecked(true);
	  std::cout << "Rule of thumb threshold 5" << std::endl;
	  connect(&(this->exg),SIGNAL(newImage(cv::Mat,qint64)), this->m_rowDetect5, SLOT(analyze(cv::Mat, qint64)));
	  break;
	default:
	  std::cout << "No match" << std::endl;
	  assert(1 == 0);
      }
}


void VisionSpray::parcelNrReceiver(int parcel)
{
  TreatmentType treatment;
  enum direction{
    entering,
    leaving};
  direction  dir;
  if(parcel>0)
    dir=entering;
  else
  {
    dir=leaving;
    parcel = -parcel;
  }
  treatment = tdb->getTreatmentOfParcel(parcel);
  if(dir==entering)
  {
    changeAlgorithm(treatment);
  }
}

void VisionSpray::initAlgorithmRadio(void )
{
  algorithm_radio_group = new QGroupBox("Current Algorithm");
  algorithm_layout = new QVBoxLayout;
  this->algorithm_checkbox_mapper = new QSignalMapper(this);
  
  algortihm_never_spray_radio = new QRadioButton("Never Spray");
  algorithm_layout->addWidget(algortihm_never_spray_radio);
  connect(this->algortihm_never_spray_radio, SIGNAL(pressed()),
	  this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algortihm_never_spray_radio, (int)NEVER_SPRAY);
  
  algortihm_always_spray_radio = new QRadioButton("Always Spray");
  algorithm_layout->addWidget(algortihm_always_spray_radio);
  connect(this->algortihm_always_spray_radio, SIGNAL(pressed()),
	  this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algortihm_always_spray_radio, (int)ALWAYS_SPRAY);
  
  algorithm_modicovi_threshold1 = new QRadioButton("MoDiCoVi 1");
  algorithm_layout->addWidget(algorithm_modicovi_threshold1);
  connect(this->algorithm_modicovi_threshold1, SIGNAL(pressed()),
	  this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algorithm_modicovi_threshold1, (int)MODICOVI_THRESHOLD1);
  
  algorithm_modicovi_threshold2 = new QRadioButton("MoDiCoVi 2");
  algorithm_layout->addWidget(algorithm_modicovi_threshold2);
  connect(this->algorithm_modicovi_threshold2, SIGNAL(pressed()),
	  this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algorithm_modicovi_threshold2, (int)MODICOVI_THRESHOLD2);
  
  algorithm_modicovi_threshold3 = new QRadioButton("MoDiCoVi 3");
  algorithm_layout->addWidget(algorithm_modicovi_threshold3);
  connect(this->algorithm_modicovi_threshold3, SIGNAL(pressed()),
	this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algorithm_modicovi_threshold3, (int)MODICOVI_THRESHOLD3);
  
  algorithm_modicovi_threshold4 = new QRadioButton("MoDiCoVi 4");
  algorithm_layout->addWidget(algorithm_modicovi_threshold4);
  connect(this->algorithm_modicovi_threshold4, SIGNAL(pressed()),
	  this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algorithm_modicovi_threshold4, (int)MODICOVI_THRESHOLD4);
  
  algorithm_modicovi_threshold5 = new QRadioButton("MoDiCoVi 5");
  algorithm_layout->addWidget(algorithm_modicovi_threshold5);
  connect(this->algorithm_modicovi_threshold5, SIGNAL(pressed()),
	  this->algorithm_checkbox_mapper, SLOT(map()));
  algorithm_checkbox_mapper->setMapping(this->algorithm_modicovi_threshold5, (int)MODICOVI_THRESHOLD5);
  
  
  algorithm_rule_of_thumb_threshold1 = new QRadioButton("Rule of Thumb 1");
  algorithm_layout->addWidget(algorithm_rule_of_thumb_threshold1);
  algorithm_rule_of_thumb_threshold2 = new QRadioButton("Rule of Thumb 2");
  algorithm_layout->addWidget(algorithm_rule_of_thumb_threshold2);
  algorithm_rule_of_thumb_threshold3 = new QRadioButton("Rule of Thumb 3");
  algorithm_layout->addWidget(algorithm_rule_of_thumb_threshold3);
  algorithm_rule_of_thumb_threshold4 = new QRadioButton("Rule of Thumb 4");
  algorithm_layout->addWidget(algorithm_rule_of_thumb_threshold4);
  algorithm_rule_of_thumb_threshold5 = new QRadioButton("Rule of Thumb 5");
  algorithm_layout->addWidget(algorithm_rule_of_thumb_threshold5);
  this->algorithm_radio_group->setLayout(algorithm_layout);
  connect(this->algorithm_checkbox_mapper, SIGNAL(mapped(int)), this, SLOT(algorithmRadioCommand(int)));
}

void VisionSpray::algorithmRadioCommand(int cmd)
{
  std::cout << "Received checkbox request to change to " << (TreatmentType)cmd << std::endl;
  changeAlgorithm((TreatmentType)cmd);
}


void VisionSpray::initPlots(void )
{
#ifdef PLOT_VELOCITY

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
    connect(&m_velocityfilter, SIGNAL(velocity(float)), this, SLOT(filtVelPlot(float)));
    //this->spraytimekeeper->Spray(0, (QDateTime::currentMSecsSinceEpoch()+10000)*1000, (QDateTime::currentMSecsSinceEpoch()+12000)*1000);
    plotTimer = new QTimer(this);
    connect(this->plotTimer, SIGNAL(timeout()), this, SLOT(updatePlots()));
    plotTimer->start(100);
#endif
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
  //this->modicoviText->setText(QString::number(round(v*10)/10));
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
    this->overlayCheckbox = new QCheckBox( "Enable Overlay");
    this->cameraSettingsBtn = new QPushButton("Camera settings");
    this->imageSelect = new QComboBox(globalWidget);
    //connect(this->imageSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentViewChanged(QString)));
    //connect(this->imageSelect, SIGNAL(currentIndexChanged(QString)), modi, SLOT(imshowSelector(QString)));
    this->currentAlgorithmText = new QLabel("Modicovi Score:");
    this->sideWidget = new QWidget(globalWidget);
    this->sideLayout = new QGridLayout(this->sideWidget);
    this->view->setMinimumHeight(768);
    this->view->setMinimumWidth(1024);
    this->initViewSelect();
    this->initAlgorithmRadio();
    this->Layout->addWidget(view, 1,1);
    this->Layout->addWidget(imageSelect, 2,1);
    this->Layout->addWidget(sideWidget, 1,2);
    this->sideLayout->addWidget(overlayCheckbox,7,1);
    this->sideLayout->addWidget(Valve1Btn, 3,1);
    this->sideLayout->addWidget(Valve2Btn, 4,1);
    this->sideLayout->addWidget(Valve3Btn, 5,1);
    this->sideLayout->addWidget(cameraSettingsBtn, 6,1);
    this->sideLayout->addWidget(currentAlgorithmText, 1,1);
    this->sideLayout->addWidget(algorithm_radio_group, 2,1);
#ifdef USE_GPS
    this->gpswidget = new gpsWidget(gps);
    this->Layout->addWidget(gpswidget, 1,3);
#endif
    this->sideWidget->setLayout(this->sideLayout);
    this->globalWidget->setLayout(this->Layout);
    setCentralWidget(this->globalWidget);
    connect(this->overlayCheckbox,SIGNAL(stateChanged(int)), this, SLOT(checkboxToggled(int)));
    connect(this->Valve1Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve2Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve3Btn, SIGNAL(pressed()), this, SLOT(valveButtonMapper()));
    connect(this->Valve1Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(this->Valve2Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(this->Valve3Btn, SIGNAL(released()), this, SLOT(valveButtonMapper()));
    connect(cameraSettingsBtn, SIGNAL(pressed()), camera, SLOT(showCameraSettings()));
}

void VisionSpray::initViewSelect(void )
{
    this->imageSelect->addItem("Input");
    this->imageSelect->addItem("Vignette Corrected");
    this->imageSelect->addItem("Color image");
    this->imageSelect->addItem("Excess Green");
    modicovi_prefix = "Modicovi:";
    this->modicovi_threshold1->initViewSelect(this->imageSelect, modicovi_prefix);
    connect(this->imageSelect, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentViewChanged(QString)));
}

void VisionSpray::checkboxToggled(int state)
{
  switch (state){
    case Qt::Checked:
      connect(&m_sprayplanner,SIGNAL(sprayNozzleMap(cv::Mat_<uint8_t>, qint64)),view,SLOT(updateOverlayBuffer(cv::Mat_<uint8_t>,qint64)));
      break;
    case Qt::Unchecked:
      disconnect(&m_sprayplanner,SIGNAL(sprayNozzleMap(cv::Mat_<uint8_t>, qint64)),view,SLOT(updateOverlayBuffer(cv::Mat_<uint8_t>,qint64)));
      break;
  }
}


void VisionSpray::currentViewChanged(const QString& text)
{
  static QObject * lastSender = 0;
  std::cout << "Received new view " << text.toLocal8Bit().data() << std::endl;
  if(lastSender!=0)
    disconnect(lastSender,0,this->view, 0);
  if(text.compare("Input")==0)
  {
    lastSender = this->camera;
    connect(this->camera, SIGNAL(newBayerGRImage(cv::Mat,qint64)), this->view, SLOT(showImage(cv::Mat,qint64)));
  }
  if(text.compare("Vignette Corrected")==0)
  {
    lastSender = this->camera;
    connect(this->camera, SIGNAL(vignettingCorrectedInImage(cv::Mat,qint64)), this->view, SLOT(showImage(cv::Mat,qint64)));
  }
  if(text.compare("Color image")==0)
  {
    lastSender = &(this->dem);
    connect(this->camera, SIGNAL(vignettingCorrectedInImage(cv::Mat,qint64)), &(this->dem), SLOT(newBayerGRImage(cv::Mat,qint64)));
    connect(&(this->dem), SIGNAL(newImage(cv::Mat, qint64)), this->view, SLOT(showImage(cv::Mat,qint64)));
  }
  if(text.compare("Excess Green")==0)
  {
    lastSender = &(this->exg);
    connect(&(this->exg), SIGNAL(newImage(cv::Mat,qint64)), this->view, SLOT(showImage(cv::Mat,qint64)));
  }
  else if(text.startsWith(modicovi_prefix))
  {
    lastSender = this->modicovi_threshold1;
    QString id = text.midRef(modicovi_prefix.length()).toString();
    this->modicovi_threshold1->currentViewChanged(id, this->view);
  }
}

void VisionSpray::initParcelReceiver()
{
  connect(&(this->armadillo), SIGNAL(parcelReceiver(int)), this, SLOT(parcelNrReceiver(int)));
}


VisionSpray::~VisionSpray()
{}

#include "VisionSpray.moc"

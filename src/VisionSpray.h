#ifndef VisionSpray_H
#define VisionSpray_H


#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QComboBox>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QButtonGroup>
#include <QtGui/QRadioButton>
#include <QDir>    
#include <rowdetect.h>
#include <QSettings>

#include "../include/qOpenGLCVWidget/qOpenGLCVWidget.h"
#include "demosaic_cv.h"
#include "armadillointerface.h"
#include "../include/BayerExG/exg_cv.h"
#include "exg_cv.h"
#ifdef USE_GPS
  #include "../include/gpsReader/gpsreader.h"
  #include "../include/qtgpscWidget/gpswidget.h"
  #include "gpsreader.h"
  #include "../include/qtgpscWidget/satview.h"
  #include "../include/qtgpscWidget/sattellite.h"
#endif

#ifdef USE_CAMERA
  #pragma message "Using a camera"
  #include "../include/QtGigE/qtgige.h"
#else
  #pragma message "Not using a camera"
#endif

#ifdef USE_DATALOGGER
  #include "datalogger.h"
#endif

class VisionSpray : public QMainWindow
{
Q_OBJECT
public:
    VisionSpray();
    virtual ~VisionSpray();
#ifdef USE_CAMERA
    QTGIGE * camera;
#endif
private:
    armadilloInterface armadillo;
    QPushButton * Valve1Btn;
    QPushButton * Valve2Btn;
    QPushButton * cameraSettingsBtn;
    CQtOpenCVViewerGl * view;
    void drawGui(void);
    QGridLayout *Layout;
    QGridLayout *sideLayout;
    QWidget *globalWidget;
    QWidget *sideWidget;
    QComboBox *imageSelect;
    QLabel * modicoviText;
    demosaic_cv dem;
    ExG_cv exg;
    QSettings settings;
    RowDetect m_rowDetect;
#ifndef USE_CAMERA
    void init_CameraSimulator(void);
    QFileInfoList * simulationFiles;
    int fileptr;
#endif
    
#ifdef USE_DATALOGGER
    dataLogger * log;
#endif
    
#ifdef USE_GPS
    void loadGPS(void);
    gpsReader * gps;
    gpsWidget *gpswidget;
#endif
private slots:
    void turnValve1On(void);
    void turnValve1Off(void);
    void turnValve2On(void);
    void turnValve2Off(void);
    void currentViewChanged ( const QString & text );
#ifndef USE_CAMERA
    void cameraSimulator(void);
#endif
#ifdef USE_GPS
    void updateSatlist(SatList sats);
    void updateSatStatus(QByteArray time, QByteArray latitude, char latitudeHeading, 
              QByteArray longitude, char longitudeHeading, int GPSQuality, 
              int sattelitesInView, float horizontalDilution, float altitude,
              char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit,
              float dGPSAge, int dGPSStation);
    void VTGReceiver(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator,
	      QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator,
	      QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit,
	      float GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit);
signals:
    void velocity(float kmh);
#endif
#ifndef USE_CAMERA
signals:
    void newSimulatedImage( cv::Mat *image );
#endif
};

#endif // VisionSpray_H

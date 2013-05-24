#ifndef VisionSpray_H
#define VisionSpray_H


#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QComboBox>
#include <QWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QDir>    
#include "../include/RowDetect/rowdetect.h"
#include <QSettings>

#include "../include/qOpenGLCVWidget/qOpenGLCVWidget.h"
#include "demosaic_cv.h"
#include "armadillointerface.h"
#include "nozzlecontrol.h"
#include "../include/RowDetect/greendetect.h"
#include "../include/SprayTimeKeeper/spraytimekeeper.h"
#include "../include/SprayPlanner/sprayplanner.h"

#include "../include/BayerExG/exg_cv.h"
#ifdef USE_GPS
  #include "../include/gpsReader/gpsreader.h"
  #include "../include/qtgpscWidget/gpswidget.h"
#endif

#include "../include/QtGigE/qtgige.h"

#ifdef USE_DATALOGGER
  #include "datalogger.h"
#endif

class VisionSpray : public QMainWindow
{
Q_OBJECT
public:
    VisionSpray();
    virtual ~VisionSpray();
    QTGIGE * camera;
private:
    armadilloInterface armadillo;
    QPushButton * Valve1Btn;
    QPushButton * Valve2Btn;
    QPushButton * Valve3Btn;
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
    NozzleControl nz;
    RowDetect m_rowDetect;
    SprayTimeKeeper * spraytimekeeper;
    GreenDetect m_greendetect;
    SprayPlanner m_sprayplanner;
    
#ifdef USE_GPS
    void loadGPS(void);
    gpsReader * gps;
    gpsWidget *gpswidget;
#endif
private slots:
    void currentViewChanged ( const QString & text );
    void valveButtonMapper();
};

#endif // VisionSpray_H

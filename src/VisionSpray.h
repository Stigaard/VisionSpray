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
#include <QSettings>

#include "../include/qOpenGLCVWidget/qOpenGLCVWidget.h"
#include "demosaic_cv.h"
#include "armadillointerface.h"
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
    
#ifdef USE_GPS
    void loadGPS(void);
    gpsReader * gps;
    gpsWidget *gpswidget;
#endif
private slots:
    void currentViewChanged ( const QString & text );
};

#endif // VisionSpray_H

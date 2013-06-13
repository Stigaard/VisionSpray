#ifndef VisionSpray_H
#define VisionSpray_H


#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QWidget>
#include <QLabel>
#include <QButtonGroup>
#include <QCheckBox>
#include <QRadioButton>
#include <QDir>    
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_series_data.h>
#include "../include/RowDetect/rowdetect.h"
#include <QSettings>
#include <QQueue>
#include <QTimer>
#include <QGroupBox>

#include "../include/qOpenGLCVWidget/qOpenGLCVWidget.h"
#include "demosaic_cv.h"
#include "armadillointerface.h"
#include "nozzlecontrol.h"
#include "../include/RowDetect/greendetect.h"
#include "../include/SprayTimeKeeper/spraytimekeeper.h"
#include "../include/SprayPlanner/sprayplanner.h"
#include "../include/velocity_filter/velocityfilter.h"

#include "../include/BayerExG/exg_cv.h"
#include "../include/PresenningExG/presenningExg.h"
#include "../include/modicovi-rt/modicovi_rt.h"
#include "TreatmentDatabase.h"


#ifdef USE_GPS
  #include "../include/gpsReader/gpsreader.h"
  #include "../include/qtgpscWidget/gpswidget.h"
#endif

#include "../include/QtGigE/qtgige.h"

#include "../include/LoggerModule/imagelogger.h"

#ifdef USE_DATALOGGER
  #include "../include/LoggerModule/loggermodule.h"
#endif

#include "always.h"

class VisionSpray : public QMainWindow
{
Q_OBJECT
public:
    VisionSpray();
    virtual ~VisionSpray();
    QTGIGE * camera;
private:
    void initPlots(void);
    armadilloInterface armadillo;
    TreatmentDatabase * tdb;
    QPushButton * Valve1Btn;
    QPushButton * Valve2Btn;
    QPushButton * Valve3Btn;
    QPushButton * cameraSettingsBtn;
    QCheckBox * overlayCheckbox;
    CQtOpenCVViewerGl * view;
    modicovi_rt * modicovi_threshold1;
    modicovi_rt * modicovi_threshold2;
    modicovi_rt * modicovi_threshold3;
    modicovi_rt * modicovi_threshold4;
    modicovi_rt * modicovi_threshold5;
    void drawGui(void);
    QGridLayout *Layout;
    QGridLayout *sideLayout;
    QWidget *globalWidget;
    QWidget *sideWidget;
    QComboBox *imageSelect;
    QLabel * currentAlgorithmText;
    demosaic_cv dem;
    ExG_cv exg;
    //presenningExg exg;
    QSettings settings;
    NozzleControl nz;
    RowDetect* m_rowDetect1;
    RowDetect* m_rowDetect2;
    RowDetect* m_rowDetect3;
    RowDetect* m_rowDetect4;
    RowDetect* m_rowDetect5;
    SprayTimeKeeper * spraytimekeeper;
    GreenDetect m_greendetect;
    SprayPlanner m_sprayplanner;
    velocityFilter m_velocityfilter;
    Always m_always;
    QRadioButton * algortihm_never_spray_radio;
    QRadioButton * algortihm_always_spray_radio;
    QRadioButton * algorithm_modicovi_threshold1;
    QRadioButton * algorithm_modicovi_threshold2;
    QRadioButton * algorithm_modicovi_threshold3;
    QRadioButton * algorithm_modicovi_threshold4;
    QRadioButton * algorithm_modicovi_threshold5;
    QRadioButton * algorithm_rule_of_thumb_threshold1;
    QRadioButton * algorithm_rule_of_thumb_threshold2;
    QRadioButton * algorithm_rule_of_thumb_threshold3;
    QRadioButton * algorithm_rule_of_thumb_threshold4;
    QRadioButton * algorithm_rule_of_thumb_threshold5;
    QGroupBox * algorithm_radio_group;
    QVBoxLayout * algorithm_layout;
    void initAlgorithmRadio(void);
    void changeAlgorithm(TreatmentType treatment);
    void initCamera(void);
    void initGPS(void);
    void initConfigFile(void);
    void initDatalogger(void);
    void initModicovi(void);
    void initRowDetect(void);
    void initSprayPlanner(void);
    void initSprayTimeKeeper(void);
    void initTreatmentDatabase(void);
    void initOpenCV(void);
    
    QSignalMapper * algorithm_checkbox_mapper;
    
#ifdef USE_DATALOGGER
    ImageLogger * imageLog;
    LoggerModule * velocityLogger;
#endif
#ifdef USE_GPS
    gpsReader * gps;
    gpsWidget *gpswidget;
#endif
    QQueue<double> velocityTimes;
    QQueue<double> filtVelocities;
    QQueue<double> rawVelocities;
    QTimer * plotTimer;
    QwtPlot * m_VelPlot;
    QwtPlotCurve* m_filtVelCurve;
    QwtPlotCurve* m_rawVelCurve;
    void initViewSelect(void);
    QString modicovi_prefix;
private slots:
    void currentViewChanged ( const QString & text );
    void valveButtonMapper();
    void velocityEcho(float v);
    void velocityLog(float v);
    void rawVelPlot(float v);
    void filtVelPlot(float v);
    void updatePlots(void);
    void checkboxToggled(int);
    void parcelNrReceiver(int parcel);
    void algorithmRadioCommand(int cmd);
};

#endif // VisionSpray_H

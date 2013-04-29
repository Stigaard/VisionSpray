/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <qt4/QtCore/QThread>
#include <opencv2/opencv.hpp>
#include <qt4/QtCore/QFile>
#include <qt4/QtCore/QDir>
#include <qt4/QtCore/QTextStream>
#include <deque>

class dataLogger : public QThread
{
    Q_OBJECT
public:
    dataLogger(const QString pathToLog, const QString lognamePostString);
    void run();
public slots:
#ifdef USE_GPS
    void NMEALogger(QByteArray talker, QByteArray command, QList<QByteArray> arg);
    void GGALogger(QByteArray time, QByteArray latitude, char latitudeHeading,
                   QByteArray longitude, char longitudeHeading, int GPSQuality,
                   int sattelitesInView, float horizontalDilution, float altitude,
                   char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit,
                   float dGPSAge, int dGPSStation);
#endif
#ifdef USE_CAMERA
    void rawImageLogger(void* img);
    void pngImageLoggerCameraOne(cv::Mat image);
    void pngImageLoggerCameraTwo(cv::Mat image);
    void pngImageLogger(cv::Mat image);
    void pngImageLogger(cv::Mat image, QString cameraName);
    void burstImageLogger(cv::Mat image);
    void saveImageBurst(void);
#endif

    void valve1Logger(int decision);
    void valve2Logger(int decision);
    void weedPressureLogger(float weedpressure);
    void weedAmountLogger(float weedAmount);
    void runtimeLogger(qint64 ExcessGreenTime, qint64 thresholdtime, qint64 morphtime,
                       qint64 edgeResponseTime, qint64 thresholdEdgeStrengthTime,
                       qint64 reduceWidthOfEdgesTime, qint64 locateEdgePointsTime,
                       qint64 makeRelativeCoordinatesTime, qint64 calculateGaussianFeaturesTime,
                       qint64 estimateWeedPressureTime, qint64 totaltime);
private slots:
    void flushLogs(void);
private:
    bool burstLoggerIsActive;
    QDir * logdir;
    QFile * Valve1File;
    QTextStream * Valve1Stream;
    QFile * Valve2File;
    QTextStream * Valve2Stream;
    QFile * weedPressureFile;
    QTextStream * weedPressureStream;
    QFile * weedAmountFile;
    QTextStream * weedAmountStream;
    QFile * runtimeLoggerFile;
    QTextStream * runtimeLoggerStream;

    void initWeedPressure(void);
    void initWeedAmount(void);
    void initValve(void);
    void initRuntime(void);

#ifdef USE_CAMERA
    void initCamera(void);
    QDir * rawImageDir;
    QDir * pngImageDir;
    std::deque<cv::Mat> listOfImages;
    std::deque<uint64> listOfTimeStamps;
#endif

#ifdef USE_GPS
    void initGPS(void);
    QFile * NMEAFile;
    QTextStream * NMEAStream;
    QFile * GGAFile;
    QTextStream * GGAStream;
#endif
};

#endif // DATALOGGER_H

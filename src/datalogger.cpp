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


#include "datalogger.h"
#include "datalogger.moc"
#include <qt4/QtCore/QDateTime>
#include <qt4/QtCore/QString>
#include <qt4/QtCore/QTimer>

#include <iostream>
#include <vector>

dataLogger::dataLogger(const QString pathToLog, const QString lognamePostString)
{
    //Generate log folder name
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    logdir = new QDir(pathToLog);
    logdir->mkdir(dateTimeString + lognamePostString);
    logdir->cd(dateTimeString + lognamePostString);
    // Write directory where the log files are stored.
    std::cout << logdir->absolutePath().toStdString() << std::endl;

#ifdef USE_CAMERA
    this->initCamera();
#endif
#ifdef USE_GPS
    this->initGPS();
#endif
    this->initValve();
    this->initWeedPressure();
    this->initWeedAmount();
    this->initRuntime();
    
    burstLoggerIsActive = true;
}

void dataLogger::run()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flushLogs()));
    timer->start(5000);
    QThread::run();
}

void dataLogger::flushLogs(void )
{
    this->Valve1File->flush();
    this->Valve2File->flush();
    this->weedAmountFile->flush();
    this->weedPressureFile->flush();
    this->runtimeLoggerFile->flush();
#ifdef USE_GPS
    NMEAFile->flush();
    GGAFile->flush();
#endif
}

void dataLogger::initWeedPressure(void )
{
    weedPressureFile = new QFile(logdir->filePath("weedPressure.csv"));
    if (!weedPressureFile->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", weedPressureFile->fileName().toLocal8Bit().constData());
    }
    weedPressureStream = new QTextStream(weedPressureFile);
}

void dataLogger::initRuntime(void )
{
    runtimeLoggerFile = new QFile(logdir->filePath("runtime.csv"));
    if (!runtimeLoggerFile->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", runtimeLoggerFile->fileName().toLocal8Bit().constData());
    }
    runtimeLoggerStream = new QTextStream(runtimeLoggerFile);
    *runtimeLoggerStream << "EpochTime" << "," << "EXGTime" << "," << "ThresholdTime" << "," << "MorphologyTime"
                         << "," << "CalculateEdgeResponseTime" << "," << "ThresholdEdgeResponseTime"
                         << "," << "ReduceWidthOfEdgesTime" << "," << "LocateEdgePointsTime"
                         << "," << "MakeRelativeCoordinatesTime" << "," << "CalculateGaussianFeaturesTime"
                         << "," << "EstimateWeedPressureTime" << "," << "TotalTime" << endl;
}



void dataLogger::initWeedAmount(void )
{
    weedAmountFile = new QFile(logdir->filePath("weedAmount.csv"));
    if (!weedAmountFile->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", weedAmountFile->fileName().toLocal8Bit().constData());
    }
    weedAmountStream = new QTextStream(weedAmountFile);
}

void dataLogger::runtimeLogger(qint64 ExcessGreenTime, qint64 thresholdtime, qint64 morphtime,
                               qint64 edgeResponseTime, qint64 thresholdEdgeStrengthTime,
                               qint64 reduceWidthOfEdgesTime, qint64 locateEdgePointsTime,
                               qint64 makeRelativeCoordinatesTime, qint64 calculateGaussianFeaturesTime,
                               qint64 estimateWeedPressureTime, qint64 totaltime)
{
    *runtimeLoggerStream << QString::number(QDateTime::currentMSecsSinceEpoch()) + "," + QString::number(ExcessGreenTime)
                         + "," + QString::number(thresholdtime) + "," + QString::number(morphtime)
                         + "," + QString::number(edgeResponseTime) + "," + QString::number(thresholdEdgeStrengthTime)
                         + "," + QString::number(reduceWidthOfEdgesTime) + "," + QString::number(locateEdgePointsTime)
                         + "," + QString::number(makeRelativeCoordinatesTime) + "," + QString::number(calculateGaussianFeaturesTime)
                         + "," + QString::number(estimateWeedPressureTime) + "," + QString::number(totaltime) << endl;
}

void dataLogger::weedAmountLogger(float weedAmount)
{
    *weedAmountStream << QString::number(QDateTime::currentMSecsSinceEpoch()) + "," + QString::number(weedAmount) << endl;
}


void dataLogger::weedPressureLogger(float weedpressure)
{
    *weedPressureStream << QString::number(QDateTime::currentMSecsSinceEpoch()) + "," + QString::number(weedpressure) << endl;
}


void dataLogger::initValve(void )
{
    Valve1File = new QFile(logdir->filePath("valve1.csv"));
    if (!Valve1File->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", Valve1File->fileName().toLocal8Bit().constData());
    }
    Valve1Stream = new QTextStream(Valve1File);

    Valve2File = new QFile(logdir->filePath("valve2.csv"));
    if (!Valve2File->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", Valve2File->fileName().toLocal8Bit().constData());
    }
    Valve2Stream = new QTextStream(Valve2File);
}

void dataLogger::valve1Logger(int decision)
{
    *Valve1Stream << QString::number(QDateTime::currentMSecsSinceEpoch()) << "," << QString::number(decision) << endl;
}

void dataLogger::valve2Logger(int decision)
{
    *Valve2Stream << QString::number(QDateTime::currentMSecsSinceEpoch()) << "," << QString::number(decision) << endl;
}

#ifdef USE_CAMERA
void dataLogger::initCamera(void )
{
    rawImageDir = new QDir(*logdir);
    rawImageDir->mkdir("Raw_Images");
    rawImageDir->cd("Raw_Images");

    pngImageDir = new QDir(*logdir);
    pngImageDir->mkdir("png_Images");
    pngImageDir->cd("png_Images");
}


void dataLogger::pngImageLoggerCameraOne(cv::Mat image)
{
    std::cout << "Image from camera one" << std::endl;
    pngImageLogger(image, "cameraOne");
}

void dataLogger::pngImageLoggerCameraTwo(cv::Mat image)
{
    std::cout << "Image from camera two" << std::endl;
    pngImageLogger(image, "cameraTwo");
}

void dataLogger::pngImageLogger(cv::Mat image)
{
  pngImageLogger(image, "");
}

void dataLogger::pngImageLogger(cv::Mat image, QString cameraName)
{
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    QString targetFilename = QString::number(QDateTime::currentMSecsSinceEpoch()) + cameraName + ".png";

    cv::imwrite(pngImageDir->filePath(targetFilename).toStdString(), image, compression_params);
}

void dataLogger::burstImageLogger(cv::Mat image)
{
    int maxNumberOfImages = 100;
    if(!burstLoggerIsActive)
	return;
    listOfImages.push_back(image);
    uint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    listOfTimeStamps.push_back(currentTime);
    if(listOfImages.size() > maxNumberOfImages)
    {
	listOfImages.pop_front();
	listOfTimeStamps.pop_front();
    }
}


void dataLogger::saveImageBurst(void)
{
    burstLoggerIsActive = false;
    int count = 0;
    while (!listOfImages.empty())
    {
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	QString targetFilename = QString::number(listOfTimeStamps.front()) + ".png";
	count++;
	std::string pathToFile = pngImageDir->filePath(targetFilename).toStdString();
	cv::imwrite(pathToFile, listOfImages.front(), compression_params);
	listOfImages.pop_front();
	listOfTimeStamps.pop_front();
    }
    burstLoggerIsActive = true;
}

void dataLogger::rawImageLogger(void* img)
{
    QFile imageFile(rawImageDir->filePath(QString::number(QDateTime::currentMSecsSinceEpoch()) + ".raw"));
    if (!imageFile.open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", imageFile.fileName().toLocal8Bit().constData());
    }
    imageFile.write((char*)img, (1024*768*3)/2);
    imageFile.close();
}
#endif

#ifdef USE_GPS

void dataLogger::initGPS(void )
{
    NMEAFile = new QFile(logdir->filePath("NMEA.txt"));
    if (!NMEAFile->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", NMEAFile->fileName().toLocal8Bit().constData());
    }
    NMEAStream = new QTextStream(NMEAFile);

    GGAFile = new QFile(logdir->filePath("GGA.csv"));
    if (!GGAFile->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot create the file %s", GGAFile->fileName().toLocal8Bit().constData());
    }
    GGAStream = new QTextStream(GGAFile);
}


void dataLogger::NMEALogger(QByteArray talker, QByteArray command, QList< QByteArray > arg)
{
//   *NMEAStream << QDateTime::currentMSecsSinceEpoch() + "," + talker + command;
//    for (int i = 0; i < arg.size(); ++i) {
//      *NMEAStream << "," + arg.at(i);
//     }
//      *NMEAStream << endl;

}

void dataLogger::GGALogger(QByteArray time, QByteArray latitude, char latitudeHeading,
                           QByteArray longitude, char longitudeHeading, int GPSQuality,
                           int sattelitesInView, float horizontalDilution, float altitude,
                           char altitudeUnit, QByteArray geoidalSeperation,
                           char geoidalSeperationUnit, float dGPSAge, int dGPSStation)
{
    *GGAStream << QString::number(QDateTime::currentMSecsSinceEpoch()) + "," + time + "," + latitude;
    *GGAStream <<  "," + QString(latitudeHeading) + "," + QString(longitude) + "," + QString(longitudeHeading);
    *GGAStream <<  "," + QString::number(GPSQuality) + "," + QString::number(sattelitesInView) + "," + QString::number(horizontalDilution);
    *GGAStream <<  "," + QString::number(altitude) + "," + altitudeUnit + "," + QString(geoidalSeperation);
    *GGAStream <<  "," + QString(geoidalSeperationUnit) + "," + QString::number(dGPSAge) + "," + QString::number(dGPSStation) << endl;
}

#endif
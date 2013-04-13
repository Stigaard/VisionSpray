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


#include "loggermodule.h"
#include "loggermodule.moc"
#include <qt4/QtCore/QDateTime>
#include <qt4/QtCore/QString>
#include <qt4/QtCore/QTimer>

#include <iostream>
#include <vector>

bool LoggerModule::isAtLeastOneLoggerModuleInitialized = false;
QDir * LoggerModule::baseDirectory = 0;

LoggerModule::LoggerModule(const QString pathToLog, const QString lognamePostString)
{
    loggerIsActive = true;
    ensureThatBaseDirectoryExists(pathToLog, "");
    createSubLogDir(lognamePostString);
}

void LoggerModule::ensureThatBaseDirectoryExists(const QString pathToLog, const QString lognamePostString)
{
    if(!isAtLeastOneLoggerModuleInitialized)
    {
        isAtLeastOneLoggerModuleInitialized = true;
        createBaseLogDirectory(pathToLog, lognamePostString);
    }
}

void LoggerModule::createBaseLogDirectory(const QString pathToLog, const QString lognamePostString)
{
    //Generate log folder name
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    QDir * templogdir = new QDir(pathToLog);
    // TODO: Insert assertion that the directory is valid.
    templogdir->mkdir(dateTimeString + lognamePostString);
    templogdir->cd(dateTimeString + lognamePostString);
    // Write directory where the log files are stored.
    std::cout << "Log files can be found in this directory." << std::endl;
    std::cout << templogdir->absolutePath().toStdString() << std::endl;

    baseDirectory = new QDir(templogdir->absolutePath());
}

void LoggerModule::createSubLogDir(const QString subDirName)
{
    logdir = new QDir(*baseDirectory);
    logdir->mkdir(subDirName);
    logdir->cd(subDirName);
}

void LoggerModule::run()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flushLogs()));
    timer->start(5000);
    QThread::run();
}

void LoggerModule::activate()
{
    loggerIsActive = true;
}

void LoggerModule::deactivate()
{
    loggerIsActive = false;
}

void LoggerModule::flushLogs(void )
{
//    this->Valve1File->flush();
}

void LoggerModule::log(const QString nameOfValue, int value)
{
    if(loggerIsActive)
    {
	qint64 epochTimeStamp = QDateTime::currentMSecsSinceEpoch();
	
	// TODO: Avoid to open and close files at each logging event.
	QFile targetFile(logdir->filePath(nameOfValue + ".csv"));
	if (!targetFile.open(QIODevice::Append))
	{
	    qWarning("Cannot create the file %s", targetFile.fileName().toLocal8Bit().constData());
	}
	QTextStream targetStream(&targetFile);
	targetStream << epochTimeStamp << " " << value << endl;
	targetFile.close();
    }
}

void LoggerModule::log(const QString nameOfValue, char* value)
{
    if(loggerIsActive)
    {
	qint64 epochTimeStamp = QDateTime::currentMSecsSinceEpoch();
	
	// TODO: Avoid to open and close files at each logging event.
	QFile targetFile(logdir->filePath(nameOfValue + ".csv"));
	if (!targetFile.open(QIODevice::Append))
	{
	    qWarning("Cannot create the file %s", targetFile.fileName().toLocal8Bit().constData());
	}
	QTextStream targetStream(&targetFile);
	targetStream << epochTimeStamp << " " << value << endl;
	targetFile.close();
    }
}


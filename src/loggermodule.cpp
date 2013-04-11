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

LoggerModule::LoggerModule(const QString pathToLog, const QString lognamePostString)
{
    //Generate log folder name
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
    logdir = new QDir(pathToLog);
    logdir->mkdir(dateTimeString + lognamePostString);
    logdir->cd(dateTimeString + lognamePostString);
    // Write directory where the log files are stored.
    std::cout << logdir->absolutePath().toStdString() << std::endl;
}

void LoggerModule::run()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flushLogs()));
    timer->start(5000);
    QThread::run();
}

void LoggerModule::flushLogs(void )
{
//    this->Valve1File->flush();
}

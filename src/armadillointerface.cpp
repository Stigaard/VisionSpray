/*
    Copyright (c) 2013, Morten S. Laursen <email>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Morten S. Laursen <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Morten S. Laursen <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QVariant>
#include "armadillointerface.h"
#include "armadillointerface.moc"


armadilloInterface::armadilloInterface(QObject* parent): QThread(parent)
{
  this->start();
}

void armadilloInterface::run()
{
#ifndef EMULATE_ARMADILLO
    QString rosBridgeAdr;
    quint32 rosBridgePort;      
    if(settings.contains("ROS/BridgeAdr"))
    {
      rosBridgeAdr = settings.value("ROS/BridgeAdr").toString();
    }
    else
    {
      rosBridgeAdr = "192.168.1.129";
      settings.setValue("ROS/BridgeAdr", rosBridgeAdr);
    }
    if(settings.contains("ROS/BridgePort"))
    {
      rosBridgePort = settings.value("ROS/BridgePort").toInt();
    }
    else
    {
      rosBridgePort = 9090;
      settings.setValue("ROS/BridgePort", rosBridgePort);
    }
    QHostAddress rosAdr(rosBridgeAdr);
    ros = new qtRosBridge(rosAdr, rosBridgePort);
    connect(ros, SIGNAL(newMsg(QVariant)), this, SLOT(msgFromRosReceiver(QVariant)));
    ros->subscribe("/fmKnowledge/encoder_odom", "nav_msgs/Odometry", 500);  
    ros->subscribe("/fmKnowledge/polygon_map", "/fmLib/msgs/msg/IntStamped", 500);  
#else
    while(true)
    {
      float vel = 1;
      emit(forwardVelocity(vel));
      this->msleep(50);
    }
#endif
    exec();
}

void armadilloInterface::msgFromRosReceiver(QVariant msg)
{
  if(msg.toMap()["topic"].toString().compare("/fmKnowledge/encoder_odom")==0)
  {
    float vel = msg.toMap()["msg"].toMap()["twist"].toMap()["twist"].toMap()["linear"].toMap()["x"].toDouble();
//    qDebug() << "Velocity: " << vel;
    emit(forwardVelocity(vel));
  }
  
  if(msg.toMap()["topic"].toString().compare("/fmKnowledge/polygon_map")==0)
  {
    int parcel = msg.toMap()["msg"].toMap()["data"].toInt();
//    qDebug() << "Velocity: " << vel;
    emit(parcelReceiver(parcel));
  }
//  qDebug() << flush;
}

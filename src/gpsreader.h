/*
    Copyright (c) 2012, <copyright holder> <email>
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

    THIS SOFTWARE IS PROVIDED BY <copyright holder> <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL <copyright holder> <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef GPSREADER_H
#define GPSREADER_H

#include <QThread>
#include <QtCore/QByteArrayMatcher>
#include <qt4/QtCore/QByteArray>
#include <QMutex>
#include "../include/qextserialport/src/qextserialport.h"
#include "../include/qtgpscWidget/satellite.h"

class gpsReader : public QThread
{
  Q_OBJECT
public:
  enum GPS_STATE 
  {
        GPS_STATE_SearchSOS = 0,         // Search for start of sentence ($)
        GPS_STATE_RetrieveTalker_1,        // See http://gpsd.berlios.de/NMEA.html#_talker_ids
        GPS_STATE_RetrieveTalker_2,        //
        GPS_STATE_RetrieveAddress,       // retrieves the sentence type
        GPS_STATE_RetrieveData,          // Get data
        GPS_STATE_RetrieveChecksum_1,    // Get first checksum character
        GPS_STATE_RetrieveChecksum_2     // Get second checksum character
  };
  gpsReader(QObject* parent = 0);
  void run();
private:
  QextSerialPort * serial;
  QByteArray * buffer;
  GPS_STATE structureState;
  void stateMachine(QByteArray data);
//  void parseData(QByteArray data);
  uchar calcChecksum;     //Calculated checksum
  QByteArray receivedChecksum; //Received checksum
  QByteArray talker_id;
  QByteArray * address;
  QList<QByteArray> * data;
  QByteArray * dataWord;
  QByteArrayMatcher * isGGA;
  QMutex * stateMachineLock;
  SatList * satellites;
private slots:
  void newDataAvailable(void);
  void processNewNMEA(QByteArray talker, QByteArray command, QList<QByteArray> arg);
  void GGATest(QByteArray time, QByteArray latitude, char latitudeHeading, 
              QByteArray longitude, char longitudeHeading, int GPSQuality, 
              int sattelitesInView, float horizontalDilution, float altitude,
              char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit,
              float dGPSAge, int dGPSStation);
  void RMCTest(QByteArray time, char status, QByteArray latitude, char latitudeHeading, 
              QByteArray longitude, char longitudeHeading, float speedOverGround,
              float trackMadeGood, QByteArray date, float magneticVariation, 
              char magneticVariationUnit);
  void GSATest(char selMode, char mode, QList<int> satellites, float pDOP, float hDOP, float vDOP);
  void GSVCollector(int numMessages, int messageNum, int satsInView, QList<int> prn, QList<int> elevation, QList<int> azimuthh, QList<int> snr);
//   void VTG(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator,
// 	      QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator,
// 	      QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit,
// 	      float_t GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit);
signals:
  void newNMEASentence(QByteArray talker, QByteArray command, QList<QByteArray> arg);
  void newGGA(QByteArray time, QByteArray latitude, char latitudeHeading, 
              QByteArray longitude, char longitudeHeading, int GPSQuality, 
              int sattelitesInView, float horizontalDilution, float altitude,
              char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit,
              float dGPSAge, int dGPSStation);
  void newRMC(QByteArray time, char status, QByteArray latitude, char latitudeHeading, 
              QByteArray longitude, char longitudeHeading, float speedOverGround,
              float trackMadeGood, QByteArray date, float magneticVariation, 
              char magneticVariationUnit);
  void newGSA(char selMode, char mode, QList<int> satellites, float pDOP, float hDOP, float vDOP);
  void newSubGSV(int numMessages, int messageNum, int satsInView, QList<int> prn, QList<int> elevation, QList<int> azimuthh, QList<int> snr);
  void newVTG(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator,
	      QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator,
	      QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit,
	      float GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit);
  void satellitesUpdated(SatList sats);
};

#endif // GPSREADER_H

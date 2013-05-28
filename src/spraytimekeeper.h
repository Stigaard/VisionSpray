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


#ifndef SPRAYTIMEKEEPER_H
#define SPRAYTIMEKEEPER_H

#include <QThread>
#include <QMutex>
#include <QElapsedTimer>

class sprayTimeKeeper : public QThread
{
  Q_OBJECT
public:
  sprayTimeKeeper(void);
  int init_spray(int decision, int timeout);
  void update_spray(int decision, int key);
  int get_decision(int key);
virtual void run();
private:
  static const int numBuffers = 32;
  int decisions[numBuffers];
  qint64 times[numBuffers];
  int keyPointer;
  int timeoutPointer;
  QMutex keyLock;
  QElapsedTimer zero;
signals:
  void spray(int decision);
};

#endif // SPRAYTIMEKEEPER_H

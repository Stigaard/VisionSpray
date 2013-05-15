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

#include <iostream>
#include "nozzlecontrol.h"
#include "nozzlecontrol.moc"

NozzleControl::NozzleControl(QObject* parent): QObject(parent)
{
  this->loadSettings();
  this->adam = new QADAM(this->adamAdr);
  connect(this, SIGNAL(write_bit(quint8,bool)), this->adam, SLOT(write_bit(quint8,bool)), Qt::DirectConnection);
}

void NozzleControl::loadSettings(void )
{
     if(settings.contains("NozzleControl/nozzle0bit"))
      nozzle[0] = settings.value("NozzleControl/nozzle0bit").toUInt();
     else
     {
       nozzle[0] = 0;
       settings.setValue("NozzleControl/nozzle0bit", nozzle[0]);
     }
     
     if(settings.contains("NozzleControl/nozzle1bit"))
      nozzle[1] = settings.value("NozzleControl/nozzle1bit").toUInt();
     else
     {
       nozzle[1] = 1;
       settings.setValue("NozzleControl/nozzle1bit", nozzle[1]);
     }
     
     if(settings.contains("NozzleControl/nozzle2bit"))
      nozzle[2] = settings.value("NozzleControl/nozzle2bit").toUInt();
     else
     {
       nozzle[2] = 2;
       settings.setValue("NozzleControl/nozzle2bit", nozzle[2]);
     }
     
     if(settings.contains("NozzleControl/ADAM_IP"))
      this->adamAdr = settings.value("NozzleControl/ADAM_IP").toString();
     else
     {
       this->adamAdr = "10.0.0.1";
       settings.setValue("NozzleControl/ADAM_IP", this->adamAdr.toString());
     }
}

void NozzleControl::spray(quint8 nozzleID, bool state)
{
  if(nozzleID<3)
  {
    emit(write_bit(nozzle[nozzleID], state));
    emit(write_bit(nozzle[nozzleID], state));
  }
  else
    std::cerr << "Invalid nozzleID" << std::endl;
}

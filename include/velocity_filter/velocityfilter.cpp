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
#include "velocityfilter.h"
#include "velocityfilter.moc"

velocityFilter::velocityFilter(QObject* parent): QThread(parent)
{
  for(int i=0;i<MWSPT_NSEC;i++)
    section[i] = new SOS(DEN[i], NUM[i]);
}

void velocityFilter::velocitySlot(float vel)
{
  double x = vel;
  for(int i=0;i<(MWSPT_NSEC);i++)
  {
    x = section[i]->eval(x);
//    std::cout << "x[" << i << "]=" << x << "	";
  }
  emit(velocity(x));
}


velocityFilter::SOS::SOS(const double* a, const double* b)
{
  for(int i=0;i<3;i++)
    this->a[i] = a[i];
  for(int i=0;i<3;i++)
    this->b[i] = b[i];
  for(int i=0;i<3;i++)
    z[i] = 0;
}

double velocityFilter::SOS::eval(double x)
{
  double y;
  double w;
  
  //Update delay line
  for(int i=2;i>0;i--)
    z[i] = z[i-1];
  
  
  //Perform filtering
  //std::cout <<  "b[0]:"<< b[0] << "	b[1]:" << b[1] << "	b[2]:" << b[2] << "	a[1]:" << a[1] << "	a[2]:" << a[2] << std::endl;
  w = - z[1]*a[1] - z[2]*a[2] + x;
  z[0] = w;  
  y =  + b[0]*z[0] + b[1]*z[1] + b[2]*z[2];
  return y;
}


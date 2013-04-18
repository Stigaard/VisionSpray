/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Morten Stigaard Laursen <email>

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


#include "exg_cv.h"
#include "exg_cv.moc"

ExG_cv::ExG_cv(void ): QThread()
{
  abort = false;
  this->start();
}

ExG_cv::~ExG_cv()
{
  abort = true;
  this->msleep(300);
}

void ExG_cv::run()
{
  int i =0;
  while(abort==false)
  {
    bool cont = false;
    while(cont == false)
    {
      cont = semImg.tryAcquire(1,100);
      if(abort == true)
	return;
    }
    cv::Mat img = Imgs.dequeue();
    cv::Mat exg;
//    QString fname("Img_" + QString::number(i++) + ".png");
//    cv::imwrite(fname.toLocal8Bit().constData(), BayerGR8);
    BayerGR16ToExG(img, exg);
    emit(newImage(exg));    
  }
}

void ExG_cv::BayerGR16ToExG(cv::InputArray in, cv::OutputArray out)
{
#pragma message BayerGR16ToExG only outputs raw bayer as exg now needs rewrite
  cv::Mat tmp_out(in.getMat().size().height, in.getMat().size().width, cv::DataType<uint8_t>::type);
  uint8_t * out_ = tmp_out.ptr();
  uint16_t * _in = (uint16_t*)in.getMat().ptr();
  uint16_t h = in.getMat().size().height;
  uint16_t w = in.getMat().size().width;
  for(uint16_t y = 0; y < h-1; y+=2)
  {
    for(uint16_t x = 0; x < w-1; x+=2)
    {
 //     std::cout << "ptr:" << ptr << std::endl;
      int32_t r,g1,g2,b;
      g1 = _in[x+0+(y+0)*w];
      r  = _in[x+1+(y+0)*w];
      b  = _in[x+0+(y+1)*w];
      g2 = _in[x+1+(y+1)*w];
      int32_t exg = g1 + g2 - r -b;
      exg = (exg>>10)+128;
      
      out_[x+y*w] = exg;
    }
    for(uint16_t x = 1; x < w-1; x+=2)
    {
 //     std::cout << "ptr:" << ptr << std::endl;
      int32_t r,g1,g2,b;
      r  = _in[x+0+(y+0)*w];
      g1 = _in[x+1+(y+0)*w];
      g2 = _in[x+0+(y+1)*w];
      b  = _in[x+1+(y+1)*w];
      int32_t exg = g1 + g2 - r -b;
      exg = (exg>>10)+128;
      
      out_[x+y*w] = exg;
    }    
  }
  for(uint16_t y = 1; y < h-1; y+=2)
  {
    for(uint16_t x = 0; x < w-1; x+=2)
    {
 //     std::cout << "ptr:" << ptr << std::endl;
      int32_t r,g1,g2,b;
      b  = _in[x+0+(y+0)*w];
      g2 = _in[x+1+(y+0)*w];
      g1 = _in[x+0+(y+1)*w];
      r  = _in[x+1+(y+1)*w];
      int32_t exg = g1 + g2 - r -b;
      exg = (exg>>10)+128;
      
      out_[x+y*w] = exg;
    }
    for(uint16_t x = 1; x < w-1; x+=2)
    {
 //     std::cout << "ptr:" << ptr << std::endl;
      int32_t r,g1,g2,b;
      g2 = _in[x+0+(y+0)*w];
      b  = _in[x+1+(y+0)*w];
      r  = _in[x+0+(y+1)*w];
      g1 = _in[x+1+(y+1)*w];
      int32_t exg = g1 + g2 - r -b;
      exg = (exg>>10)+128;
      
      out_[x+y*w] = exg;
    }    
  }
  tmp_out.copyTo(out);
}

void ExG_cv::newBayerGRImage(cv::Mat img)
{
  Imgs.enqueue(img);
  semImg.release(1);
}

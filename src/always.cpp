/*************************************************************************
 * 
 * Author: thomas
 * File: always.cpp
 * Date: 12 June 2013
 * Description: This is an On/Off algorithm. It either says SPRAY or DONT SPRAY
 */

#include "always.h"
#include "always.moc"
#include <opencv2/core/core.hpp>

#ifdef USE_DATALOGGER
  #include "../include/LoggerModule/loggermodule.h"
  #include "../include/LoggerModule/imagelogger.h"
#endif


Always::Always(QObject *parent) :
    QObject(parent)
{
#ifdef USE_DATALOGGER  
    m_loggermodule = new ImageLogger("../Logging","Always");
#endif
}

void Always::spray(cv::Mat image, qint64 _t2)
{
  #ifdef USE_DATALOGGER  
    m_loggermodule->log("AlwaysSpray",1);
  #endif  
  emit analysisResult(cv::Mat_<uint8_t>::ones(1,1),_t2);
}

void Always::dontSpray(cv::Mat image, qint64 _t2)
{
  #ifdef USE_DATALOGGER  
    m_loggermodule->log("AlwaysDontSpray",0);
  #endif  
  emit analysisResult(cv::Mat_<uint8_t>::zeros(1,1),_t2);
}


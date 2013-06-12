/*************************************************************************
 * 
 * Author: thomas
 * File: always.h
 * Date: 23 May 2013
 */

#ifndef ALWAYS_H
#define ALWAYS_H

#include <QObject>
#include <opencv2/core/core.hpp>
#include "../include/LoggerModule/imagelogger.h"

class Always : public QObject
{
    Q_OBJECT
public:
    explicit Always(QObject *parent = 0);    
signals:
    void analysisResult(cv::Mat_<uint8_t> image, qint64 _t2);

public slots:
    void spray(cv::Mat image, qint64 _t2);
    void dontSpray(cv::Mat image, qint64 _t2);
private:
    ImageLogger * m_loggermodule; //NOTE no memory control
};

#endif // ALWAYS_H

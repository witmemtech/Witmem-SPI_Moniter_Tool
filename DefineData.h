/******************************************************************************************
**  Copyright 2023 Hangzhou Zhicun (Witmem) Technology Co., Ltd.  All rights reserved.   **
**                                                                                       **
**  This program is free software: you can redistribute it and/or modify                 **
**  it under the terms of the GNU General Public License as published by                 **
**  the Free Software Foundation, either version 3 of the License, or                    **
**  (at your option) any later version.                                                  **
**                                                                                       **
**  This program is distributed in the hope that it will be useful,                      **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of                       **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                        **
**  GNU General Public License for more details.                                         **
**                                                                                       **
**  You should have received a copy of the GNU General Public License                    **
**  along with this program.  If not, see http://www.gnu.org/licenses/.                  **
*******************************************************************************************/

#ifndef DEFINEDATA_H
#define DEFINEDATA_H
#include <QVector>
#include <QMap>
#include <QCheckBox>

#define YAXIS_MAX_RANG 70000
#define FLOAT_YAXIS_MIN_RANG 0
#define FLOAT_YAXIS_MAX_RANG 1
#define YAXIS_MIN_RANG 0
#define LOG_YAXIS_MIN_RANG -200
#define LOG_YAXIS_MAX_RANG 0
#define LOG__MATH_YAXIS_MAX_RANG 200
#define LOG_BASE_VALUE -200
#define MATH_NUM 1000


#define SPI_DEVICE_SCAN_FAILE -1
#define SPI_DEVICE_OPEN_FAILE -2
#define SPI_DEVICE_GET_INFO_FAILE -3
#define SPI_DEVICE_INIT_FAILE -4
#define SPI_DEVICE_SLAVE_READ_FAILE -5
#define SPI_DEVICE_OPEN_SUCCESS 0

typedef enum _ScaleType{
    ScaleType_Linear,
    ScaleType_Logarithmic

}ENUM_ScaleType;

enum SAVEFILETYPE{
    WAV_FILE,
    EXCEL_FILE
};

enum ARRAYXASIXTYPE{
    POINT_TYPE = 0,
    TIME_TYPE
};

enum SENDDATATYPE{
    PCM,
    FFT,
    NONE,
    ARRAY,
};
enum UITYPE{
    FrequencyDomain,
    TimeDomain,
    ArrayType
};
typedef enum _BitType{
    BIT_16,
    BIT_24,
    BIT_32,
    BIT_float
}ENUM_BitType;

typedef enum DeviceStatus{
    Device_Offline = 0,
    Device_Wait_Stare,
    Device_Transmitting
}ENUM_DeviceStatus;

struct FrequencyData
{
    double dSamplingRate = 16000;
    double dSampleSize = 512;
    int nDataType = 1;
    int nTransmissionOrder = 0;
    FrequencyData() {}
};
struct TimeDomainData
{
    double dSamplingRate = 16000;
    double dSampleSize = 128;
    int nTransmissionBit = 1;
    int nAudioTrackNum = 10;
    QString  strSampleType = "int16";
    double dFrameLength = 4.0;
    TimeDomainData() {}
};

struct ArrayDomainData
{
    double dArrayMembersNum= 64;
    int nDataType = 1;
    int nXAsixType = POINT_TYPE;
    int nPointCountTime = 100;
    QList<QCheckBox*>chickedList;
    ArrayDomainData() {}
};
struct cfgUIData
{
    bool isExportData =  false;
    int nAxisType = 0;
    int nMathYAisType = 1;
    int nG1G2YAisType = 1;
    int nAvgNum = 1;
    int nChannelMath = 0;
    double dbNum = 1;
    double dbMatnNum = 1;
};
extern QMap<int, QString> dataFreTypeDesc;
extern QMap<int, QString> dataTimeTypeDesc;
extern QMap<int, QString> dataArrayTypeDesc;
extern QMap<int, QString> SpiErrorDesc;
extern QMap<int, QString> DeviceStatusDesc;
extern QMap<QString, unsigned int> sampleNumMap;
extern QMap<QString,double> LogNumMap;
extern QMap<int,QString> TransmissionOrderMap;
extern QMap<int,QString> ArrayTypeMap;

#endif // DEFINEDATA_H

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

#ifndef SERIALWORK_H
#define SERIALWORK_H

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QApplication>
#include <QMutex>
#include <QMap>
#include <QtCore>
#include <QTextStream>
#include <QMessageBox>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>        
#include "usb_device.h"
#include "usb2spi.h"
#include "DefineData.h"
#include "ThreadSaveData.h"

#define BODYLEN 4096
#define INTERVAL 3

enum ParseType
{
    StateIdle  = 0,
    StateRecvHead,
    StateRecvExtFlag,
    StateRecvExtension,
    StateRecvVerify,
    StateRecvVersion,
    StateRecvBody,
    StateRecvEnd,
};

struct ParseBuffer
{
    enum ParseType ParseType;
    int bodyLen;
    int crcLen;
    int headLen;
    int allLen;
    int extLen;
    uchar extFlag;
    uchar version;
    uchar bufHead[2] = {0};
    uchar bufExt[10] = {0};
    uchar *bufBody =  nullptr;
    uchar bufCRC[4] = {0};
    int extSampleRate;
    int extFFT = 0;
    int extFrameLen;
    uchar extDataStruct;
    QString  extDataType;
    int fftSize = 0;
    int TimeAllLength = 0;
};

class SerialWork : public QObject
{
    Q_OBJECT
public:
    static SerialWork* instance() {
        static QMutex mutex;
        if (!m_pSerialWork){
            QMutexLocker locker(&mutex);
            if (!m_pSerialWork){
                m_pSerialWork = new SerialWork();
            }
        }
        return m_pSerialWork;
    }
    ~SerialWork();
    void parseData(unsigned char *, int);
    static QMutex m_mutexQueue;

signals:
    void signalSendG1G2Data(QVector<double>&data1, QVector<double>&data2);
    void signalResultOpenSpi(int ret);
    void signalShowMessage(QString title, QString message);
    void signalStartParse(int dataNum);

    void signalIsSave(bool);
    void signalIsSaveWav(bool);
    void signalFineshedSaveWav();
    void signalSave(QVector<float>);
    void signalSaveWav(QVector<float>);
    void signalTimeDomainData(TimeDomainData &data);
    void signalErrorCount(int nErrorCount, bool isRight);
    void signalDeviceStatus(int status);
    void signalArrayData(QVector<float>);

public slots:
    void  slotOpenSpi();
    void  slotSaveDataToCsvFile(bool isSave);
    void  slotSaveDataToWavFile(bool isSave);
    void  slotSaveDataToBinFile(bool isSave);
    void  slotDequeueParse(int dataNum);
    void  slotCheckDeviceIsOnline();
    void  slotFreDomainDataCfg(FrequencyData &data);
    void  slotTimtDomainDataCfg(TimeDomainData &data);
    void  slotArrayDomainDataCfg(ArrayDomainData &data);
private:
    void getFFTData();
    void getWavData();
    void getArrayData();
    void getSaveBinFile();
    void delNullCsvFile();
    void returnDataTypeSourceData(unsigned char * fftData);

private:
    SerialWork();
    static SerialWork* m_pSerialWork;
    QVector<float> m_fftData;
    QVector<double> m_G1Data;
    QVector<double> m_G2Data;
    QVector<double> m_ArrayData;

    int m_nDevHandle[10] ={0};
    int m_nSPIIndex = SPI1_CS0;
    int m_nG1G2DataNum = 0;

    ParseBuffer m_parseBuf;
    QTimer *m_pErrorTimer = nullptr;
    QTimer *m_pNoDataTimer = nullptr;
    bool m_bHaveData = false;

    QThread *m_pThread;
    bool m_bIsSave = false;
    bool m_bIsSaveWav = false;
    bool m_bIsSaveBinFile = false;
    QString m_strBinFilePath;
    int m_nIsSetSingleShot = 0;
    int m_nIsPcmOrFFtData = NONE;
};


#endif // SERIALWORK_H

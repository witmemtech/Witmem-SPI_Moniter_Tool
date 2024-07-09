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

#include "serialwork.h"
#include "ThreadSaveData.h"
#include "ThreadSaveWavData.h"
#include <QMetaType>
#include <QApplication>
#include <cstring>

int WINAPI SPI_SlaveStartContinueRead(int DevHandle,int SPIIndex);
int WINAPI SPI_SlaveStopContinueRead(int DevHandle,int SPIIndex);

QMutex SerialWork::m_mutexQueue;

QQueue<unsigned char *> globalQueue;
SerialWork *SerialWork::m_pSerialWork = nullptr;

SerialWork::SerialWork()
{
    qRegisterMetaType<QVector<float>>("QVector<float>&");
    connect(this, &SerialWork::signalStartParse,this, &SerialWork::slotDequeueParse);
    m_pThread = new QThread;
    ThreadSaveData *tSave = new ThreadSaveData();
    ThreadSaveWavData *pSaveWav = new  ThreadSaveWavData();
    tSave->moveToThread(m_pThread);
    pSaveWav->moveToThread(m_pThread);
    connect(m_pThread, &QThread::finished,tSave, &ThreadSaveData::deleteLater);
    connect(this, &SerialWork::signalSave,tSave, &ThreadSaveData::slotSaveCsvData);
    connect(this, &SerialWork::signalIsSave,tSave, &ThreadSaveData::slotIsSaveData);

    connect(m_pThread, &QThread::finished,pSaveWav, &ThreadSaveWavData::deleteLater);
    connect(this, &SerialWork::signalSaveWav,pSaveWav, &ThreadSaveWavData::slotSaveWavData);
    connect(this, &SerialWork::signalIsSaveWav,pSaveWav, &ThreadSaveWavData::slotIsSaveData);
    connect(this, &SerialWork::signalTimeDomainData,pSaveWav, &ThreadSaveWavData::slotInfoData);

    m_pThread->start();
    m_parseBuf.ParseType = StateIdle;
}

SerialWork::~SerialWork()
{
    if(m_parseBuf.bufBody == nullptr){
        delete[] m_parseBuf.bufBody;
        m_parseBuf.bufBody = nullptr;
    }
    delNullCsvFile();
}


static int spim_checksum(uint8_t *buffer,int size)
{
    int sum = 0,index = 0;
    for(index = 0 ; index < size ;index ++)
        sum += buffer[index];
    return sum;
}


int static chance = 1;
void SerialWork::parseData(unsigned char *data, int length)
{
    int i = 0;
    while(length--){
        unsigned char ch = data[i++];
        switch(m_parseBuf.ParseType){
        case StateIdle:
            if(ch == 0x78){
                m_parseBuf.bufBody = new uchar[m_parseBuf.extFFT * m_parseBuf.fftSize];
                m_parseBuf.ParseType = StateRecvHead;
                m_parseBuf.bodyLen = 0;
                m_parseBuf.headLen = 0;
                m_parseBuf.crcLen = 0;
                m_parseBuf.extLen = 0;
                m_parseBuf.allLen = 0;
                m_parseBuf.extFlag = 0;
            }
            break;
        case StateRecvHead:
            m_parseBuf.bufHead[m_parseBuf.headLen++] = ch;
            if(m_parseBuf.headLen == 2){
                m_parseBuf.allLen = (m_parseBuf.bufHead[0]<<8) + m_parseBuf.bufHead[1];
                if(m_parseBuf.allLen <= 10){
                    m_parseBuf.ParseType = StateIdle;
                    break;
                }
                int nDataAllLength = 0;
                nDataAllLength = m_parseBuf.extFFT * m_parseBuf.fftSize;
                if(m_parseBuf.allLen != nDataAllLength + 10 && chance != 0){
                    m_parseBuf.ParseType = StateIdle;
                    emit signalErrorCount(1,false);
                }else{
                    m_parseBuf.ParseType = StateRecvExtFlag;
                }
            }
            break;
        case StateRecvExtFlag:
            m_parseBuf.extFlag = ch;
            if(ch == 0x00){
                m_parseBuf.ParseType = StateRecvVerify;
            }
            if(ch == 0x01){
                m_parseBuf.ParseType = StateIdle;
            }
            break;
        case StateRecvExtension:
            m_parseBuf.bufExt[m_parseBuf.extLen++] = ch;
            if(m_parseBuf.extLen == 10){
                m_parseBuf.ParseType = StateRecvVerify;
            }
            break;
        case StateRecvVerify:
            if(m_parseBuf.crcLen != 4){
                m_parseBuf.bufCRC[m_parseBuf.crcLen++] = ch;
            }
            if(m_parseBuf.crcLen == 4){
                m_parseBuf.ParseType = StateRecvVersion;
            }
            break;
        case StateRecvVersion:
            m_parseBuf.version = ch;
            m_parseBuf.ParseType = StateRecvBody;
            break;
        case StateRecvBody:
            if(m_parseBuf.bodyLen >= m_parseBuf.extFFT * m_parseBuf.fftSize){
                m_parseBuf.ParseType = StateIdle;
                break;
            }
            m_parseBuf.bufBody[m_parseBuf.bodyLen++] = ch;
            if(m_parseBuf.extFlag == 0){
                if(m_parseBuf.bodyLen ==  m_parseBuf.extFFT * m_parseBuf.fftSize){
                    m_parseBuf.ParseType = StateRecvEnd;
                }
            }else if(m_parseBuf.extFlag == 1){
                if(m_parseBuf.bodyLen == m_parseBuf.allLen - 10 - 10){
                    m_parseBuf.ParseType = StateRecvEnd;
                }
            }
            break;
        case StateRecvEnd:
            if(ch == 0x16){
                uint32_t RecvCRC = 0;
                RecvCRC = (m_parseBuf.bufCRC[0] << 24) |(m_parseBuf.bufCRC[1] << 16) |
                        (m_parseBuf.bufCRC[2] << 8) |m_parseBuf.bufCRC[3];

                int CalcCRC = 0;
                uchar head = 0x78;
                uchar tail = 0x16;
                CalcCRC += spim_checksum(&head,1);
                CalcCRC += spim_checksum(m_parseBuf.bufHead,m_parseBuf.headLen);
                CalcCRC += spim_checksum(&m_parseBuf.extFlag,1);
                if(m_parseBuf.extFlag == 1){
                    CalcCRC += spim_checksum(m_parseBuf.bufExt,m_parseBuf.extLen);
                }
                CalcCRC += spim_checksum(&m_parseBuf.version,1);
                CalcCRC += spim_checksum(reinterpret_cast<uint8_t*>(m_parseBuf.bufBody),m_parseBuf.bodyLen);
                CalcCRC += spim_checksum(&tail,1);
                if(CalcCRC == RecvCRC){
                    chance = 1;
                    if(m_parseBuf.extFlag == 0){
                        returnDataTypeSourceData(m_parseBuf.bufBody);
                        if(m_bIsSaveBinFile == true){
                            getSaveBinFile();
                        }
                        if(m_nIsPcmOrFFtData ==  FFT){
                            getFFTData();
                        }else if(m_nIsPcmOrFFtData ==  PCM){
                            getWavData();
                        }else if(m_nIsPcmOrFFtData ==  ARRAY){
                            getArrayData();
                        }
                        m_parseBuf.ParseType = StateIdle;
                        emit signalErrorCount(0,true);
                    }else{
                        m_parseBuf.ParseType = StateIdle;
                    }
                    delete[] m_parseBuf.bufBody;
                    m_parseBuf.bufBody = nullptr;
                }else{
                    m_parseBuf.ParseType = StateIdle;
                    emit signalErrorCount(1,false);
                    delete[] m_parseBuf.bufBody;
                    m_parseBuf.bufBody = nullptr;
                    break;
                }
            }else{
                m_parseBuf.ParseType = StateIdle;
                emit signalErrorCount(1,false);
                delete[] m_parseBuf.bufBody;
                m_parseBuf.bufBody = nullptr;
            }
            break;
        default:
            break;
        }
    }
}

void SerialWork::returnDataTypeSourceData(unsigned char *fftData)
{
    m_fftData.clear();
    if(m_parseBuf.extDataType == "float"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            unsigned char bytes[4] = {fftData[ii*4], fftData[ii*4+1], fftData[ii*4+2], fftData[ii*4+3]};
            float val = 0;
            memcpy(&val, &bytes, sizeof(float));
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "uint32"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            uint32_t val = (fftData[ii*4+3] << 24) | (fftData[ii*4+2] << 16)
                    | (fftData[ii*4+1] << 8) | fftData[ii*4];
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "int32"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            int32_t val = (fftData[ii*4+3] << 24) | (fftData[ii*4+2] << 16)
                    | (fftData[ii*4+1] << 8) | fftData[ii*4];
            m_fftData.append(val);
        }
    }
    else if(m_parseBuf.extDataType == "uint24"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            uint32_t val = (fftData[ii*4+3] << 24) | (fftData[ii*4+2] << 16)
                    | (fftData[ii*4+1] << 8) | fftData[ii*4];
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "int24"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            int32_t val = (fftData[ii*4+3] << 24) | (fftData[ii*4+2] << 16)
                    | (fftData[ii*4+1] << 8) | fftData[ii*4];
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "uint16"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            uint16_t val = (fftData[ii*2+1] << 8) | fftData[ii*2];
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "int16"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            int16_t val = (fftData[ii*2+1] << 8) | fftData[ii*2];
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "uint8"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            uint8_t val = fftData[ii];
            m_fftData.append(val);
        }
    }else if(m_parseBuf.extDataType == "int8"){
        for(int ii = 0; ii<m_parseBuf.extFFT; ii++){
            int8_t val = fftData[ii];
            m_fftData.append(val);
        }
    }
}


void SerialWork::getFFTData()
{
    if(m_nG1G2DataNum < INTERVAL){
        m_nG1G2DataNum++;
    }
    m_G1Data.clear();
    m_G2Data.clear();
    if(m_bIsSave == true){
        emit signalSave(m_fftData);
    }
    if(m_nG1G2DataNum == INTERVAL){
        if(m_parseBuf.extDataStruct == 0){
            double d = 0;
            for(int i = 0; i< m_fftData.length(); i++){
                d = m_fftData.at(i);
                m_G1Data.append(d);
            }
            for (int i = 0; i< m_fftData.length(); i++){
                m_G2Data.append(0);
            }
        }else if(m_parseBuf.extDataStruct == 1){
            double d;
            for (int i = 0; i < m_fftData.length()/2; i++){
                d = m_fftData.at(i);
                m_G1Data.append(d);
            }
            for (int i = m_fftData.length()/2; i < m_fftData.length(); i++){
                d = m_fftData.at(i);
                m_G2Data.append(d);
            }
        }else if(m_parseBuf.extDataStruct == 2){
            double d;
            for (int i = 0; i < m_fftData.length(); i++){
                d = m_fftData.at(i);
                if(i%2 == 0){
                    m_G1Data.append(d);
                }else{
                    m_G2Data.append(d);
                }
            }
        }
        emit signalSendG1G2Data(m_G1Data,m_G2Data);
        m_nG1G2DataNum = 0;
    }
}

void SerialWork::getWavData()
{
    if(m_bIsSaveWav ==  true){
        emit signalSaveWav(m_fftData);
    }
}
void SerialWork::getArrayData()
{
    emit signalArrayData(m_fftData);
}

void SerialWork::getSaveBinFile()
{
    QFile AudioDataFile(m_strBinFilePath);
    if(AudioDataFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        QDataStream out(&AudioDataFile);
        for (const float& value : m_fftData) {
            if(m_parseBuf.extDataType == "int16"){
                int16_t v = static_cast<int16_t>(value);
                out << v;
            }else if(m_parseBuf.extDataType == "int8"){
                int8_t v = static_cast<int8_t>(value);
                out << v;
            }else{
                int32_t v = static_cast<int32_t>(value);
                out << v;
            }
        }
    }
    AudioDataFile.close();
}

void SerialWork::delNullCsvFile()
{
    QString csvDirPath = QCoreApplication::applicationDirPath() + "/csv";
    QDir csvDir(csvDirPath);
    QStringList csvFiles = csvDir.entryList(QStringList() << "*.csv", QDir::Files);
    for (const QString& csvFile : csvFiles){
        QString csvFilePath = csvDirPath + "/" + csvFile;
        QFile file(csvFilePath);
        if (file.size() == 0){
            file.remove();
        }
    }
}


void SerialWork::slotSaveDataToCsvFile(bool isSave)
{
    m_bIsSave = isSave;
    emit signalIsSave(isSave);
}

void SerialWork::slotSaveDataToWavFile(bool isSave)
{
    m_bIsSaveWav = isSave;
    emit signalIsSaveWav(isSave);
}

void SerialWork::slotSaveDataToBinFile(bool isSave)
{
    m_bIsSaveBinFile = isSave;
    if(m_bIsSaveBinFile == true){
        QString binPath;
        binPath = QCoreApplication::applicationDirPath();
        binPath+= QDir::separator();
        binPath += "BinFile";
        QDir dir;
        dir.setPath(binPath);
        if (dir.exists() == false){
            if (dir.mkpath(binPath) == false){
                return;
            }
        }
        QString date = QDateTime::currentDateTime().toString("yyyy_MM_dd_hhmmss");
        m_strBinFilePath = binPath+"/"+ QString("AudioData_%1").arg(date) +".bin";
    }
}

void SerialWork::slotCheckDeviceIsOnline()
{
    if(false == m_bHaveData){
        if(m_pNoDataTimer->isActive()){
            m_pNoDataTimer->stop();
        }
        chance = 0;
        m_nIsSetSingleShot = 0;
        m_parseBuf.ParseType = StateIdle;
        int ret = 0;
        ret = USB_ScanDevice(m_nDevHandle);
        if(ret > 0){
            int ret = USB_RetryConnect(m_nDevHandle[0]);
            if(ret  < 0){
                emit signalDeviceStatus(0);
            }else{
                emit signalDeviceStatus(1);
            }
        }else{
            emit signalDeviceStatus(0);
        }
    }
    if(true == m_bHaveData){
        m_bHaveData = false;
    }
}

void SerialWork::slotFreDomainDataCfg(FrequencyData &data)
{
    m_nIsPcmOrFFtData = FFT;
    m_parseBuf.extFFT = data.dSampleSize;
    m_parseBuf.extSampleRate = data.dSamplingRate;
    m_parseBuf.extDataStruct = data.nTransmissionOrder;
    m_parseBuf.extDataType = dataFreTypeDesc.value(data.nDataType);
    m_parseBuf.fftSize = sampleNumMap.value(m_parseBuf.extDataType) / 8;
}

void SerialWork::slotTimtDomainDataCfg(TimeDomainData &data)
{
    m_nIsPcmOrFFtData = PCM;
    m_parseBuf.extDataType = data.strSampleType;
    m_parseBuf.TimeAllLength = (data.dSamplingRate / 1000) *data.dFrameLength *
            data.nAudioTrackNum * (sampleNumMap.value(data.strSampleType) / 8);
    m_parseBuf.extFFT = m_parseBuf.TimeAllLength / (sampleNumMap.value(data.strSampleType) / 8);
    m_parseBuf.fftSize = sampleNumMap.value(m_parseBuf.extDataType) / 8;
}

void SerialWork::slotArrayDomainDataCfg(ArrayDomainData &data)
{
    m_nIsPcmOrFFtData = ARRAY;
    m_parseBuf.extFFT = data.dArrayMembersNum;
    m_parseBuf.extDataType = dataFreTypeDesc.value(data.nDataType);
    m_parseBuf.fftSize = sampleNumMap.value(m_parseBuf.extDataType) / 8;
}

void SerialWork::slotDequeueParse(int dataNum)
{
    QMutexLocker locker(&m_mutexQueue);
    if (!globalQueue.isEmpty()){
        unsigned char* buffer = globalQueue.dequeue();
        locker.unlock();
        m_bHaveData = true;
        if(0 == m_nIsSetSingleShot++){
            if(m_pNoDataTimer == nullptr){
                m_pNoDataTimer = new QTimer();
                connect(m_pNoDataTimer,&QTimer::timeout,this,&SerialWork::slotCheckDeviceIsOnline);
            }
            m_pNoDataTimer->start(1000);
        }
        parseData(buffer,dataNum);
        emit signalDeviceStatus(2);
    }else{
        return;
    }
}

int __stdcall SPI_SlaveReadData(int DevHandle,int SPIIndex,unsigned char *pData,int DataNum)
{
    (void) DevHandle;
    (void) SPIIndex;

    if(DataNum <= 0){return 0;}
    unsigned char* new_buffer = new unsigned char[DataNum];
    std::memset(new_buffer, 0, DataNum);
    memcpy(new_buffer, pData, DataNum);{
        QMutexLocker locker(&(SerialWork::instance()->m_mutexQueue));
        globalQueue.enqueue(new_buffer);
    }
    emit SerialWork::instance()->signalStartParse(DataNum);
    return 0;
}

void SerialWork::slotOpenSpi()
{
    SPI_CONFIG SPIConfig;
    uchar state;
    int ret = 0;
    ret = USB_ScanDevice(m_nDevHandle);
    if(ret <= 0){
        emit signalResultOpenSpi(SPI_DEVICE_SCAN_FAILE);
        return;
    }

    state = USB_OpenDevice(m_nDevHandle[0]);
    if(state == 0){
        emit signalResultOpenSpi(SPI_DEVICE_OPEN_FAILE);
        return;
    }

    int DataTemp = 1;
    SPIConfig.CPHA = DataTemp;
    SPIConfig.CPOL = DataTemp;
    SPIConfig.Mode = SPI_MODE_HARD_HDX;
    SPIConfig.ClockSpeedHz = 10000000;

    SPIConfig.LSBFirst = SPI_MSB;
    SPIConfig.Master = SPI_SLAVE;
    SPIConfig.SelPolarity = SPI_SEL_LOW;
    state = DEV_SetPowerLevel(m_nDevHandle[0],POWER_LEVEL_1V8);
    if(state == 0){
        return;
    }

    ret = SPI_Init(m_nDevHandle[0],m_nSPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        emit signalResultOpenSpi(SPI_DEVICE_INIT_FAILE);
        return;
    }
    int ret1 = SPI_SlaveContinueRead(m_nDevHandle[0],m_nSPIIndex,SPI_SlaveReadData);
    if(ret1 != SPI_SUCCESS){
        emit signalResultOpenSpi(SPI_DEVICE_SLAVE_READ_FAILE);
        return;
    }else{
        if(m_pErrorTimer == nullptr){
            m_pErrorTimer = new QTimer();
            m_pErrorTimer->setSingleShot(true);
            m_pErrorTimer->start(10000);
        }
    }
    emit signalResultOpenSpi(0);
}


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

#include "ThreadSaveWavData.h"
#define MAX_CSV_FILE_SIZE 1000000

ThreadSaveWavData::ThreadSaveWavData()
{

}

ThreadSaveWavData::~ThreadSaveWavData()
{
    for(int i = 0; i< m_FileList.size(); i++){
        QFile* pfile = m_FileList.at(i);
        if(pfile){
            if(pfile->isOpen()){
                pfile->close();
            }
            delete pfile;
            m_FileList.removeAt(i);
            i--;
        }
    }
}

void ThreadSaveWavData::addWAVFileHead(quint32 size,int sampleRate, int numChannels, int sampleSize)
{
    m_FileList.clear();
    for(int i = 0;i < m_fileNameList.size(); i++){
        QFile  *pWavFile =  new QFile(m_fileNameList.at(i));
        if(pWavFile->exists()){
            pWavFile->remove();
        }
        pWavFile->open(QIODevice::WriteOnly | QIODevice::Append);
        m_FileList.append(pWavFile);

        QDataStream wavDataStream(pWavFile);
        wavDataStream.setByteOrder(QDataStream::LittleEndian);
        wavDataStream.writeRawData("RIFF", 4);
        quint32 fileSize = 0;
        wavDataStream << fileSize;
        wavDataStream.writeRawData("WAVE", 4);
        wavDataStream.writeRawData("fmt ", 4);
        qint32 subchunk1Size = 16;
        wavDataStream << subchunk1Size;
        qint16 audioFormat = 1;
        wavDataStream << audioFormat;

        qint16 numChannelsValue = numChannels;
        wavDataStream << numChannelsValue;

        qint32 sampleRateValue = sampleRate;
        wavDataStream << sampleRateValue;

        qint32 byteRate = sampleRate * numChannels * (sampleSize / 8);
        wavDataStream << byteRate;

        qint16 blockAlign = numChannels * (sampleSize / 8);
        wavDataStream << blockAlign;

        qint16 bitsPerSample = sampleSize;
        wavDataStream << bitsPerSample;

        wavDataStream.writeRawData("data", 4);
        wavDataStream << size;
        pWavFile->flush();
    }
}

void ThreadSaveWavData::convertPCMToWAV(QVector<float>vec)
{
    int num = vec.size() / m_sInfoData.nAudioTrackNum;
    m_nFileSizeTotal += num * sampleNumMap.value(m_sInfoData.strSampleType) / 8;

    QVector<float>vecData;
    for(int i = 0; i< m_FileList.size(); i++){
        QFile* pfile = m_FileList.at(i);
        if(pfile){
            QDataStream wavDataStream(pfile);
            wavDataStream.setByteOrder(QDataStream::LittleEndian);

            wavDataStream.device()->seek(4);
            wavDataStream << m_nFileSizeTotal;

            wavDataStream.device()->seek(40);
            wavDataStream << m_nFileSizeTotal -36;
            wavDataStream.device()->seek(wavDataStream.device()->size());

            vecData = vec.mid(i*num,num);
            for (const float& value : vecData) {
                if(m_sInfoData.strSampleType == "int16"){
                    int16_t v = static_cast<int16_t>(value);
                    wavDataStream << v;
                }else if(m_sInfoData.strSampleType == "int8"){
                    int8_t v = static_cast<int8_t>(value);
                    wavDataStream << v;
                }else{
                    int32_t v = static_cast<int32_t>(value);
                    wavDataStream << v;
                }
            }
            pfile->flush();
        }
    }
}

void ThreadSaveWavData::slotSaveWavData(QVector<float> vec)
{
    int sampleRate = m_sInfoData.dSamplingRate;
    int numChannels = 1;
    int sampleSize  = sampleNumMap.value(m_sInfoData.strSampleType);
    if(m_bIsAddWavHead ==  true){
        addWAVFileHead(64,sampleRate,numChannels,sampleSize);
        m_bIsAddWavHead = false;
    }
    convertPCMToWAV(vec);
}

void ThreadSaveWavData::slotIsSaveData(bool isSave)
{
    m_bIsCreatFile = isSave;
    if(isSave == false) {
        for(int i = 0; i< m_FileList.size(); i++){
            QFile* pfile = m_FileList.at(i);
            if(pfile){
                if(pfile->isOpen()){
                    pfile->close();
                }
                delete pfile;
                m_FileList.removeAt(i);
                i--;
            }
        }
    }else{
        m_bIsAddWavHead = true;
        m_nFileSizeTotal = 36;
        createWavFile();
    }
}

void ThreadSaveWavData::slotInfoData(TimeDomainData &sInfoData)
{
    m_sInfoData = sInfoData;
    m_bIsAddWavHead = true;
    m_nFileSizeTotal = 36;
    if(m_bIsCreatFile == true){
        createWavFile();
    }
}

bool ThreadSaveWavData::createWavFile()
{
    m_fileNameList.clear();
    QString date = QDateTime::currentDateTime().toString("yyyy_MM_dd_hhmmss");

    QString wavPath;
    QString wavDataPath;
    wavPath = QCoreApplication::applicationDirPath();
    wavPath+= QDir::separator();
    wavPath += "wav";
    QDir dir;
    dir.setPath(wavPath);

    if (dir.exists() == false){
        if (dir.mkpath(wavPath) == false){
            return false;
        }
    }

    if(m_sInfoData.nAudioTrackNum != 0){
        wavDataPath = wavPath;
        wavDataPath+= QDir::separator();
        wavDataPath += date;
        dir.setPath(wavDataPath);
        if (dir.exists() == false){
            if (dir.mkpath(wavDataPath) == false){
                return false;
            }
        }
    }

    for(int i = 0; i < m_sInfoData.nAudioTrackNum; i++){
        QString  strFileName = wavDataPath+"/"+ QString("AudioTrack%1_%2").arg(i).arg(date) +".wav";
        m_fileNameList.append(strFileName);
    }
    return true;
}


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

#ifndef THREADSAVEWAVDATA_H
#define THREADSAVEWAVDATA_H

#include <QObject>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QDataStream>
#include <QVector>
#include <QSaveFile>
#include <QDebug>
#include "DefineData.h"

class ThreadSaveWavData: public QObject
{
    Q_OBJECT
public:
    ThreadSaveWavData();
    ~ThreadSaveWavData();
public slots:
    void slotSaveWavData(QVector<float>);
    void slotIsSaveData(bool);
    void slotInfoData(TimeDomainData &sInfoData);

private:
    bool createWavFile();
    void addWAVFileHead(quint32 size,int sampleRate, int numChannels, int sampleSize);
    void convertPCMToWAV(QVector<float>vec);
private:
    QFile  *m_pWavFile = nullptr;
    qint32 m_nFileSizeTotal = 36;
    bool m_bIsAddWavHead = true;
    bool m_bIsCreatFile = true;
    TimeDomainData m_sInfoData;
    QStringList m_fileNameList;
    QList<QFile*>m_FileList;
};

#endif // THREADSAVEWAVDATA_H

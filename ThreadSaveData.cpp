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

#include "ThreadSaveData.h"
#define MAX_CSV_FILE_SIZE 1000000

ThreadSaveData::ThreadSaveData()
{
    createCsvFile();
}

ThreadSaveData::~ThreadSaveData()
{
    if(m_pCsvFile){
        if(m_pCsvFile->isOpen()){
            m_pCsvFile->close();
        }
        delete m_pCsvFile;
        m_pCsvFile = nullptr;
    }
}

bool  ThreadSaveData::checkFileSize()
{
    bool status = true;
    if(nullptr != m_pCsvFile && m_nFileSize >= MAX_CSV_FILE_SIZE){
        m_pCsvFile->close();
        delete m_pCsvFile;
        m_pCsvFile = nullptr;
        status = createCsvFile();
    }
    return status;
}

void ThreadSaveData::slotSaveCsvData(QVector<float> vec)
{
    QString str;
    for (int i = 0; i < vec.length(); i++){
        if(i == vec.length()-1){
            str = QString::number(vec[i])+'\n';
        }else{
            str = QString::number(vec[i])+',';
        }
        m_pCsvFile->write(str.toStdString().c_str());
        m_pCsvFile->flush();
    }
    m_nFileSize = m_pCsvFile->size();
    checkFileSize();
}

void ThreadSaveData::slotIsSaveData(bool isSave)
{
    if(isSave == false){
        if(m_pCsvFile->isOpen()){
            m_pCsvFile->close();
        }
        delete m_pCsvFile;
        m_pCsvFile = nullptr;
    }else{
        if(m_pCsvFile == nullptr){
            createCsvFile();
        }
    }
}

bool ThreadSaveData::createCsvFile()
{
    QString csvPath;
    csvPath = QCoreApplication::applicationDirPath();
    csvPath+= QDir::separator();
    csvPath += "csv";
    QDir dir;
    dir.setPath(csvPath);
    if (dir.exists() == false){
        if (dir.mkpath(csvPath) == false){
            return false;
        }
    }

    QString date=QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString  name = csvPath+"/"+date+".csv";

    m_pCsvFile = new QFile(name);
    m_pCsvFile->open(QIODevice::WriteOnly | QIODevice::Append);
    return true;
}


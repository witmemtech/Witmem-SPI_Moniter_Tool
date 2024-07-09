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

#ifndef THREADSAVEDATA_H
#define THREADSAVEDATA_H

#include <QObject>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

class ThreadSaveData: public QObject
{
    Q_OBJECT
public:
    ThreadSaveData();
    ~ThreadSaveData();
public slots:
    void slotSaveCsvData(QVector<float>);
    void slotIsSaveData(bool);

private:
    bool createCsvFile();
    bool checkFileSize();

    QFile *m_pCsvFile = nullptr;
    int m_nFileSize = 0;
};

#endif // THREADSAVEDATA_H

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

#include <QApplication>
#include <QStyleFactory>
#include "FrequencyToolsMainWindow.h"
#include "DefineData.h"

void initDataType(){
    dataFreTypeDesc.insert(1,"float");
    dataFreTypeDesc.insert(2,"uint32");
    dataFreTypeDesc.insert(3,"int32");
    dataFreTypeDesc.insert(4,"uint16");
    dataFreTypeDesc.insert(5,"int16");
    dataFreTypeDesc.insert(6,"unint8");
    dataFreTypeDesc.insert(7,"int8");

    dataTimeTypeDesc.insert(1,"float");
    dataTimeTypeDesc.insert(2,"uint32");
    dataTimeTypeDesc.insert(3,"int32");
    dataTimeTypeDesc.insert(4,"uint16");
    dataTimeTypeDesc.insert(5,"int16");
    dataTimeTypeDesc.insert(6,"unint8");
    dataTimeTypeDesc.insert(7,"int8");

    dataArrayTypeDesc.insert(1,"float");
    dataArrayTypeDesc.insert(2,"uint32");
    dataArrayTypeDesc.insert(3,"int32");
    dataArrayTypeDesc.insert(4,"uint16");
    dataArrayTypeDesc.insert(5,"int16");
    dataArrayTypeDesc.insert(6,"unint8");
    dataArrayTypeDesc.insert(7,"int8");
}

void initTransmissionOrderMap(){
    TransmissionOrderMap.insert(0,"1组样本");
    TransmissionOrderMap.insert(1,"2组样本-分段");
    TransmissionOrderMap.insert(2,"2组样本-交替");
}
void initSampleNumMap(){
    sampleNumMap.insert("float",32);
    sampleNumMap.insert("uint32",32);
    sampleNumMap.insert("int32",32);
    sampleNumMap.insert("uint24",32);
    sampleNumMap.insert("int24",32);
    sampleNumMap.insert("uint16",16);
    sampleNumMap.insert("int16",16);
    sampleNumMap.insert("unint8",8);
    sampleNumMap.insert("int8",8);

    LogNumMap.insert("float",1);
    LogNumMap.insert("uint32",4294967295);
    LogNumMap.insert("int32",2147483647);
    LogNumMap.insert("uint24",4294967295);
    LogNumMap.insert("int24",2147483647);
    LogNumMap.insert("uint16",65535);
    LogNumMap.insert("int16",32767);
    LogNumMap.insert("unint8",255);
    LogNumMap.insert("int8",127);
}
void initSpicyErrorType(){
    SpiErrorDesc.insert(SPI_DEVICE_SCAN_FAILE,"扫描搜索设备失败!");
    SpiErrorDesc.insert(SPI_DEVICE_OPEN_FAILE,"设备打开失败!");
    SpiErrorDesc.insert(SPI_DEVICE_GET_INFO_FAILE,"设备信息获取失败!");
    SpiErrorDesc.insert(SPI_DEVICE_INIT_FAILE,"设备配置参数失败!");
    SpiErrorDesc.insert(SPI_DEVICE_SLAVE_READ_FAILE,"从机模式启动失败!");
    SpiErrorDesc.insert(SPI_DEVICE_OPEN_SUCCESS,"设备打开成功!");
}

void initArrayTypeMap(){
    ArrayTypeMap.insert(POINT_TYPE,"样点");
    ArrayTypeMap.insert(TIME_TYPE,"时间");
}
void initDevStatus(){
    DeviceStatusDesc.insert(0,"适配器离线");
    DeviceStatusDesc.insert(1,"就绪");
    DeviceStatusDesc.insert(2,"传输中");
}
void Setstyle(const QString &styleName)
{
    QFile file(QString(":/ResFile/%1").arg(styleName));
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyle(QStyleFactory::create("fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
    initDataType();
    initArrayTypeMap();
    initSampleNumMap();
    initTransmissionOrderMap();
    initSpicyErrorType();
    initDevStatus();
    a.setWindowIcon(QIcon(":/ResFile/logo.png"));
    FrequencyToolsMainWindow w;
    w.setWindowTitle(QString("SPI_Moniter_v%1").arg(APP_VERSION));
    QFont font;
    font.setPixelSize(12);
    qApp->setFont(font);
    w.show();
    return a.exec();
}

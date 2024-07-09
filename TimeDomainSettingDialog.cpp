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

#include "TimeDomainSettingDialog.h"
#include "ui_TimeDomainSettingDialog.h"
#include <QDebug>
TimeDomainSettingDialog::TimeDomainSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeDomainSettingDialog)
{
    ui->setupUi(this);
    initUi();
}

TimeDomainSettingDialog::~TimeDomainSettingDialog()
{
    delete ui;
}

void TimeDomainSettingDialog::initUi()
{
    setWindowTitle("设置");
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->TracksNumBox->setCurrentText("10");
    ui->SampleBitBox->setCurrentText("16");
    ui->TransmissionBitBox->setCurrentText("16");
    ui->SamplingRateBox->setCurrentText("16000Hz");
    readCfgDataFromJson();
    this->setFixedSize(this->size());
}

void TimeDomainSettingDialog::saveCfgDataToJson()
{
    QString path = QCoreApplication::applicationDirPath();
    QString TimeDomainCfgFilePath = path + QString("/cfgJsonFile/TimeDomainCfg.json");

    QJsonObject json;
    QJsonObject timeDomainData;
    timeDomainData["SamplingRate"] = ui->SamplingRateBox->currentText();
    timeDomainData["AudioTrackNum"] = ui->TracksNumBox->currentText();
    timeDomainData["TransmissionBit"] = ui->TransmissionBitBox->currentText();
    timeDomainData["SampleType"] = ui->SampleBitBox->currentText();
    timeDomainData["FrameLength"] = ui->FrameLenghtBox->currentText();

    json["timeDomainData"] = timeDomainData;
    QJsonDocument doc(json);
    QFile cfgFile(TimeDomainCfgFilePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        cfgFile.write(doc.toJson());
    }
    cfgFile.close();
}

void TimeDomainSettingDialog::readCfgDataFromJson()
{
    QString path = QCoreApplication::applicationDirPath();
    QString TimeDomainCfgFilePath = path + QString("/cfgJsonFile/TimeDomainCfg.json");
    QFile cfgFile(TimeDomainCfgFilePath);
    if (cfgFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cfgFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = doc.object();
        QJsonObject TimeDomainObj = jsonObj["timeDomainData"].toObject();
        ui->SamplingRateBox->setCurrentText(TimeDomainObj["SamplingRate"].toString());
        ui->TracksNumBox->setCurrentText(TimeDomainObj["AudioTrackNum"].toString());
        ui->TransmissionBitBox->setCurrentText(TimeDomainObj["TransmissionBit"].toString());
        ui->SampleBitBox->setCurrentText(TimeDomainObj["SampleType"].toString());
        ui->FrameLenghtBox->setCurrentText(TimeDomainObj["FrameLength"].toString());
    }
    cfgFile.close();
}

TimeDomainData TimeDomainSettingDialog::getTimeCfgData()
{
    TimeDomainData data;
    QString text = ui->SamplingRateBox->currentText();
    QString numberString = text.left(text.indexOf("Hz"));

    data.dSamplingRate = numberString.toDouble();
    data.nAudioTrackNum = ui->TracksNumBox->currentText().toDouble();
    data.nTransmissionBit = ui->TransmissionBitBox->currentText().toInt();
    data.strSampleType = ui->SampleBitBox->currentText();
    QString temp = ui->FrameLenghtBox->currentText();
    QString FrameLength = temp.left(temp.indexOf("ms"));
    data.dFrameLength = FrameLength.toDouble();
    return data;
}

void TimeDomainSettingDialog::on_TimeConfirmBtn_clicked()
{
    done(1);
    saveCfgDataToJson();
}


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

#include "FrequencyDomainSettingDialog.h"
#include "ui_FrequencyDomainSettingDialog.h"

FrequencyDomainSettingDialog::FrequencyDomainSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrequencyDomainSettingDialog)
{
    ui->setupUi(this);
    initUi();
}

FrequencyDomainSettingDialog::~FrequencyDomainSettingDialog()
{
    delete ui;
}

FrequencyData FrequencyDomainSettingDialog::getFreCfgData()
{
    FrequencyData data;
    QString text = ui->SamplingRateBox->currentText();
    QString numberString = text.left(text.indexOf("Hz"));

    data.dSamplingRate = numberString.toDouble();
    data.dSampleSize = ui->FrequencyNumBox->currentText().toDouble();
    data.nDataType = ui->DataTypeBox->currentIndex() + 1;
    data.nTransmissionOrder = ui->TransmissionOrderBox->currentIndex();
    return data;
}

void FrequencyDomainSettingDialog::saveCfgDataToJson()
{
    QString path = QCoreApplication::applicationDirPath();
    QString FreDomainCfgFilePath = path + QString("/cfgJsonFile/FreDomainCfg.json");

    QJsonObject json;
    QJsonObject freDomainData;
    freDomainData["SamplingRate"] = ui->SamplingRateBox->currentText();
    freDomainData["SampleSize"] = ui->FrequencyNumBox->currentText();
    freDomainData["DataType"] = ui->DataTypeBox->currentText();
    freDomainData["TransmissionOrder"] = ui->TransmissionOrderBox->currentText();

    json["freDomainData"] = freDomainData;
    QJsonDocument doc(json);
    QFile cfgFile(FreDomainCfgFilePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        cfgFile.write(doc.toJson());
    }
    cfgFile.close();
}

void FrequencyDomainSettingDialog::readCfgDataFromJson()
{
    QString path = QCoreApplication::applicationDirPath();
    QString FreDomainCfgFilePath = path + QString("/cfgJsonFile/FreDomainCfg.json");
    QFile cfgFile(FreDomainCfgFilePath);
    if (cfgFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cfgFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = doc.object();
        QJsonObject FreDomainObj = jsonObj["freDomainData"].toObject();
        ui->SamplingRateBox->setCurrentText(FreDomainObj["SamplingRate"].toString());
        ui->FrequencyNumBox->setCurrentText(FreDomainObj["SampleSize"].toString());
        ui->DataTypeBox->setCurrentText(FreDomainObj["DataType"].toString());
        ui->TransmissionOrderBox->setCurrentText(FreDomainObj["TransmissionOrder"].toString());

    }
    cfgFile.close();
}

void FrequencyDomainSettingDialog::initUi()
{
    setWindowTitle("设置");
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->SamplingRateBox->setCurrentText("16000Hz");
    ui->DataTypeBox->setCurrentIndex(1);
    readCfgDataFromJson();
    this->setFixedSize(this->size());
}

void FrequencyDomainSettingDialog::on_FreConfirmBtn_clicked()
{
    done(1);
    saveCfgDataToJson();
}


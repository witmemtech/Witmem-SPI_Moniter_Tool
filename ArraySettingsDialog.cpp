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

#include "ArraySettingsDialog.h"
#include "ui_ArraySettingsDialog.h"
#include  <QListView>

ArraySettingsDialog::ArraySettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArraySettingsDialog)
{
    ui->setupUi(this);
    initUi();
}

ArraySettingsDialog::~ArraySettingsDialog()
{
    delete ui;
}

ArrayDomainData ArraySettingsDialog::getArrayCfgData()
{
    QList<QCheckBox*>list = this->findChildren<QCheckBox*>();
    ArrayDomainData data;
    data.chickedList.clear();
    data.dArrayMembersNum = ui->ArrayMembersNumEdit->text().toDouble();
    data.nDataType = ui->ArrayDataTypeBox->currentIndex() + 1;
    data.nPointCountTime = ui->ArrayPointCountEdit->text().toDouble();
    data.nXAsixType = ui->XAxisTypeComBox->currentIndex();
    for(int i = 0; i <list.size();i++){
        if(list.at(i)->isChecked() == true){
            data.chickedList.append(list.at(i));
        }
    }
    return data;
}

void ArraySettingsDialog::initUi()
{
    setWindowTitle("设置");
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->ArrayMembersNumEdit->setText("64");
    readCfgDataFromJson();
    this->setFixedSize(this->size());
    for(int i = 1; i <= 32;i++){
        ChangnelSelectShowWidget *widget = new ChangnelSelectShowWidget(this);
        widget->setLabelText(QString::number(i));
        if(i == 1){
          widget->setCheckBox(true);
        }
        if(i <= 8){
            ui->horizontalLayout_3->addWidget(widget);
        }else if(i <= 16){
            ui->horizontalLayout_4->addWidget(widget);
        }else if(i <= 24){
            ui->horizontalLayout_5->addWidget(widget);
        }else if(i <= 32){
            ui->horizontalLayout_6->addWidget(widget);
        }
    }
    ui->scrollArea->m_checkList = this->findChildren<QCheckBox*>();
}

void ArraySettingsDialog::saveCfgDataToJson()
{
    QString path = QCoreApplication::applicationDirPath();
    QString ArrayDomainCfgFilePath = path + QString("/cfgJsonFile/ArrayDomainCfg.json");

    QJsonObject json;
    QJsonObject arrayDomainData;
    arrayDomainData["ArrayMembersNum"] = ui->ArrayMembersNumEdit->text();
    arrayDomainData["DataType"] = ui->ArrayDataTypeBox->currentText();

    json["arrayDomainData"] = arrayDomainData;
    QJsonDocument doc(json);
    QFile cfgFile(ArrayDomainCfgFilePath);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        cfgFile.write(doc.toJson());
    }
    cfgFile.close();
}

void ArraySettingsDialog::readCfgDataFromJson()
{
    QString path = QCoreApplication::applicationDirPath();
    QString ArrayDomainCfgFilePath = path + QString("/cfgJsonFile/ArrayDomainCfg.json");
    QFile cfgFile(ArrayDomainCfgFilePath);
    if (cfgFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cfgFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = doc.object();
        QJsonObject ArrayDomainObj = jsonObj["arrayDomainData"].toObject();
        ui->ArrayMembersNumEdit->setText(ArrayDomainObj["ArrayMembersNum"].toString());
        ui->ArrayDataTypeBox->setCurrentText(ArrayDomainObj["DataType"].toString());
    }
    cfgFile.close();
}

void ArraySettingsDialog::on_ArrayConfirmBtn_clicked()
{
    done(1);
    saveCfgDataToJson();
}

void ArraySettingsDialog::on_ArrayMembersNumEdit_textChanged(const QString &arg1)
{
    int pointCount = arg1.toInt();
    for(int i = 0; i  < m_checkBoxList.size();i++){
        if(i < pointCount){
            m_checkBoxList.at(i)->setDisabled(false);
        }else{
            m_checkBoxList.at(i)->setDisabled(true);
            m_checkBoxList.at(i)->setChecked(false);
        }
    }
}


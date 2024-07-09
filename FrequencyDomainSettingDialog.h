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

#ifndef FREQUENCYDOMAINSETTINGDIALOG_H
#define FREQUENCYDOMAINSETTINGDIALOG_H

#include <QDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

#include "DefineData.h"
namespace Ui {
class FrequencyDomainSettingDialog;
}

class FrequencyDomainSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FrequencyDomainSettingDialog(QWidget *parent = nullptr);
    ~FrequencyDomainSettingDialog();
    FrequencyData getFreCfgData();
private slots:
    void on_FreConfirmBtn_clicked();

private:
    void initUi();
    void saveCfgDataToJson();
    void readCfgDataFromJson();
private:
    Ui::FrequencyDomainSettingDialog *ui;
};

#endif // FREQUENCYDOMAINSETTINGDIALOG_H

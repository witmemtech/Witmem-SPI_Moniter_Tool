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

#ifndef ARRAYSETTINGSDIALOG_H
#define ARRAYSETTINGSDIALOG_H

#include <QDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QCheckBox>
#include <QAction>
#include "ChangnelSelectShowWidget.h"
#include "DefineData.h"

namespace Ui {
class ArraySettingsDialog;
}

class ArraySettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArraySettingsDialog(QWidget *parent = nullptr);
    ~ArraySettingsDialog();
    ArrayDomainData getArrayCfgData();

private slots:
    void on_ArrayConfirmBtn_clicked();
    void on_ArrayMembersNumEdit_textChanged(const QString &arg1);
private:
    void initUi();
    void saveCfgDataToJson();
    void readCfgDataFromJson();
private:
    Ui::ArraySettingsDialog *ui;
    QList<QCheckBox*>m_checkBoxList;
};

#endif // ARRAYSETTINGSDIALOG_H

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

#include "ChangnelSelectShowWidget.h"
#include "ui_ChangnelSelectShowWidget.h"

ChangnelSelectShowWidget::ChangnelSelectShowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangnelSelectShowWidget)
{
    ui->setupUi(this);
    initUi();
}

ChangnelSelectShowWidget::~ChangnelSelectShowWidget()
{
    delete ui;
}

void ChangnelSelectShowWidget::setLabelText(QString text)
{
    ui->label->setText(text);
    ui->checkBox->setObjectName(QString("ChannelCheckBox_%1").arg(text));
}

void ChangnelSelectShowWidget::setCheckBox(bool isChecked)
{
    ui->checkBox->setChecked(isChecked);
}

void ChangnelSelectShowWidget::initUi()
{

}

void ChangnelSelectShowWidget::on_checkBox_clicked(bool checked)
{
    emit signalIsShowGrahp(checked);
}


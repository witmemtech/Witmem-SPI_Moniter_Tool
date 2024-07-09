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

#include "ArrayLegedLabel.h"
#include "ui_ArrayLegedLabel.h"

ArrayLegedLabel::ArrayLegedLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArrayLegedLabel)
{
    ui->setupUi(this);
}

ArrayLegedLabel::~ArrayLegedLabel()
{
    delete ui;
}

void ArrayLegedLabel::setLegendInfo(QString name, QString color)
{
    ui->LegendColor->setStyleSheet(QString("border-radius:4px;background:%1;").arg(color));
    ui->LegendName->setText(name);
}

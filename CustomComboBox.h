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

#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QDebug>
class CustomComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit CustomComboBox(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        // 创建下拉按钮
        QPushButton* button = new QPushButton("Select", this);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, &QPushButton::clicked, this, &CustomComboBox::toggleMenu);

        // 创建下拉控件
        combo = new QComboBox(this);
        combo->addItem("Item 1");
        combo->addItem("Item 2");
        combo->addItem("Item 3");
        combo->hide(); // 初始状态隐藏下拉控件
        connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CustomComboBox::onComboActivated);

        // 设置布局
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(button);
        layout->addWidget(combo);
        setLayout(layout);
    }

private slots:
    void toggleMenu()
    {
        if (combo->isVisible()) {
            combo->hide();
        } else {
            combo->show();
        }
    }

    void onComboActivated(int index)
    {
        // 处理选项选择
        qDebug() << "Selected item:" << combo->itemText(index);
        combo->hide(); // 选项选择后隐藏下拉控件
    }

private:
    QComboBox* combo;
};

#endif // CUSTOMCOMBOBOX_H

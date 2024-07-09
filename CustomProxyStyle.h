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

#ifndef CUSTOMPROXYSTYLE_H
#define CUSTOMPROXYSTYLE_H

#include <QtGui>
#include<QProxyStyle>
#include<QStyleOption>
#include <QStyle>
//绘制一个你自己想要的风格，用法可以查帮助文档，很详细
class CustomTabStyle : public QProxyStyle
{
public:
public:
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override
    {
        if (metric == QStyle::PM_MaximumPopupHeight) {
            return 100; // 设置下拉框的最大高度为100px
        }
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
};
#endif // CUSTOMPROXYSTYLE _H

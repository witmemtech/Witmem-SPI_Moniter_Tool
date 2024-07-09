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

#ifndef AVRCALCULATOR_H
#define AVRCALCULATOR_H
#include <QVector>
#include <QObject>
struct AvrResult {
    QVector<double> channel_L;
    QVector<double> channel_R;
    bool valid = false;
};

class AvrCalculator
{

public:
    AvrCalculator(size_t n);
    AvrResult AddCalculatedData(const QVector<double>& channel_a, const QVector<double>& channel_b);
private:
    size_t m_AvgCount;
    size_t m_CurrCount{0};
    QVector<double> m_ChannelLeft;
    QVector<double> m_ChannelRight;
};

#endif // AVRCALCULATOR_H

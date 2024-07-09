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

#include "AvrCalculator.h"
#include <QDebug>

AvrCalculator::AvrCalculator(size_t n)
{
    m_AvgCount = n;
}

AvrResult AvrCalculator::AddCalculatedData(const QVector<double> &channel_a, const QVector<double> &channel_b)
{
    m_ChannelLeft.resize(channel_a.size());
    m_ChannelRight.resize(channel_b.size());

    for (size_t i = 0; i < channel_a.size(); i++){
        m_ChannelLeft[i] += channel_a[i];
    }

    for (size_t i = 0; i < channel_b.size(); i++){
        m_ChannelRight[i] += channel_b[i];
    }

    AvrResult result;
    m_CurrCount += 1;
    if (m_CurrCount == m_AvgCount){
        auto div = [this](double & num){
            num = num / this -> m_AvgCount;
        };

        std::for_each(m_ChannelLeft.begin(), m_ChannelLeft.end(), div);
        std::for_each(m_ChannelRight.begin(), m_ChannelRight.end(), div);

        result.valid = true;
        result.channel_L = std::move(m_ChannelLeft);
        result.channel_R = std::move(m_ChannelRight);

        m_CurrCount = 0;
        m_ChannelLeft.clear();
        m_ChannelRight.clear();
    }

    return result;
}

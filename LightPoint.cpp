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

#include "lightpoint.h"
#include "qpainter.h"
#include "qevent.h"
#include "qtimer.h"
#include "qdebug.h"

LightPoint::LightPoint(QWidget *parent) : QWidget(parent)
{
    m_nStep = 10;
    m_nInterval = 50;
    m_bgColor = QColor(255, 0, 0);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_nOffset = 0;
    m_bAdd = true;
}

LightPoint::~LightPoint()
{
    if (m_pTimer->isActive()) {
        m_pTimer->stop();
    }
}

void LightPoint::setStop()
{
    m_pTimer->stop();
}

void LightPoint::setStart()
{
    m_pTimer->start();
}

void LightPoint::paintEvent(QPaintEvent *)
{
    int width = this->width();
    int height = this->height();
    int side = qMin(width, height);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.translate(width / 2, height / 2);
    painter.scale(side / 200.0, side / 200.0);
    drawBg(&painter);
}

void LightPoint::drawBg(QPainter *painter)
{
    int radius = 90;
    painter->save();

    QRadialGradient g(QPoint(0, 0), radius);

    (m_nOffset < 70 && m_bAdd) ? (m_nOffset += m_nStep) : (m_bAdd = false);
    (m_nOffset > 0 && !m_bAdd) ? (m_nOffset -= m_nStep) : (m_bAdd = true);

    m_bgColor.setAlpha(255);
    g.setColorAt(0.1, m_bgColor);
    m_bgColor.setAlpha(100 + m_nOffset);
    g.setColorAt(0.3, m_bgColor);
    m_bgColor.setAlpha(50 + m_nOffset);
    g.setColorAt(0.6, m_bgColor);
    m_bgColor.setAlpha(0);
    g.setColorAt(1.0, m_bgColor);

    painter->setPen(Qt::NoPen);
    painter->setBrush(g);
    painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);

    painter->restore();
}

int LightPoint::getStep() const
{
    return this->m_nStep;
}

int LightPoint::getInterval() const
{
    return this->m_nInterval;
}

QColor LightPoint::getBgColor() const
{
    return this->m_bgColor;
}

QSize LightPoint::sizeHint() const
{
    return QSize(100, 100);
}

QSize LightPoint::minimumSizeHint() const
{
    return QSize(5, 5);
}

void LightPoint::slotSetStep(int step)
{
    if (this->m_nStep != step) {
        this->m_nStep = step;
        update();
    }
}

void LightPoint::slotSetInterval(int interval)
{
    if (this->m_nInterval != interval) {
        this->m_nInterval = interval;
        m_pTimer->setInterval(interval);
        update();
    }
}

void LightPoint::slotSetBgColor(const QColor &bgColor)
{
    if (this->m_bgColor != bgColor) {
        this->m_bgColor = bgColor;
        update();
    }
}

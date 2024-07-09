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

#ifndef LIGHTPOINT_H
#define LIGHTPOINT_H
#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT LightPoint : public QWidget
        #else
class LightPoint : public QWidget
        #endif

{
    Q_OBJECT
    Q_PROPERTY(int step READ getStep WRITE slotSetStep)
    Q_PROPERTY(int interval READ getInterval WRITE slotSetInterval)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE slotSetBgColor)

public:
    explicit LightPoint(QWidget *parent = 0);
    ~LightPoint();
    void setStop();
    void setStart();
protected:
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);

private:
    int m_nStep;
    int m_nInterval;
    QColor m_bgColor;
    int m_nOffset;
    bool m_bAdd;
public:
    QTimer *m_pTimer;

public:
    int getStep()                   const;
    int getInterval()               const;
    QColor getBgColor()             const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public slots:
    void slotSetStep(int step);
    void slotSetInterval(int interval);
    void slotSetBgColor(const QColor &bgColor);
};

#endif // LIGHTPOINT_H

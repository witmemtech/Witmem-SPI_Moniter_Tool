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

#ifndef FREQUENCYTOOLSMAINWINDOW_H
#define FREQUENCYTOOLSMAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#include "qcustomplot.h"
#include "DefineData.h"
#include <fstream>
#include "AvrCalculator.h"
#include "ArraySettingsDialog.h"
#include "TimeDomainSettingDialog.h"
#include "FrequencyDomainSettingDialog.h"
#include "ArrayLegedLabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FrequencyToolsMainWindow; }
QT_END_NAMESPACE

class FrequencyToolsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FrequencyToolsMainWindow(QWidget *parent = nullptr);
    ~FrequencyToolsMainWindow();
signals:
    void signalnOpenSpi();
    void signalTimeDomainDataCfg(TimeDomainData &data);
    void signalFreDomainDataCfg(FrequencyData &data);
    void signalArrayDomainDataCfg(ArrayDomainData &data);
private slots:
    void on_combo_scale_type_currentIndexChanged(int index);

    void on_combo_scale_type_G1G2_Y_currentIndexChanged(int index);

    void on_combo_scale_type_Math_Y_currentIndexChanged(int index);

    void on_pauseBtn_clicked(bool checked);

    void on_averageVlaueResBtn_clicked();

    void on_exportDataCheckbox_clicked(bool checked);

    void on_defaultScalingBtn_clicked();

    void on_AutoScalingBtn_clicked();

    void on_MathTypeBox_currentIndexChanged(int index);

    void slotOpenSpiRes(int ret);

    void onShowMessage(QString title, QString message);

    void on_exportFilePostionBtn_clicked();

    void on_dBNumBtn_clicked();

    void on_saveWAVFilePostionBtn_clicked();

    void on_SaveWAVFileCheckBox_clicked(bool checked);

    void on_TimeDomainSettingBtn_clicked();

    void on_FreDomainSettingBtn_clicked();

    void on_ArrayTypeSettingBtn_clicked();

    void on_dBNumcfgEdit_returnPressed();

    void on_dBNumCfgMathEdit_returnPressed();

    void on_MathTypeBox_activated(int index);

    void on_tabWidget_currentChanged(int index);

    void on_saveBinFilePostionBtn_clicked();

    void on_SaveBinFileCheckBox_clicked(bool checked);

    void on_arrayAutoScalingBtn_clicked();

    void on_arrayDefaultScalingBtn_clicked();

    void on_arrayPauseBtn_clicked(bool checked);

public slots:
    void slotDrawG1G2Data(QVector<double> &clData, QVector<double> &crData);
    void slotFrequencyData(FrequencyData &cfgData);
    void slotErrorDataCount(int nErrorCount, bool isRight);
    void slotChannelSelectBtnChicked();
    void slotDeviceStatus(int status);
    void slotArrayData(QVector<float>floatData);
private:
    void initUI();
    void initCustomPlot();
    void drawAsxis();
    void drawArrayAsxis();
    void initLegendLabel();
    void saveInputData();
    void readCfgFileData();
    void drawData(int numOfBars, QVector<double> &clData, QVector<double> &crData);
    void drawMathData(int numOfBars, QVector<double> &clData, QVector<double> &crData);
    void setTimeTabBarSatusInfo(TimeDomainData data);
    void setFreTabBarSatusInfo(FrequencyData data);
    void setArrayTabBarSatusInfo(ArrayDomainData data);

protected: 
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);

signals:
    void signalIsCheckBoxStatus(bool isCheck);
    void signalIsCheckBoxWavStatus(bool isCheck);
    void signalIsCheckBoxBinStatus(bool isCheck);
private:
    Ui::FrequencyToolsMainWindow *ui;
    FrequencyDomainSettingDialog *m_pFreDomainDialog;
    TimeDomainSettingDialog *m_pTimeDomainDialg;
    ArraySettingsDialog *m_pArrayDomainDialg;
private:
    QCustomPlot * m_pG1G2Plot{new QCustomPlot(this)};
    QCustomPlot * m_pMathPlot{new QCustomPlot(this)};
    QCustomPlot * m_pArrayPlot{new QCustomPlot(this)};

    QVector<double> m_vectorXAxisTicks;
    QVector<double> m_vectorCLData, m_vectorCRData;

    ENUM_ScaleType  m_xAxisScaleType{ScaleType_Linear}
    , m_yG1G2AxisScaleType{ScaleType_Linear}
    , m_yMathAxisScaleType{ScaleType_Linear};

    ENUM_BitType   m_BitType{BIT_16};

    int m_nFttSize = 0;
    double m_dLogBitType = 1;
    double m_dMathLogBitType = 1;
    bool m_bIsPause = false;
    bool m_arrayIsPause = false;
    std::unique_ptr<AvrCalculator> m_var;

    FrequencyData m_sFreCfgData;
    TimeDomainData m_sTimeCfgData;
    ArrayDomainData m_sArrayCfgData;
    cfgUIData m_sCfgUiData;
    int m_nErrCount = 0;

    QThread *m_pSerialThread;
    bool m_bFirstMsgFrame = true;
    bool m_bDefault = false;
    int m_nArrayXAxisStart = 0;
    bool m_bFirstArrayFrame = true;
    bool m_mIsFirstRescale =true;
    QTimer*  m_pDataTimer;
    double m_dRangeWidth = 0;
    double m_dArrayXAsixCount = 0;
    QVector<QCPGraph*> graphs;
    int  m_nPreGraphsCount = 0;
};
#endif // FREQUENCYTOOLSMAINWINDOW_H

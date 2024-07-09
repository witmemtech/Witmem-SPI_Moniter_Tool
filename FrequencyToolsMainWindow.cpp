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

#include "FrequencyToolsMainWindow.h"
#include "ui_FrequencyToolsMainWindow.h"
#include "SerialWork.h"

FrequencyToolsMainWindow::FrequencyToolsMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FrequencyToolsMainWindow)
{
    ui->setupUi(this);
    ui->dBNumBtn->hide();
    initUI();
    initCustomPlot();

    m_pSerialThread = new QThread;
    SerialWork* serialWork = SerialWork::instance();
    serialWork->moveToThread(m_pSerialThread);
    connect(this,&FrequencyToolsMainWindow::signalTimeDomainDataCfg,serialWork,&SerialWork::signalTimeDomainData);
    connect(this,&FrequencyToolsMainWindow::signalTimeDomainDataCfg,serialWork,&SerialWork::slotTimtDomainDataCfg);
    connect(this,&FrequencyToolsMainWindow::signalFreDomainDataCfg,serialWork,&SerialWork::slotFreDomainDataCfg);
    connect(this,&FrequencyToolsMainWindow::signalArrayDomainDataCfg,serialWork,&SerialWork::slotArrayDomainDataCfg);

    connect(m_pSerialThread, &QThread::finished,serialWork, &SerialWork::deleteLater);
    connect(serialWork, &SerialWork::signalErrorCount,this,&FrequencyToolsMainWindow::slotErrorDataCount);
    connect(serialWork, &SerialWork::signalSendG1G2Data,this,&FrequencyToolsMainWindow::slotDrawG1G2Data);
    connect(serialWork, &SerialWork::signalArrayData,this,&FrequencyToolsMainWindow::slotArrayData);
    connect(this, &FrequencyToolsMainWindow::signalnOpenSpi,serialWork, &SerialWork::slotOpenSpi);
    connect(serialWork,&SerialWork::signalResultOpenSpi,this,&FrequencyToolsMainWindow::slotOpenSpiRes);
    connect(this, &FrequencyToolsMainWindow::signalIsCheckBoxStatus,serialWork,&SerialWork::slotSaveDataToCsvFile);
    connect(this, &FrequencyToolsMainWindow::signalIsCheckBoxWavStatus,serialWork,&SerialWork::slotSaveDataToWavFile);
    connect(this, &FrequencyToolsMainWindow::signalIsCheckBoxBinStatus,serialWork,&SerialWork::slotSaveDataToBinFile);
    connect(serialWork, &SerialWork::signalShowMessage, this, &FrequencyToolsMainWindow::onShowMessage);
    connect(serialWork, &SerialWork::signalDeviceStatus, this, &FrequencyToolsMainWindow::slotDeviceStatus);
    emit signalnOpenSpi();
    ui->tabWidget->setCurrentIndex(0);
    on_tabWidget_currentChanged(0);

    on_SaveWAVFileCheckBox_clicked(true);
    on_exportDataCheckbox_clicked(m_sCfgUiData.isExportData);

    m_pSerialThread->start();
    ui->tabWidget->removeTab(2);
}

FrequencyToolsMainWindow::~FrequencyToolsMainWindow()
{
    delete ui;
    m_pSerialThread->quit();
    m_pSerialThread->wait();
    if(m_pSerialThread != nullptr){
        delete m_pSerialThread;
        m_pSerialThread = nullptr;
    }
}


void FrequencyToolsMainWindow::on_combo_scale_type_currentIndexChanged(int index)
{
    if(0 == index){
        m_xAxisScaleType = ScaleType_Linear;
    }else{
        m_xAxisScaleType = ScaleType_Logarithmic;
    }

    drawAsxis();
    drawData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    drawMathData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    on_AutoScalingBtn_clicked();
}


void FrequencyToolsMainWindow::on_combo_scale_type_G1G2_Y_currentIndexChanged(int index)
{
    if(0 == index){
        m_yG1G2AxisScaleType = ScaleType_Linear;
    }else{
        m_yG1G2AxisScaleType = ScaleType_Logarithmic;
    }

    drawAsxis();
    drawData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    drawMathData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    on_AutoScalingBtn_clicked();
}


void FrequencyToolsMainWindow::on_combo_scale_type_Math_Y_currentIndexChanged(int index)
{
    if(0 == index){
        m_yMathAxisScaleType = ScaleType_Linear;
    }else{
        m_yMathAxisScaleType = ScaleType_Logarithmic;
    }

    drawAsxis();
    drawData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    drawMathData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    on_AutoScalingBtn_clicked();
}

void FrequencyToolsMainWindow::initUI()
{
    ui->errorCountTotalLabel->setText(QString("数据错误:<font color = #FF0000> %1 </font>次").arg(m_nErrCount));
    ui->FiveWidget->hide();
    ui->FourWidget->hide();
    ui->ThreeWidget->hide();

    m_pFreDomainDialog = new  FrequencyDomainSettingDialog(this);
    m_sFreCfgData = m_pFreDomainDialog->getFreCfgData();

    m_pTimeDomainDialg = new TimeDomainSettingDialog(this);
    m_sTimeCfgData = m_pTimeDomainDialg->getTimeCfgData();

    m_pArrayDomainDialg = new ArraySettingsDialog(this);
    m_sArrayCfgData = m_pArrayDomainDialg->getArrayCfgData();

    qRegisterMetaType<QVector<double>>("QVector<double>&");
    qRegisterMetaType<FrequencyData>("FrequencyData&");
    qRegisterMetaType<TimeDomainData>("TimeDomainData&");
    qRegisterMetaType<ArrayDomainData>("ArrayDomainData&");

    initLegendLabel();
    ui->averageVlaueEdit->setText("1");

    QRegExp rx("^([1-9]|[1-9]\\d|1\\d{2}|200)$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->averageVlaueEdit->setValidator(pReg);

    m_var = std::make_unique<AvrCalculator>(1);
    on_exportDataCheckbox_clicked(false);

    connect(ui->ChannelG1Box,&QRadioButton::clicked,this,&FrequencyToolsMainWindow::slotChannelSelectBtnChicked);
    connect(ui->ChannelG2Box,&QRadioButton::clicked,this,&FrequencyToolsMainWindow::slotChannelSelectBtnChicked);

    QStringList YAxisList;
    YAxisList.clear();
    YAxisList <<"线性"<<"对数";
    ui->combo_scale_type_G1G2_Y->addItems(YAxisList);
    ui->combo_scale_type_Math_Y->addItems(YAxisList);
    ui->combo_scale_type_array_Y->addItems(YAxisList);

    readCfgFileData();
    ui->averageVlaueEdit->setText(QString::number(m_sCfgUiData.nAvgNum));
    ui->exportDataCheckbox->setChecked(m_sCfgUiData.isExportData);
    ui->combo_scale_type_G1G2_Y->setCurrentIndex(m_sCfgUiData.nG1G2YAisType);
    ui->combo_scale_type_Math_Y->setCurrentIndex(m_sCfgUiData.nMathYAisType);
    ui->combo_scale_type->setCurrentIndex(m_sCfgUiData.nAxisType);
    ui->MathTypeBox->setCurrentIndex(m_sCfgUiData.nChannelMath);
    QRegExpValidator* validator = new QRegExpValidator(QRegExp("[1-9]\\d*"), ui->dBNumcfgEdit);
    ui->dBNumcfgEdit->setValidator(validator);
}

void FrequencyToolsMainWindow::initCustomPlot()
{
    for(int i = 0; i < 3;i++){
        QCustomPlot *pCustomPlot;
        if(i == 0){
            pCustomPlot = m_pMathPlot;
            ui->verticalLayout_Math->addWidget(pCustomPlot);
        }else if(i == 1){
            pCustomPlot = m_pG1G2Plot;
            ui->verticalLayout_G1G2->addWidget(pCustomPlot);
        }else if(i == 2){
            pCustomPlot = m_pArrayPlot;
            ui->verticalLayout_Arrray->addWidget(pCustomPlot);
        }

        pCustomPlot->setBackground(QColor(50, 50, 50));
        pCustomPlot->xAxis->setVisible(true);
        pCustomPlot->xAxis->setTickLabelColor(Qt::white);
        pCustomPlot->xAxis->setLabelColor(QColor(0, 160, 230));
        pCustomPlot->xAxis->setBasePen(QPen(QColor(255, 255, 255)));
        pCustomPlot->xAxis->setTickPen(QPen(QColor(255, 255, 255)));
        pCustomPlot->xAxis->setSubTickPen(QPen(QColor(255, 255, 255)));
        QFont xFont = pCustomPlot->xAxis->labelFont();
        xFont.setPixelSize(20);
        pCustomPlot->xAxis->setLabelFont(xFont);
        pCustomPlot->xAxis->setLabel("Hz");
        pCustomPlot->xAxis->setTickLengthIn(0);
        pCustomPlot->xAxis->setTickLengthOut(7);
        pCustomPlot->xAxis->setSubTickLengthIn(0);
        pCustomPlot->xAxis->setSubTickLengthOut(3);
        pCustomPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);


        pCustomPlot->yAxis->setTickLabelColor(Qt::white);
        pCustomPlot->yAxis->setLabelColor(QColor(0, 160, 230));
        pCustomPlot->yAxis->setBasePen(QPen(QColor(255, 255, 255)));
        pCustomPlot->yAxis->setTickPen(QPen(QColor(255, 255, 255)));
        pCustomPlot->yAxis->setSubTickPen(QPen(QColor(255, 255, 255)));
        QFont yFont = pCustomPlot->yAxis->labelFont();
        yFont.setPixelSize(20);
        pCustomPlot->yAxis->setLabelFont(yFont);
        pCustomPlot->xAxis->grid()->setPen(QPen(QColor(0, 170, 120,100), 0.5, Qt::PenStyle::SolidLine));
        pCustomPlot->yAxis->grid()->setPen(QPen(QColor(0, 170, 120,100), 0.5, Qt::PenStyle::SolidLine));

        pCustomPlot->yAxis->setTickLengthIn(0);
        pCustomPlot->yAxis->setTickLengthOut(7);
        pCustomPlot->yAxis->setSubTickLengthIn(0);
        pCustomPlot->yAxis->setSubTickLengthOut(3);
        m_pG1G2Plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
        m_pArrayPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
        pCustomPlot-> setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    }
    connect(m_pG1G2Plot, &QCustomPlot::mousePress, m_pMathPlot, &QCustomPlot::do_mousePressEvent);
    connect(m_pG1G2Plot, &QCustomPlot::mouseMove, m_pMathPlot, &QCustomPlot::do_mouseMoveEvent);
    connect(m_pG1G2Plot, &QCustomPlot::mouseRelease, m_pMathPlot, &QCustomPlot::do_mouseReleaseEvent);
    connect(m_pG1G2Plot, &QCustomPlot::mouseWheel, m_pMathPlot, &QCustomPlot::do_wheelEvent);

    connect(m_pMathPlot, &QCustomPlot::mousePress, m_pG1G2Plot, &QCustomPlot::do_mousePressEvent);
    connect(m_pMathPlot, &QCustomPlot::mouseMove, m_pG1G2Plot, &QCustomPlot::do_mouseMoveEvent);
    connect(m_pMathPlot, &QCustomPlot::mouseRelease, m_pG1G2Plot, &QCustomPlot::do_mouseReleaseEvent);
    connect(m_pMathPlot, &QCustomPlot::mouseWheel, m_pG1G2Plot, &QCustomPlot::do_wheelEvent);

    m_bFirstMsgFrame = true;
}

void FrequencyToolsMainWindow::drawAsxis()
{
    m_pG1G2Plot->clearPlottables();
    m_pMathPlot->clearPlottables();
    m_pG1G2Plot->rescaleAxes(true);
    m_pMathPlot->rescaleAxes(true);

    double  x_Range = m_sFreCfgData.dSamplingRate / 2;
    if (ScaleType_Linear == m_xAxisScaleType){
        m_pG1G2Plot->xAxis->setScaleType(QCPAxis::ScaleType::stLinear);
        m_pMathPlot->xAxis->setScaleType(QCPAxis::ScaleType::stLinear);
        QSharedPointer<QCPAxisTicker> intTicker(new QCPAxisTicker);
        intTicker->setTickCount(11);
        m_pG1G2Plot->xAxis->setTicker(intTicker);
        m_pMathPlot->xAxis->setTicker(intTicker);
        m_pG1G2Plot->xAxis->setRange(0,x_Range);
        m_pMathPlot->xAxis->setRange(0,x_Range);
    }else{
        m_pG1G2Plot->xAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
        m_pMathPlot->xAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);

        QSharedPointer<QCPAxisTickerLog> lgTicker(new QCPAxisTickerLog);
        lgTicker->setTickCount(11);
        m_pG1G2Plot->xAxis->setTicker(lgTicker);
        m_pMathPlot->xAxis->setTicker(lgTicker);
        m_pG1G2Plot->xAxis->setRange(10, x_Range);
        m_pMathPlot->xAxis->setRange(10, x_Range);
    }
    m_pG1G2Plot->yAxis->ticker()->setTickCount(10);
    m_pMathPlot->yAxis->ticker()->setTickCount(10);

    if(ScaleType_Linear == m_yG1G2AxisScaleType){
        m_pG1G2Plot->yAxis->setRangeReversed(false);
        m_pG1G2Plot->yAxis->setScaleType(QCPAxis::ScaleType::stLinear);
        if(m_bFirstMsgFrame == true || m_bDefault == true){
            if(m_sFreCfgData.nDataType == 1){
                m_pG1G2Plot->yAxis->setRange(FLOAT_YAXIS_MIN_RANG,FLOAT_YAXIS_MAX_RANG);
            }else{
                m_pG1G2Plot->yAxis->setRange(YAXIS_MIN_RANG,YAXIS_MAX_RANG);
            }
        }
        m_pG1G2Plot->yAxis->setLabel("G1/G2\t\t");
    }else{
        QSharedPointer<QCPAxisTicker> dBTicker(new QCPAxisTicker);
        dBTicker->setTickCount(10);
        m_pG1G2Plot->yAxis->setTicker(dBTicker);
        if(m_bFirstMsgFrame == true || m_bDefault == true){
            m_pG1G2Plot->yAxis->setRange(LOG_YAXIS_MIN_RANG, LOG_YAXIS_MAX_RANG);
        }
        m_pG1G2Plot->yAxis->setLabel("G1/G2\t\tdB");
    }

    if(ScaleType_Linear == m_yMathAxisScaleType) {
        m_pMathPlot->yAxis->setScaleType(QCPAxis::ScaleType::stLinear);
        if(m_bFirstMsgFrame == true || m_bDefault == true){
            if(m_sFreCfgData.nDataType == 1){
                m_pMathPlot->yAxis->setRange(FLOAT_YAXIS_MIN_RANG,FLOAT_YAXIS_MAX_RANG);
            }else{
                m_pMathPlot->yAxis->setRange(YAXIS_MIN_RANG,YAXIS_MAX_RANG);
            }
        }

        m_pMathPlot->yAxis->setLabel("Math\t\t");
    }else{
        if(m_bFirstMsgFrame == true || m_bDefault == true){
            m_pMathPlot->yAxis->setRange(LOG_YAXIS_MIN_RANG, LOG__MATH_YAXIS_MAX_RANG);
        }
        m_pMathPlot->yAxis->setLabel("Math\t\tdB");
    }

    m_pG1G2Plot->xAxis->setSubTicks(true);
    m_pG1G2Plot->xAxis->setTicks(true);

    m_pMathPlot->yAxis->setRange(m_pMathPlot->yAxis->range().lower,m_pMathPlot->yAxis->range().upper);
    m_pG1G2Plot->yAxis->setRange(m_pG1G2Plot->yAxis->range().lower,m_pG1G2Plot->yAxis->range().upper);

    m_pG1G2Plot->replot(QCustomPlot::rpQueuedReplot);
    m_pMathPlot->replot(QCustomPlot::rpQueuedReplot);
}

void FrequencyToolsMainWindow::drawArrayAsxis()
{
    m_pArrayPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
    m_pArrayPlot->legend->setVisible(false);
    m_pArrayPlot->xAxis->setRange(0,100);
    m_pArrayPlot->xAxis->setLabel("");

    if(m_sArrayCfgData.nXAsixType == TIME_TYPE){
        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        m_pArrayPlot->xAxis->setTicker(timeTicker);
    }else{
        QSharedPointer<QCPAxisTicker> timeTicker(new QCPAxisTicker);
        m_pArrayPlot->xAxis->setTicker(timeTicker);
    }
    int nGraphCount = m_sArrayCfgData.dArrayMembersNum;
    for(int  i = 0;i < graphs.size(); i++){
        m_pArrayPlot->removeGraph(graphs.at(i));
    }
    graphs.clear();
    QList<ArrayLegedLabel*> legendlist = this->findChildren<ArrayLegedLabel*>();
    for(int i = 0 ; i< legendlist.size();i++){
        ui->verticalLayout_len->removeWidget(legendlist.at(i));
        delete legendlist.at(i);
    }

    for(int i = 0;i < nGraphCount; i++){
        QCPGraph* graph = m_pArrayPlot->addGraph();
        graph->rescaleAxes(true);
        graph->setPen(QPen(QColor::fromHsv((i * 30) % 360, 200, 220)));
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,4));
        graphs.append(graph);
        ArrayLegedLabel *legend =  new ArrayLegedLabel(this);
        int hue = (i * 30) % 360;
        int saturation = 255;
        int value = 255;
        QColor rgbColor;
        rgbColor.setHsv(hue, saturation, value);
        QString hexColor = rgbColor.name(QColor::HexRgb);
        legend->setLegendInfo(QString("变量%1").arg(i+1),hexColor);
        ui->verticalLayout_len->addWidget(legend);
        graphs[i]->setVisible(false);
    }

    for(int i = 0; i <  m_sArrayCfgData.chickedList.size();i++){
        QCheckBox *box = m_sArrayCfgData.chickedList.at(i);
        QString strObjname = box->objectName().replace("ChannelCheckBox_", "").trimmed();
        int index = strObjname.toInt();
        if((index -1) < graphs.size()){
            if(graphs[index -1] != nullptr){
                graphs[index -1]->setVisible(true);
            }
        }
    }
    m_pArrayPlot->yAxis->rescale(true);
    m_pDataTimer = new QTimer(this);
    connect(m_pDataTimer,&QTimer::timeout,this,[=](){
        if(m_arrayIsPause == false){
            double key = m_dArrayXAsixCount;
            m_dRangeWidth = m_pArrayPlot->xAxis->range().size();
            if(m_sArrayCfgData.nXAsixType == TIME_TYPE){
                key = m_dArrayXAsixCount / m_sArrayCfgData.nPointCountTime;
            }

            if (key > m_dRangeWidth){
                m_pArrayPlot->xAxis->setRange(key - m_dRangeWidth, key);
            }else{
                m_pArrayPlot->xAxis->setRange(0, m_dRangeWidth, Qt::AlignLeft);
            }
            m_pArrayPlot->replot(QCustomPlot::rpQueuedReplot);
        }
    });
    m_pDataTimer->start(100);
}

void FrequencyToolsMainWindow::initLegendLabel()
{
    ui->G1LegendLabel->setStyleSheet("border-radius:4px;background:#5879A9;");
    ui->G2LegendLabel->setStyleSheet("border-radius:4px;background:#376D54;");
    ui->G1G2LegendLabel->setStyleSheet("border-radius:4px;background:#64AAAB;");
    ui->G1G2PoorLabel->setStyleSheet("border-radius:4px;background:#1444ac;");
}

void FrequencyToolsMainWindow::saveInputData()
{
    QString strCfgFilePath;
    strCfgFilePath = QCoreApplication::applicationDirPath();
    strCfgFilePath += QDir::separator();
    strCfgFilePath += "cfgFile.json";
    QFile cfgFile(strCfgFilePath);

    QJsonObject json;
    QJsonObject dBNumRange;
    QJsonObject dBNumRangeMath;
    QJsonObject dBNumRangeOld;
    QJsonObject dBNumRangeOldMath;

    if(!cfgFile.exists()){
        dBNumRange["Float"] = 1;
        dBNumRange["Uint32"] = 4294967295;
        dBNumRange["int32"] = 2147483647;
        dBNumRange["Uint16"] = 65535;
        dBNumRange["int16"] = 32767;
        dBNumRange["Uint8"] = 255;
        dBNumRange["int8"] = 127;

        dBNumRangeMath["Float"] = 1;
        dBNumRangeMath["Uint32"] = 4294967295;
        dBNumRangeMath["int32"] = 2147483647;
        dBNumRangeMath["Uint16"] = 65535;
        dBNumRangeMath["int16"] = 32767;
        dBNumRangeMath["Uint8"] = 255;
        dBNumRangeMath["int8"] = 127;
    }else{
        QFile cfgFile(strCfgFilePath);
        if (cfgFile.open(QIODevice::ReadOnly)) {
            QByteArray data = cfgFile.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject json = doc.object();
            dBNumRangeOld = json["dBNumRange"].toObject();
            dBNumRangeOldMath = json["dBNumRangeMath"].toObject();
        }
        switch (m_sFreCfgData.nDataType) {
        case 0:
            break;
        case 1:
            dBNumRange["Float"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Uint32"] = dBNumRangeOld.value("Uint32").toDouble();
            dBNumRange["int32"] =  dBNumRangeOld.value("int32").toDouble();
            dBNumRange["Uint16"] =  dBNumRangeOld.value("Uint16").toDouble();
            dBNumRange["int16"] =  dBNumRangeOld.value("int16").toDouble();
            dBNumRange["Uint8"] =  dBNumRangeOld.value("Uint8").toDouble();
            dBNumRange["int8"] =  dBNumRangeOld.value("int8").toDouble();

            dBNumRangeMath["Float"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Uint32"] = dBNumRangeOldMath.value("Uint32").toDouble();
            dBNumRangeMath["int32"] =  dBNumRangeOldMath.value("int32").toDouble();
            dBNumRangeMath["Uint16"] =  dBNumRangeOldMath.value("Uint16").toDouble();
            dBNumRangeMath["int16"] =  dBNumRangeOldMath.value("int16").toDouble();
            dBNumRangeMath["Uint8"] =  dBNumRangeOldMath.value("Uint8").toDouble();
            dBNumRangeMath["int8"] =  dBNumRangeOldMath.value("int8").toDouble();
            break;
        case 2:
            dBNumRange["Uint32"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Float"] = dBNumRangeOld.value("Float").toDouble();
            dBNumRange["int32"] =  dBNumRangeOld.value("int32").toDouble();
            dBNumRange["Uint16"] =  dBNumRangeOld.value("Uint16").toDouble();
            dBNumRange["int16"] =  dBNumRangeOld.value("int16").toDouble();
            dBNumRange["Uint8"] =  dBNumRangeOld.value("Uint8").toDouble();
            dBNumRange["int8"] =  dBNumRangeOld.value("int8").toDouble();

            dBNumRangeMath["Uint32"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Float"] = dBNumRangeOldMath.value("Float").toDouble();
            dBNumRangeMath["int32"] =  dBNumRangeOldMath.value("int32").toDouble();
            dBNumRangeMath["Uint16"] =  dBNumRangeOldMath.value("Uint16").toDouble();
            dBNumRangeMath["int16"] =  dBNumRangeOldMath.value("int16").toDouble();
            dBNumRangeMath["Uint8"] =  dBNumRangeOldMath.value("Uint8").toDouble();
            dBNumRangeMath["int8"] =  dBNumRangeOldMath.value("int8").toDouble();
            break;
        case 3:
            dBNumRange["int32"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Float"] = dBNumRangeOld.value("Float").toDouble();
            dBNumRange["Uint32"] =  dBNumRangeOld.value("Uint32").toDouble();
            dBNumRange["Uint16"] =  dBNumRangeOld.value("Uint16").toDouble();
            dBNumRange["int16"] =  dBNumRangeOld.value("int16").toDouble();
            dBNumRange["Uint8"] =  dBNumRangeOld.value("Uint8").toDouble();
            dBNumRange["int8"] =  dBNumRangeOld.value("int8").toDouble();

            dBNumRangeMath["int32"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Float"] = dBNumRangeOldMath.value("Float").toDouble();
            dBNumRangeMath["Uint32"] =  dBNumRangeOldMath.value("Uint32").toDouble();
            dBNumRangeMath["Uint16"] =  dBNumRangeOldMath.value("Uint16").toDouble();
            dBNumRangeMath["int16"] =  dBNumRangeOldMath.value("int16").toDouble();
            dBNumRangeMath["Uint8"] =  dBNumRangeOldMath.value("Uint8").toDouble();
            dBNumRangeMath["int8"] =  dBNumRangeOldMath.value("int8").toDouble();
            break;
        case 4:
            dBNumRange["Uint16"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Float"] = dBNumRangeOld.value("Float").toDouble();
            dBNumRange["int32"] =  dBNumRangeOld.value("int32").toDouble();
            dBNumRange["Uint32"] =  dBNumRangeOld.value("Uint32").toDouble();
            dBNumRange["int16"] =  dBNumRangeOld.value("int16").toDouble();
            dBNumRange["Uint8"] =  dBNumRangeOld.value("Uint8").toDouble();
            dBNumRange["int8"] =  dBNumRangeOld.value("int8").toDouble();

            dBNumRangeMath["Uint16"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Float"] = dBNumRangeOldMath.value("Float").toDouble();
            dBNumRangeMath["int32"] =  dBNumRangeOldMath.value("int32").toDouble();
            dBNumRangeMath["Uint32"] =  dBNumRangeOldMath.value("Uint32").toDouble();
            dBNumRangeMath["int16"] =  dBNumRangeOldMath.value("int16").toDouble();
            dBNumRangeMath["Uint8"] =  dBNumRangeOldMath.value("Uint8").toDouble();
            dBNumRangeMath["int8"] =  dBNumRangeOldMath.value("int8").toDouble();
            break;
        case 5:
            dBNumRange["int16"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Float"] = dBNumRangeOld.value("Float").toDouble();
            dBNumRange["int32"] =  dBNumRangeOld.value("int32").toDouble();
            dBNumRange["Uint32"] =  dBNumRangeOld.value("Uint32").toDouble();
            dBNumRange["Uint16"] =  dBNumRangeOld.value("Uint16").toDouble();
            dBNumRange["Uint8"] =  dBNumRangeOld.value("Uint8").toDouble();
            dBNumRange["int8"] =  dBNumRangeOld.value("int8").toDouble();

            dBNumRangeMath["int16"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Float"] = dBNumRangeOldMath.value("Float").toDouble();
            dBNumRangeMath["int32"] =  dBNumRangeOldMath.value("int32").toDouble();
            dBNumRangeMath["Uint32"] =  dBNumRangeOldMath.value("Uint32").toDouble();
            dBNumRangeMath["Uint16"] =  dBNumRangeOldMath.value("Uint16").toDouble();
            dBNumRangeMath["Uint8"] =  dBNumRangeOldMath.value("Uint8").toDouble();
            dBNumRangeMath["int8"] =  dBNumRangeOldMath.value("int8").toDouble();
            break;
        case 6:
            dBNumRange["Uint8"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Float"] = dBNumRangeOld.value("Float").toDouble();
            dBNumRange["int32"] =  dBNumRangeOld.value("int32").toDouble();
            dBNumRange["Uint32"] =  dBNumRangeOld.value("Uint32").toDouble();
            dBNumRange["int16"] =  dBNumRangeOld.value("int16").toDouble();
            dBNumRange["Uint16"] =  dBNumRangeOld.value("Uint16").toDouble();
            dBNumRange["int8"] =  dBNumRangeOld.value("int8").toDouble();

            dBNumRangeMath["Uint8"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Float"] = dBNumRangeOldMath.value("Float").toDouble();
            dBNumRangeMath["int32"] =  dBNumRangeOldMath.value("int32").toDouble();
            dBNumRangeMath["Uint32"] =  dBNumRangeOldMath.value("Uint32").toDouble();
            dBNumRangeMath["int16"] =  dBNumRangeOldMath.value("int16").toDouble();
            dBNumRangeMath["Uint16"] =  dBNumRangeOldMath.value("Uint16").toDouble();
            dBNumRangeMath["int8"] =  dBNumRangeOldMath.value("int8").toDouble();
            break;
        case 7:
            dBNumRange["int8"] = ui->dBNumcfgEdit->text().toDouble();
            dBNumRange["Float"] = dBNumRangeOld.value("Float").toDouble();
            dBNumRange["int32"] =  dBNumRangeOld.value("int32").toDouble();
            dBNumRange["Uint32"] =  dBNumRangeOld.value("Uint32").toDouble();
            dBNumRange["int16"] =  dBNumRangeOld.value("int16").toDouble();
            dBNumRange["Uint16"] =  dBNumRangeOld.value("Uint16").toDouble();
            dBNumRange["Uint8"] =  dBNumRangeOld.value("uint8").toDouble();

            dBNumRangeMath["int8"] = ui->dBNumCfgMathEdit->text().toDouble();
            dBNumRangeMath["Float"] = dBNumRangeOldMath.value("Float").toDouble();
            dBNumRangeMath["int32"] =  dBNumRangeOldMath.value("int32").toDouble();
            dBNumRangeMath["Uint32"] =  dBNumRangeOldMath.value("Uint32").toDouble();
            dBNumRangeMath["int16"] =  dBNumRangeOldMath.value("int16").toDouble();
            dBNumRangeMath["Uint16"] =  dBNumRangeOldMath.value("Uint16").toDouble();
            dBNumRangeMath["Uint8"] =  dBNumRangeOldMath.value("uint8").toDouble();
            break;
        default:
            break;
        }
    }
    json["dBNumRange"] = dBNumRange;
    json["dBNumRangeMath"] = dBNumRangeMath;
    json["G1G2YAisType"] = ui->combo_scale_type_G1G2_Y->currentIndex();
    json["MathYAisType"] = ui->combo_scale_type_Math_Y->currentIndex();
    json["xAxisType"] = ui->combo_scale_type->currentIndex();
    json["IsexportData"] = ui->exportDataCheckbox->isChecked();
    json["AvgNum"] = ui->averageVlaueEdit->text().toInt();
    json["ChannelMath"] = ui->MathTypeBox->currentIndex();
    QJsonDocument doc(json);
    if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        cfgFile.write(doc.toJson());
    }
    cfgFile.close();
}

void FrequencyToolsMainWindow::readCfgFileData()
{
    QString strCfgFilePath;
    strCfgFilePath = QCoreApplication::applicationDirPath();
    strCfgFilePath += QDir::separator();
    strCfgFilePath += "cfgFile.json";

    QFile cfgFile(strCfgFilePath);
    if (cfgFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cfgFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();
        m_sCfgUiData.nAvgNum = json["AvgNum"].toInt();
        m_sCfgUiData.dbNum = json["dBNum"].toDouble();
        m_sCfgUiData.isExportData = json["IsexportData"].toBool();
        m_sCfgUiData.nAxisType = json["xAxisType"].toInt();
        m_sCfgUiData.nG1G2YAisType = json["G1G2YAisType"].toInt();
        m_sCfgUiData.nMathYAisType = json["MathYAisType"].toInt();
        m_sCfgUiData.nChannelMath = json["ChannelMath"].toInt();
    }
    cfgFile.close();
}
void FrequencyToolsMainWindow::drawData(int numOfBars, QVector<double> &clData, QVector<double> &crData)
{
    if(clData.isEmpty() || crData.isEmpty()){return;}
    m_vectorXAxisTicks.clear();
    m_pG1G2Plot->clearPlottables();

    double rangeMax = m_sFreCfgData.dSamplingRate / 2;
    double tick = 0;
    for(int  i = 0; i < m_nFttSize;i++){
        tick += rangeMax / m_nFttSize;
        m_vectorXAxisTicks.append(tick - (rangeMax / m_nFttSize /2));
    }

    QCPAxis *xAxis = m_pG1G2Plot->xAxis;
    QCPAxis *yAxis = m_pG1G2Plot->yAxis;

    double barWidth = rangeMax/numOfBars;
    QCPBars *bars_base = new QCPBars(xAxis, yAxis);
    bars_base->setName("Bars");
    bars_base->setPen(QPen(QColor("#64AAAB").lighter(120)));
    bars_base->setBrush(QColor("#64AAAB"));
    bars_base->setWidth(barWidth);

    QCPBars *bars_above_CL = new QCPBars(xAxis, yAxis);
    bars_above_CL->setPen(QPen(QColor("#5879A9").lighter(120)));
    bars_above_CL->setBrush(QColor("#5879A9"));
    bars_above_CL->setWidth(barWidth);
    QCPBars *bars_above_CR = new QCPBars(xAxis, yAxis);
    bars_above_CR->setPen(QPen(QColor("#376D54").lighter(120)));
    bars_above_CR->setBrush(QColor("#376D54"));
    bars_above_CR->setWidth(barWidth);
    QVector<double> baseData, clData1, crData1,clLogData,crLogData;
    if(numOfBars != clData.size() || numOfBars != crData.size()) {return;}

    if(ScaleType_Linear == m_yG1G2AxisScaleType){
        for (int i = 0; i < numOfBars; ++i){
            double delta = clData[i] - crData[i];
            if(delta>=0){
                baseData << crData[i];
            }else{
                baseData << clData[i];
            }
            if(delta >= 0){
                clData1 << delta;
                crData1 << 0;
            }else{
                crData1 << abs(delta);
                clData1 << 0;
            }
        }
    }else{
        bars_base->setBaseValue(LOG_BASE_VALUE);
        bars_above_CR->setBaseValue(LOG_BASE_VALUE);
        bars_above_CL->setBaseValue(LOG_BASE_VALUE);
        double cl_dB, cr_dB;
        for (int i = 0; i < numOfBars; ++i) {
            if(clData[i] == 0){
                cl_dB = 0;
            }else{
                cl_dB = 20 * log10(clData[i]/m_dLogBitType) + abs(bars_base->baseValue());
            }

            if(crData[i] == 0){
                cr_dB = 0;
            }else{
                cr_dB = 20 * log10(crData[i]/m_dLogBitType) + abs(bars_base->baseValue());
            }

            clLogData << cl_dB;
            crLogData << cr_dB;
            double delta = cl_dB - cr_dB;
            if(delta >= 0){
                baseData << cr_dB;
            }else{
                baseData << cl_dB;
            }

            if(delta >= 0){
                clData1 << -delta;
                crData1 << 0;
            }else{
                crData1 << delta;
                clData1 << 0;
            }
        }
    }

    if(ui->ChannelG1Box->isChecked()){
        if(ScaleType_Linear == m_yG1G2AxisScaleType){
            bars_base->setData(m_vectorXAxisTicks, clData);
        }else{
            bars_base->setData(m_vectorXAxisTicks, clLogData);
        }
        bars_base->setPen(QPen(QColor("#5879A9").lighter(120)));
        bars_base->setBrush(QColor("#5879A9"));
    }else if(ui->ChannelG2Box->isChecked()){
        if(ScaleType_Linear == m_yG1G2AxisScaleType){
            bars_base->setData(m_vectorXAxisTicks, crData);
        }else{
            bars_base->setData(m_vectorXAxisTicks, crLogData);
        }

        bars_base->setPen(QPen(QColor("#376D54").lighter(120)));
        bars_base->setBrush(QColor("#376D54"));
    }else if(ui->ChannelG2Box->isChecked() && ui->ChannelG1Box->isChecked()){
        bars_above_CL->setData(m_vectorXAxisTicks, clData1);
        bars_above_CR->setData(m_vectorXAxisTicks, crData1);
        bars_base->setData(m_vectorXAxisTicks, baseData);
        bars_above_CL->moveAbove(bars_base);
        bars_above_CR->moveAbove(bars_base);
    }

    m_pG1G2Plot->replot(QCustomPlot::rpQueuedReplot);
}


void FrequencyToolsMainWindow::drawMathData(int numOfBars, QVector<double> &clData, QVector<double> &crData)
{
    if(clData.isEmpty() || crData.isEmpty()){return;}
    double rangeMax = m_sFreCfgData.dSamplingRate / 2;

    m_vectorXAxisTicks.clear();
    m_pMathPlot->clearPlottables();

    double tick = 0;
    for(int  i = 0; i < m_nFttSize; i++){
        tick += rangeMax / m_nFttSize;
        m_vectorXAxisTicks.append(tick - (rangeMax / m_nFttSize /2));
    }

    double barWidth = rangeMax/numOfBars;
    if(numOfBars != clData.size() || numOfBars != crData.size()){
        return;
    }
    QCPBars *bars_math  = new QCPBars(m_pMathPlot->xAxis, m_pMathPlot->yAxis);
    bars_math->setAntialiased(false);
    bars_math->setName("BarsMath");
    bars_math->setPen(QPen(QColor("#1444ac").lighter(120)));
    bars_math->setBrush(QColor(20,68,120));
    bars_math->setWidth(barWidth);

    QVector<double> mathData;
    double delta;
    if(ScaleType_Linear == m_yMathAxisScaleType){
        for (int i=0; i< numOfBars; ++i){
            if(ui->MathTypeBox->currentIndex() == 0){
                delta = clData[i] - crData[i];
            }else if(ui->MathTypeBox->currentIndex() == 1){
                delta = crData[i] - clData[i];
            }else{
                delta = qSqrt(qPow(clData[i], 2) + qPow(crData[i], 2));
            }
            mathData << delta;
        }
    }else{
        double cl_dB, cr_dB;
        for (int i = 0; i < numOfBars; ++i){
            if(ui->MathTypeBox->currentIndex() == 3){
                double sqrtData = qSqrt(qPow(cl_dB, 2) + qPow(cr_dB, 2));
                delta = 20 * log10(sqrtData/m_dMathLogBitType);
            }

            if(clData[i] == 0){
                cl_dB = 0;
            }else{
                cl_dB = 20 * log10(clData[i] / m_dMathLogBitType);
            }
            if(crData[i] == 0){
                cr_dB = 0;
            }else{
                cr_dB = 20 * log10(crData[i] / m_dMathLogBitType);
            }

            if(ui->MathTypeBox->currentIndex() == 0){
                delta = cl_dB - cr_dB;
            }else if(ui->MathTypeBox->currentIndex() == 1){
                delta = cr_dB - cl_dB;
            }else if(ui->MathTypeBox->currentIndex() == 2){
                delta = cr_dB + cl_dB;
            }
            mathData << delta;
        }
    }

    bars_math->setData(m_vectorXAxisTicks, mathData);
    m_pMathPlot->replot(QCustomPlot::rpQueuedReplot);
}

void FrequencyToolsMainWindow::setTimeTabBarSatusInfo(TimeDomainData data)
{
    ui->FirstLabel->setText("音轨帧长:");
    ui->ShowFistDataInfoLabel->setText(QString("%1ms").arg(data.dFrameLength));

    ui->secondLabel->setText("采样率:");
    ui->ShowSecondDataLabel->setText(QString("%1KHz").arg(data.dSamplingRate / MATH_NUM));

    ui->ThreeLabel->setText("传输位宽:");
    ui->ShowThreeDataLabel->setText(QString("%1位").arg(data.nTransmissionBit));

    ui->FourLabel->setText("样点类型:");
    ui->ShowFourDataLabel->setText(QString("%1").arg(data.strSampleType));

    ui->fiveLabel->setText("音轨数量:");
    ui->ShowFiveDataLabel->setText(QString("%1").arg(data.nAudioTrackNum));
    ui->FiveWidget->show();
    ui->FourWidget->show();
    ui->ThreeWidget->show();
}

void FrequencyToolsMainWindow::setArrayTabBarSatusInfo(ArrayDomainData data)
{
    ui->FirstLabel->setText("成员数量:");
    ui->ShowFistDataInfoLabel->setText(QString("%1").arg(data.dArrayMembersNum));

    ui->secondLabel->setText("数据类型:");
    ui->ShowSecondDataLabel->setText(QString("%1").arg(dataFreTypeDesc.value(data.nDataType)));

    ui->ThreeLabel->setText("横轴类型:");
    ui->ShowThreeDataLabel->setText(QString("%1").arg(ArrayTypeMap.value(data.nXAsixType)));

    if(data.nXAsixType == TIME_TYPE){
        ui->FourWidget->show();
        ui->FourLabel->setText("样点点数:");
        ui->ShowFourDataLabel->setText(QString("%1点/秒").arg(data.nPointCountTime));
    }else{
        ui->FourWidget->hide();
    }
    ui->FiveWidget->hide();
    drawArrayAsxis();
}

void FrequencyToolsMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1) {
        QString fileName = qApp->applicationDirPath() + "/Spi_Moniter说明书.pdf";
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }

    QWidget::keyPressEvent(event);
}

void FrequencyToolsMainWindow::setFreTabBarSatusInfo(FrequencyData data)
{
    ui->FirstLabel->setText("频点数量:");
    ui->ShowFistDataInfoLabel->setText(QString("%1").arg(data.dSampleSize));

    ui->secondLabel->setText("采样率:");
    ui->ShowSecondDataLabel->setText(QString("%1KHz").arg(data.dSamplingRate / MATH_NUM));

    ui->ThreeLabel->setText("数据类型:");
    ui->ShowThreeDataLabel->setText(QString("%1").arg(dataFreTypeDesc.value(data.nDataType)));

    ui->FourLabel->setText("传输顺序:");
    ui->ShowFourDataLabel->setText(QString("%1").arg(TransmissionOrderMap.value(data.nTransmissionOrder)));
    ui->FiveWidget->hide();
    ui->FourWidget->show();
    ui->ThreeWidget->show();

    drawAsxis();
    m_bFirstMsgFrame = false;

    QString strCfgFilePath;
    strCfgFilePath = QCoreApplication::applicationDirPath();
    strCfgFilePath += QDir::separator();
    strCfgFilePath += "cfgFile.json";
    double dBNum = 4294967295;
    double dBNumMath = 4294967295;
    QFile cfgFile(strCfgFilePath);

    if(!cfgFile.exists()){
        switch (m_sFreCfgData.nDataType) {
        case 0:
            break;
        case 1:
            dBNum = 1;
            break;
        case 2:
            dBNum = 4294967295;
            break;
        case 3:
            dBNum = 2147483647;
            break;
        case 4:
            dBNum = 65535;
            break;
        case 5:
            dBNum = 32767;
            break;
        case 6:
            dBNum = 255;
            break;
        case 7:
            dBNum = 127;
            break;
        default:
            break;
        }
        dBNumMath = dBNum;
    }else if (cfgFile.open(QIODevice::ReadOnly)) {
        QByteArray data = cfgFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject json = doc.object();
        QJsonObject dBNumRange = json["dBNumRange"].toObject();
        QJsonObject dBNumRangeMath = json["dBNumRangeMath"].toObject();
        switch (m_sFreCfgData.nDataType) {
        case 0:
            break;
        case 1:
            dBNum = dBNumRange["Float"].toDouble();
            dBNumMath = dBNumRangeMath["Float"].toDouble();
            break;
        case 2:
            dBNum = dBNumRange["Uint32"].toDouble();
            dBNumMath = dBNumRangeMath["Uint32"].toDouble();
            break;
        case 3:
            dBNum = dBNumRange["int32"].toDouble();
            dBNumMath = dBNumRangeMath["int32"].toDouble();
            break;
        case 4:
            dBNum = dBNumRange["Uint16"].toDouble();
            dBNumMath = dBNumRangeMath["Uint16"].toDouble();
            break;
        case 5:
            dBNum = dBNumRange["int16"].toDouble();
            dBNumMath = dBNumRangeMath["int16"].toDouble();
            break;
        case 6:
            dBNum = dBNumRange["Uint8"].toDouble();
            dBNumMath = dBNumRangeMath["Uint8"].toDouble();
            break;
        case 7:
            dBNum = dBNumRange["int8"].toDouble();
            dBNumMath = dBNumRangeMath["int8"].toDouble();
            break;
        default:
            break;

        }
    }
    cfgFile.close();
    if(dBNum == 0){
        dBNum = LogNumMap.value(dataFreTypeDesc.value(m_sFreCfgData.nDataType));
    }
    if(dBNumMath == 0){
        dBNumMath = LogNumMap.value(dataFreTypeDesc.value(m_sFreCfgData.nDataType));
    }

    ui->dBNumcfgEdit->setText(QString::number(dBNum, 'f', 0));
    ui->dBNumCfgMathEdit->setText(QString::number(dBNumMath, 'f', 0));

    m_dLogBitType = dBNum;
    m_dMathLogBitType = dBNumMath;
}

void FrequencyToolsMainWindow::closeEvent(QCloseEvent *event)
{
    saveInputData();
    QMainWindow::closeEvent(event);
}

void FrequencyToolsMainWindow::on_pauseBtn_clicked(bool checked)
{
    m_bIsPause = false;
    if (false == checked){
        ui->pauseBtn->setText("暂停");
    }else{
        ui->pauseBtn->setText("继续");
        m_bIsPause = true;
    }
}

void FrequencyToolsMainWindow::slotChannelSelectBtnChicked()
{
    m_pG1G2Plot->clearPlottables();
    drawData(m_nFttSize, m_vectorCLData, m_vectorCRData);
}

void FrequencyToolsMainWindow::slotDeviceStatus(int status)
{
    ui->ShowLogicalStatus->setText(QString("%1").arg(DeviceStatusDesc.value(status)));
}

void FrequencyToolsMainWindow::slotArrayData(QVector<float>floatData)
{
    QVector<double> doubleYData(floatData.begin(), floatData.end());
    if(m_sArrayCfgData.nXAsixType == POINT_TYPE){
        for(int i = 0;i < doubleYData.size(); i++){
            m_pArrayPlot->graph(i)->addData(m_dArrayXAsixCount, doubleYData[i]);
        }
    }else{
        for(int i = 0;i < doubleYData.size();i++){
            m_pArrayPlot->graph(i)->addData(m_dArrayXAsixCount / m_sArrayCfgData.nPointCountTime, doubleYData[i]);
        }
    }
    m_dArrayXAsixCount++;
}

void FrequencyToolsMainWindow::slotDrawG1G2Data(QVector<double> &clData, QVector<double> &crData)
{
    if(m_bIsPause) {return;}
    m_vectorCLData.clear();
    m_vectorCRData.clear();
    m_nFttSize = m_sFreCfgData.dSampleSize;
    AvrResult result = m_var->AddCalculatedData(clData,crData);
    if(result.valid){
        m_vectorCLData = result.channel_L;
        m_vectorCRData = result.channel_R;
        drawData(m_nFttSize,result.channel_L,result.channel_R);
        drawMathData(m_nFttSize,result.channel_L,result.channel_R);
        if(m_mIsFirstRescale == true){
            on_AutoScalingBtn_clicked();
            m_mIsFirstRescale = false;
        }
    }
}

void FrequencyToolsMainWindow::slotFrequencyData(FrequencyData &cfgData)
{
    m_sFreCfgData = cfgData;
}

void FrequencyToolsMainWindow::slotErrorDataCount(int nErrorCount, bool isRight)
{
    m_nErrCount += nErrorCount;
    if(isRight == true){
        ui->errorLightBtnWidget->setStop();
    }else{
        ui->errorLightBtnWidget->setStart();
    }

    ui->errorCountTotalLabel->setText(QString("数据错误:<font color = #FF0000> %1 </font>次").arg(m_nErrCount));
}

void FrequencyToolsMainWindow::on_averageVlaueResBtn_clicked()
{
    QString strAverageVlaue = ui->averageVlaueEdit->text().trimmed();
    int nAverageVlaue = strAverageVlaue.toInt();
    m_var.reset(new AvrCalculator(nAverageVlaue));
}

void FrequencyToolsMainWindow::on_exportDataCheckbox_clicked(bool checked)
{
    emit signalIsCheckBoxStatus(checked);
}

void FrequencyToolsMainWindow::on_defaultScalingBtn_clicked()
{
    m_bDefault = true;
    drawAsxis();
    drawData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    drawMathData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    m_bDefault = false;
}

void FrequencyToolsMainWindow::on_AutoScalingBtn_clicked()
{
    drawData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    drawMathData(m_nFttSize, m_vectorCLData, m_vectorCRData);
    m_pG1G2Plot->yAxis->rescale(true);
    m_pMathPlot->yAxis->rescale(true);
}

void FrequencyToolsMainWindow::on_MathTypeBox_currentIndexChanged(int index)
{
    (void)index;
    drawMathData(m_nFttSize, m_vectorCLData, m_vectorCRData);
}

void FrequencyToolsMainWindow::slotOpenSpiRes(int ret)
{
    if(ret < 0){
        ui->ShowLogicalStatus->setText(QString("%1").arg(DeviceStatusDesc.value(Device_Offline)));
        emit signalnOpenSpi();
    }else if(ret == 0){
        ui->ShowLogicalStatus->setText(QString("%1").arg(DeviceStatusDesc.value(Device_Wait_Stare)));
    }
}

void FrequencyToolsMainWindow::onShowMessage(QString title, QString message)
{
    (void)title;
    QMessageBox box(QMessageBox::Information, "提示",message,QMessageBox::Ok,this);
    box.setButtonText(QMessageBox::Ok, QString("确认"));
    box.exec();
    return;
}


void FrequencyToolsMainWindow::on_exportFilePostionBtn_clicked()
{
    QString strCSVPath;
    strCSVPath = QCoreApplication::applicationDirPath();
    strCSVPath += QDir::separator();
    strCSVPath += "csv";
    strCSVPath.replace("/", "\\");
    QProcess process;
    process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strCSVPath));
}


void FrequencyToolsMainWindow::on_dBNumBtn_clicked()
{
    m_dLogBitType = ui->dBNumcfgEdit->text().toDouble();
}


void FrequencyToolsMainWindow::on_saveWAVFilePostionBtn_clicked()
{
    QString strWAVPath;
    strWAVPath = QCoreApplication::applicationDirPath();
    strWAVPath += QDir::separator();
    strWAVPath += "wav";
    strWAVPath.replace("/", "\\");
    QProcess process;
    process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strWAVPath));
}

void FrequencyToolsMainWindow::on_SaveWAVFileCheckBox_clicked(bool checked)
{
    ui->SaveWAVFileCheckBox->setChecked(checked);
    emit signalIsCheckBoxWavStatus(checked);
}

void FrequencyToolsMainWindow::on_TimeDomainSettingBtn_clicked()
{
    m_pTimeDomainDialg->exec();
    m_sTimeCfgData = m_pTimeDomainDialg->getTimeCfgData();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void FrequencyToolsMainWindow::on_FreDomainSettingBtn_clicked()
{
    m_pFreDomainDialog->exec();
    m_sFreCfgData = m_pFreDomainDialog->getFreCfgData();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void FrequencyToolsMainWindow::on_ArrayTypeSettingBtn_clicked()
{
    m_pArrayDomainDialg->exec();
    m_sArrayCfgData = m_pArrayDomainDialg->getArrayCfgData();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void FrequencyToolsMainWindow::on_dBNumcfgEdit_returnPressed()
{
    m_dLogBitType = ui->dBNumcfgEdit->text().toDouble();
}

void FrequencyToolsMainWindow::on_dBNumCfgMathEdit_returnPressed()
{
    m_dMathLogBitType = ui->dBNumCfgMathEdit->text().toDouble();
}

void FrequencyToolsMainWindow::on_MathTypeBox_activated(int index)
{
    (void)index;
    on_AutoScalingBtn_clicked();
}

void FrequencyToolsMainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == 0){
        emit signalTimeDomainDataCfg(m_sTimeCfgData);
        setTimeTabBarSatusInfo(m_sTimeCfgData);
    }else if(index == 1){
        emit signalFreDomainDataCfg(m_sFreCfgData);
        setFreTabBarSatusInfo(m_sFreCfgData);
    }else if(index == 2){
        emit signalArrayDomainDataCfg(m_sArrayCfgData);
        setArrayTabBarSatusInfo(m_sArrayCfgData);
    }
}

void FrequencyToolsMainWindow::on_saveBinFilePostionBtn_clicked()
{
    QString strBinPath;
    strBinPath = QCoreApplication::applicationDirPath();
    strBinPath += QDir::separator();
    strBinPath += "BinFile";
    strBinPath.replace("/", "\\");
    QProcess process;
    process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strBinPath));
}

void FrequencyToolsMainWindow::on_SaveBinFileCheckBox_clicked(bool checked)
{
    ui->SaveBinFileCheckBox->setChecked(checked);
    emit signalIsCheckBoxBinStatus(checked);
}

void FrequencyToolsMainWindow::on_arrayAutoScalingBtn_clicked()
{
    m_pArrayPlot->yAxis->rescale(true);
}

void FrequencyToolsMainWindow::on_arrayDefaultScalingBtn_clicked()
{
    double key = m_dArrayXAsixCount;
    int nStartPonit = key - 25000;
    if(nStartPonit < 0){
        nStartPonit = 0;
    }
    m_pArrayPlot->xAxis->setRange(nStartPonit, key);
}

void FrequencyToolsMainWindow::on_arrayPauseBtn_clicked(bool checked)
{
    m_arrayIsPause = false;
    if (false == checked){
        ui->arrayPauseBtn->setText("暂停");
    }else{
        ui->arrayPauseBtn->setText("继续");
        m_arrayIsPause = true;
    }
}


//*****************************************************************************************
// Project:     UniversalConfigurationManager
//
// Author:      Sven Meier, NetTimeLogic GmbH
//
// License:     Copyright (c) 2017, NetTimeLogic GmbH, Switzerland, Sven Meier <contact@nettimelogic.com>
//              All rights reserved.
//
//              THIS PROGRAM IS FREE SOFTWARE: YOU CAN REDISTRIBUTE IT AND/OR MODIFY
//              IT UNDER THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE AS
//              PUBLISHED BY THE FREE SOFTWARE FOUNDATION, VERSION 3.
//
//              THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT
//              WITHOUT ANY WARRANTY; WITHOUT EVEN THE IMPLIED WARRANTY OF
//              MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. SEE THE GNU
//              LESSER GENERAL LESSER PUBLIC LICENSE FOR MORE DETAILS.
//
//              YOU SHOULD HAVE RECEIVED A COPY OF THE GNU LESSER GENERAL PUBLIC LICENSE
//              ALONG WITH THIS PROGRAM. IF NOT, SEE <http://www.gnu.org/licenses/>.
//
//*****************************************************************************************

#include <Ucm_PtpHcTab.h>
#include <PtpOcTab/Ucm_PtpOcTab.h>
#include <PtpTcTab/Ucm_PtpTcTab.h>
#include <ui_Ucm_PtpHcTab.h>
#include <QHostAddress>

Ucm_PtpHcTab::Ucm_PtpHcTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_PtpHcTab();
    ui->setupUi(this);

    ptp_hc_timer = new QTimer(this);
    ptp_hc_timer->stop();
    connect(ui->PtpHcReadValuesButton, SIGNAL(clicked()), this, SLOT(ptp_hc_read_values_button_clicked()));
    connect(ui->PtpHcWriteValuesButton, SIGNAL(clicked()), this, SLOT(ptp_hc_write_values_button_clicked()));
    connect(ui->PtpHcAutoRefreshButton, SIGNAL(clicked()), this, SLOT(ptp_hc_auto_refresh_button_clicked()));
    connect(ptp_hc_timer, SIGNAL(timeout()), this, SLOT(ptp_hc_read_values_button_clicked()));

    ptp_hc_offset_series = new QLineSeries();
    ptp_hc_offset_number_of_points = 0;

    ptp_hc_offset_chart = new QChart();
    ptp_hc_offset_chart->setContentsMargins(0, 0, 0, 0);
    ptp_hc_offset_chart->setBackgroundRoundness(0);
    ptp_hc_offset_chart->setBackgroundBrush(Qt::white);
    ptp_hc_offset_chart->legend()->hide();
    ptp_hc_offset_chart->addSeries(ptp_hc_offset_series);

    QValueAxis* ptp_hc_offset_chart_x_axis = new QValueAxis;
    ptp_hc_offset_chart_x_axis->setLabelFormat("%i");
    ptp_hc_offset_chart_x_axis->setTickCount(10);
    ptp_hc_offset_chart_x_axis->setMin(0);
    ptp_hc_offset_chart_x_axis->setMax(19);
    ptp_hc_offset_chart->addAxis(ptp_hc_offset_chart_x_axis, Qt::AlignBottom);
    ptp_hc_offset_series->attachAxis(ptp_hc_offset_chart_x_axis);

    QValueAxis* ptp_hc_offset_chart_y_axis = new QValueAxis;
    ptp_hc_offset_chart_y_axis->setLabelFormat("%i");
    ptp_hc_offset_chart_y_axis->setTickCount(5);
    ptp_hc_offset_chart_y_axis->setMin(-500);
    ptp_hc_offset_chart_y_axis->setMax(500);
    ptp_hc_offset_chart->addAxis(ptp_hc_offset_chart_y_axis, Qt::AlignLeft);
    ptp_hc_offset_series->attachAxis(ptp_hc_offset_chart_y_axis);

    ptp_hc_offset_chart_view = new QChartView(ptp_hc_offset_chart);
    ptp_hc_offset_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->PtpHcOffsetChartLayout->addWidget(ptp_hc_offset_chart_view, 0, 0);

    // max 3 ports
    for (int i=0; i<3; i++)
    {
        QLineSeries* temp_line_series = new QLineSeries();
        ptp_hc_delay_series.append(temp_line_series);
    }
    ptp_hc_delay_number_of_points = 0;

    ptp_hc_delay_chart = new QChart();
    ptp_hc_delay_chart->setContentsMargins(0, 0, 0, 0);
    ptp_hc_delay_chart->setBackgroundRoundness(0);
    ptp_hc_delay_chart->setBackgroundBrush(Qt::white);
    ptp_hc_delay_chart->legend()->hide();
    for (int i=0; i<3; i++)
    {
        ptp_hc_delay_chart->addSeries(ptp_hc_delay_series.at(i));
    }

    QValueAxis* ptp_hc_delay_chart_x_axis = new QValueAxis;
    ptp_hc_delay_chart_x_axis->setLabelFormat("%i");
    ptp_hc_delay_chart_x_axis->setTickCount(10);
    ptp_hc_delay_chart_x_axis->setMin(0);
    ptp_hc_delay_chart_x_axis->setMax(19);
    ptp_hc_delay_chart->addAxis(ptp_hc_delay_chart_x_axis, Qt::AlignBottom);
    for (int i=0; i<3; i++)
    {
        ptp_hc_delay_series.at(i)->attachAxis(ptp_hc_delay_chart_x_axis);
    }

    QValueAxis* ptp_hc_delay_chart_y_axis = new QValueAxis;
    ptp_hc_delay_chart_y_axis->setLabelFormat("%i");
    ptp_hc_delay_chart_y_axis->setTickCount(5);
    ptp_hc_delay_chart_y_axis->setMin(0);
    ptp_hc_delay_chart_y_axis->setMax(500);
    ptp_hc_delay_chart->addAxis(ptp_hc_delay_chart_y_axis, Qt::AlignLeft);
    for (int i=0; i<3; i++)
    {
        ptp_hc_delay_series.at(i)->attachAxis(ptp_hc_delay_chart_y_axis);
    }
    ptp_hc_delay_chart_view = new QChartView(ptp_hc_delay_chart);
    ptp_hc_delay_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->PtpHcDelayChartLayout->addWidget(ptp_hc_delay_chart_view, 0, 0);

}

Ucm_PtpHcTab::~Ucm_PtpHcTab()
{
    ptp_hc_timer->stop();
    delete ui;
    delete ptp_hc_timer;
    delete ptp_hc_offset_series;
    delete ptp_hc_offset_chart_view;
    delete ptp_hc_offset_chart;
    for (int i=0; i<3; i++)
    {
        delete ptp_hc_delay_series.at(i);
    }
    delete ptp_hc_delay_chart_view;
    delete ptp_hc_delay_chart;
}

int Ucm_PtpHcTab::ptp_hc_resize(int height, int width)
{
    int height_delta = 0;
    int width_delta = 0;

    if (height <= Ucm_MainHeight)
    {
        height_delta = (height-Ucm_MainHeight);
    }
    if (width <= Ucm_MainWidth)
    {
        width_delta = (width-Ucm_MainWidth);
    }

    ui->PtpHcDelayChartValue->setFixedHeight(210);
    ui->PtpHcDelayChartValue->setFixedWidth(450+(width_delta/2));
    ui->PtpHcDelayChartLabel->setFixedWidth(450+(width_delta/2));

    ui->PtpHcOffsetChartValue->move(910+(width_delta/2), 20);
    ui->PtpHcOffsetChartLabel->move(910+(width_delta/2), 30);

    ui->PtpHcOffsetChartValue->setFixedHeight(210);
    ui->PtpHcOffsetChartValue->setFixedWidth(450+(width_delta/2));
    ui->PtpHcOffsetChartLabel->setFixedWidth(450+(width_delta/2));


    updateGeometry();

    return 0;
}

void Ucm_PtpHcTab::ptp_hc_add_instance(unsigned int instance)
{
    ui->PtpHcInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_PtpHcTab::ptp_hc_enable(void)
{
    return 0;
}

int Ucm_PtpHcTab::ptp_hc_disable(void)
{
    ptp_hc_timer->stop();
    ui->PtpHcAutoRefreshButton->setText("Start Refresh");
    ui->PtpHcInstanceComboBox->setEnabled(true);
    ui->PtpHcReadValuesButton->setEnabled(true);
    ui->PtpHcWriteValuesButton->setEnabled(true);
    ui->PtpHcInstanceComboBox->clear();

    ui->PtpHcVlanValue->setText("NA");
    ui->PtpHcProfileValue->setCurrentText("NA");
    ui->PtpHcLayerValue->setCurrentText("NA");
    ui->PtpHcDelayMechanismValue->setCurrentText("NA");
    ui->PtpHcIpValue->setText("NA");
    ui->PtpHcVersionValue->setText("NA");

    ui->PtpHcDefaultDsClockIdValue->setText("NA");
    ui->PtpHcDefaultDsDomainValue->setText("NA");
    ui->PtpHcDefaultDsPriority1Value->setText("NA");
    ui->PtpHcDefaultDsPriority2Value->setText("NA");
    ui->PtpHcDefaultDsVarianceValue->setText("NA");
    ui->PtpHcDefaultDsAccuracyValue->setText("NA");
    ui->PtpHcDefaultDsClassValue->setText("NA");
    ui->PtpHcDefaultDsShortIdValue->setText("NA");
    ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
    ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
    ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);

    ui->PtpHcPortDsPeerDelayValue->setText("NA");
    ui->PtpHcPortDsPortNrComboBox->clear();
    ui->PtpHcPortDsStateValue->setText("NA");
    ui->PtpHcPortDsAsymmetryValue->setText("NA");
    ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
    ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
    ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
    ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
    ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
    ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
    ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");

    ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
    ui->PtpHcCurrentDsOffsetValue->setText("NA");

    ui->PtpHcParentDsParentClockIdValue->setText("NA");
    ui->PtpHcParentDsGmClockIdValue->setText("NA");
    ui->PtpHcParentDsGmPriority1Value->setText("NA");
    ui->PtpHcParentDsGmPriority2Value->setText("NA");
    ui->PtpHcParentDsGmAccuracyValue->setText("NA");
    ui->PtpHcParentDsGmClassValue->setText("NA");
    ui->PtpHcParentDsGmShortIdValue->setText("NA");
    ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
    ui->PtpHcParentDsNwInaccuracyValue->setText("NA");

    ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
    ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
    ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
    ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
    ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
    ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
    ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
    ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
    ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
    ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
    ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
    ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");

    ui->PtpHcVlanEnableCheckBox->setChecked(false);
    ui->PtpHcVersionValue->setText("NA");

    ptp_hc_offset_number_of_points = 0;
    ptp_hc_offset_series-> clear();

    ptp_hc_delay_number_of_points = 0;
    for (int i=0; i<3; i++)
    {
        ptp_hc_delay_series.at(i)->clear();
    }

    return 0;
}

void Ucm_PtpHcTab::ptp_hc_read_values(void)
{
    unsigned long long temp_next_jump;
    unsigned long long temp_delay;
    unsigned long long temp_offset;
    long long temp_signed_offset;
    unsigned int temp_length;
    unsigned long temp_ip;
    int temp_min = 0;
    int temp_max = 0;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;
    unsigned int nr_of_ports = 0;

    temp_string = ui->PtpHcInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PtpHybridClockCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PtpHcVlanValue->setText("NA");
            ui->PtpHcProfileValue->setCurrentText("NA");
            ui->PtpHcLayerValue->setCurrentText("NA");
            ui->PtpHcDelayMechanismValue->setCurrentText("NA");
            ui->PtpHcIpValue->setText("NA");
            ui->PtpHcVersionValue->setText("NA");

            ui->PtpHcDefaultDsClockIdValue->setText("NA");
            ui->PtpHcDefaultDsDomainValue->setText("NA");
            ui->PtpHcDefaultDsPriority1Value->setText("NA");
            ui->PtpHcDefaultDsPriority2Value->setText("NA");
            ui->PtpHcDefaultDsVarianceValue->setText("NA");
            ui->PtpHcDefaultDsAccuracyValue->setText("NA");
            ui->PtpHcDefaultDsClassValue->setText("NA");
            ui->PtpHcDefaultDsShortIdValue->setText("NA");
            ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
            ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
            ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
            ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);

            ui->PtpHcPortDsPeerDelayValue->setText("NA");
            ui->PtpHcPortDsPortNrComboBox->clear();
            ui->PtpHcPortDsStateValue->setText("NA");
            ui->PtpHcPortDsAsymmetryValue->setText("NA");
            ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
            ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
            ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");

            ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
            ui->PtpHcCurrentDsOffsetValue->setText("NA");

            ui->PtpHcParentDsParentClockIdValue->setText("NA");
            ui->PtpHcParentDsGmClockIdValue->setText("NA");
            ui->PtpHcParentDsGmPriority1Value->setText("NA");
            ui->PtpHcParentDsGmPriority2Value->setText("NA");
            ui->PtpHcParentDsGmAccuracyValue->setText("NA");
            ui->PtpHcParentDsGmClassValue->setText("NA");
            ui->PtpHcParentDsGmShortIdValue->setText("NA");
            ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
            ui->PtpHcParentDsNwInaccuracyValue->setText("NA");

            ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
            ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
            ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
            ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
            ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");

            ui->PtpHcVlanEnableCheckBox->setChecked(false);
            ui->PtpHcVersionValue->setText("NA");
            return;
        }
    }

    // PTP OC Part
    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PtpHcEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PtpHcEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->PtpHcEnableCheckBox->setChecked(false);
    }


    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ConfigVlanReg, temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ui->PtpHcVlanEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PtpHcVlanEnableCheckBox->setChecked(true);
        }

        temp_data &= 0x0000FFFF;

        ui->PtpHcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->PtpHcVlanEnableCheckBox->setChecked(false);
        ui->PtpHcVlanValue->setText("NA");
    }


    // profile and layer and mode
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ConfigProfileReg, temp_data))
    {
        switch (temp_data & 0x00000003)
        {
        case 0:
            ui->PtpHcProfileValue->setCurrentText("Default");
            break;
        case 1:
            ui->PtpHcProfileValue->setCurrentText("Power");
            break;
        case 2:
            ui->PtpHcProfileValue->setCurrentText("Utility");
            break;
        case 3:
            ui->PtpHcProfileValue->setCurrentText("TSN");
            break;
        default:
            ui->PtpHcProfileValue->setCurrentText("NA");
            break;
        }

        switch ((temp_data >> 8) & 0x00000001)
        {
        case 0:
            ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
            break;
        case 1:
            ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(true);
            break;
        default:
            ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
            break;
        }

        switch ((temp_data >> 9) & 0x00000001)
        {
        case 0:
            ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
            break;
        case 1:
            ui->PtpHcDefaultDsSignalingCheckBox->setChecked(true);
            break;
        default:
            ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
            break;
        }

        switch ((temp_data >> 16) & 0x00000001)
        {
        case 0:
            ui->PtpHcLayerValue->setCurrentText("Layer 2");
            break;
        case 1:
            ui->PtpHcLayerValue->setCurrentText("Layer 3");
            break;
        default:
            ui->PtpHcLayerValue->setCurrentText("NA");
            break;
        }

        switch ((temp_data >> 24) & 0x00000001)
        {
        case 0:
            ui->PtpHcDelayMechanismValue->setCurrentText("P2P");
            break;
        case 1:
            ui->PtpHcDelayMechanismValue->setCurrentText("E2E");
            break;
        default:
            ui->PtpHcDelayMechanismValue->setCurrentText("NA");
            break;
        }
    }
    else
    {
        ui->PtpHcProfileValue->setCurrentText("NA");
        ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
        ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
        ui->PtpHcLayerValue->setCurrentText("NA");
        ui->PtpHcDelayMechanismValue->setCurrentText("NA");
    }

    // ip
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, temp_data))
    {
        temp_ip = 0x00000000;
        temp_ip |= (temp_data >> 0) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 8) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 16) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 24) & 0x000000FF;

        temp_string = QHostAddress(temp_ip).toString();

        ui->PtpHcIpValue->setText(temp_string);

    }
    else
    {
        ui->PtpHcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // clock id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDs1Reg, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDs2Reg, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            ui->PtpHcDefaultDsClockIdValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpHcDefaultDsClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcDefaultDsClockIdValue->setText("NA");
                    }

                    // domain, priority 1 & 2
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, temp_data))
                    {
                        ui->PtpHcDefaultDsDomainValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpHcDefaultDsPriority2Value->setText(QString("0x%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpHcDefaultDsPriority1Value->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

                    }
                    else
                    {
                        ui->PtpHcDefaultDsDomainValue->setText("NA");
                        ui->PtpHcDefaultDsPriority1Value->setText("NA");
                        ui->PtpHcDefaultDsPriority2Value->setText("NA");
                    }

                    // variance, accuracy ,class
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDs4Reg, temp_data))
                    {
                        ui->PtpHcDefaultDsVarianceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                        ui->PtpHcDefaultDsAccuracyValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
                        ui->PtpHcDefaultDsClassValue->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpHcDefaultDsVarianceValue->setText("NA");
                        ui->PtpHcDefaultDsAccuracyValue->setText("NA");
                        ui->PtpHcDefaultDsClassValue->setText("NA");
                    }

                    // short id
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDs5Reg, temp_data))
                    {
                        ui->PtpHcDefaultDsShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpHcDefaultDsShortIdValue->setText("NA");
                    }

                    // inaccuracy
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_DefaultDs6Reg, temp_data))
                    {
                        ui->PtpHcDefaultDsInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpHcDefaultDsClockIdValue->setText("NA");
                    ui->PtpHcDefaultDsDomainValue->setText("NA");
                    ui->PtpHcDefaultDsPriority1Value->setText("NA");
                    ui->PtpHcDefaultDsPriority2Value->setText("NA");
                    ui->PtpHcDefaultDsVarianceValue->setText("NA");
                    ui->PtpHcDefaultDsAccuracyValue->setText("NA");
                    ui->PtpHcDefaultDsClassValue->setText("NA");
                    ui->PtpHcDefaultDsShortIdValue->setText("NA");
                    ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
                }

            }
            else
            {
                ui->PtpHcDefaultDsClockIdValue->setText("NA");
                ui->PtpHcDefaultDsDomainValue->setText("NA");
                ui->PtpHcDefaultDsPriority1Value->setText("NA");
                ui->PtpHcDefaultDsPriority2Value->setText("NA");
                ui->PtpHcDefaultDsVarianceValue->setText("NA");
                ui->PtpHcDefaultDsAccuracyValue->setText("NA");
                ui->PtpHcDefaultDsClassValue->setText("NA");
                ui->PtpHcDefaultDsShortIdValue->setText("NA");
                ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpHcDefaultDsClockIdValue->setText("NA");
        ui->PtpHcDefaultDsDomainValue->setText("NA");
        ui->PtpHcDefaultDsPriority1Value->setText("NA");
        ui->PtpHcDefaultDsPriority2Value->setText("NA");
        ui->PtpHcDefaultDsVarianceValue->setText("NA");
        ui->PtpHcDefaultDsAccuracyValue->setText("NA");
        ui->PtpHcDefaultDsClassValue->setText("NA");
        ui->PtpHcDefaultDsShortIdValue->setText("NA");
        ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
    }

    //********************************
    // port dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // state
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_PortDs3Reg, temp_data))
                    {
                        switch (temp_data)
                        {
                        case 0x0000001:
                            ui->PtpHcPortDsStateValue->setText("INITIALIZING");
                            break;
                        case 0x00000002:
                            ui->PtpHcPortDsStateValue->setText("FAULTY");
                            break;
                        case 0x00000003:
                            ui->PtpHcPortDsStateValue->setText("DISABLED");
                            break;
                        case 0x00000004:
                            ui->PtpHcPortDsStateValue->setText("LISTENING");
                            break;
                        case 0x00000005:
                            ui->PtpHcPortDsStateValue->setText("PREMASTER");
                            break;
                        case 0x00000006:
                            ui->PtpHcPortDsStateValue->setText("MASTER");
                            break;
                        case 0x00000007:
                            ui->PtpHcPortDsStateValue->setText("PASSIVE");
                            break;
                        case 0x00000008:
                            ui->PtpHcPortDsStateValue->setText("UNCALIBRATED");
                            break;
                        case 0x00000009:
                            ui->PtpHcPortDsStateValue->setText("SLAVE");
                            break;
                        default:
                            ui->PtpHcPortDsStateValue->setText("NA");
                            break;
                        }                    }
                    else
                    {
                        ui->PtpHcPortDsStateValue->setText("NA");
                    }

                    // delay req log msg interval
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_PortDs4Reg, temp_data))
                    {
                        ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText(QString::number((signed char)((temp_data >> 8) & 0x000000FF)));

                    }
                    else
                    {
                        ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                    }

                    // announce log msg interval and announce receipt timeout
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_PortDs5Reg, temp_data))
                    {
                        ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText(QString::number(((temp_data >> 8) & 0x000000FF)));

                    }
                    else
                    {
                        ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                        ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                    }

                    // sync log msg interval and sync receipt timeout
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_PortDs6Reg, temp_data))
                    {
                        ui->PtpHcPortDsSyncLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));
                        ui->PtpHcPortDsSyncReceiptTimeoutValue->setText(QString::number(((temp_data >> 8) & 0x000000FF)));

                    }
                    else
                    {
                        ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                        ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpHcPortDsStateValue->setText("NA");
                    ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                    ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
                }

            }
            else
            {
                ui->PtpHcPortDsStateValue->setText("NA");
                ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpHcPortDsStateValue->setText("NA");
        ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
        ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
        ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
        ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
        ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
    }


    //********************************
    // current dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_CurrentDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_CurrentDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // steps removed
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_CurrentDs1Reg, temp_data))
                    {
                        ui->PtpHcCurrentDsStepsRemovedValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
                    }

                    // offset
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_CurrentDs2Reg, temp_data))
                    {
                        temp_offset = temp_data;
                        temp_offset = temp_offset << 32;
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_CurrentDs3Reg, temp_data))
                        {
                            temp_offset |= temp_data;

                            if ((temp_offset & 0x8000000000000000) != 0)
                            {
                                temp_offset = (0xFFFF000000000000 | (temp_offset >> 16));
                                temp_signed_offset = (long long)temp_offset;
                            }
                            else
                            {
                                temp_offset = (0x0000FFFFFFFFFFFF & (temp_offset >> 16));
                                temp_signed_offset = (long long)temp_offset;
                            }

                            if (temp_signed_offset == -4294967296) // negativ 0
                            {
                                temp_signed_offset = 0;
                            }

                            // limit to 100000 in display
                            if (temp_signed_offset >= 100000)
                            {
                                temp_signed_offset = 100000;
                            }
                            else if (temp_signed_offset <= -100000)
                            {
                                temp_signed_offset = -100000;
                            }

                            ui->PtpHcCurrentDsOffsetValue->setText(QString::number(temp_signed_offset));

                            if (true == ptp_hc_timer->isActive())
                            {
                                ptp_hc_offset_series->append(ptp_hc_offset_number_of_points, temp_signed_offset);

                                if (ptp_hc_offset_number_of_points < 20)
                                {
                                    ptp_hc_offset_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_hc_offset_series->count(); j++)
                                    {
                                        QPointF temp_point = ptp_hc_offset_series->at(j);
                                        ptp_hc_offset_series->replace(j, (j-1), temp_point.y());
                                    }
                                    ptp_hc_offset_series->remove(0);
                                }

                                temp_min = 0;
                                temp_max = 0;
                                for (int j = 0; j < ptp_hc_offset_series->count(); j++)
                                {
                                    QPointF temp_point = ptp_hc_offset_series->at(j);
                                    if (j == 0)
                                    {
                                        temp_min = temp_point.y();
                                        temp_max = temp_point.y();
                                    }
                                    if (temp_min > temp_point.y())
                                    {
                                        temp_min = temp_point.y();
                                    }
                                    if (temp_max < temp_point.y())
                                    {
                                        temp_max = temp_point.y();
                                    }
                                }
                                temp_max = ((temp_max/100)+1)*100;
                                temp_min = ((temp_min/100)-1)*100;
                                if (temp_max > 100000)
                                {
                                    temp_max = 100000;
                                }
                                if (temp_min < -100000)
                                {
                                    temp_min = -100000;
                                }
                                ptp_hc_offset_chart->axisY()->setMin(temp_min);
                                ptp_hc_offset_chart->axisY()->setMax(temp_max);

                                ptp_hc_offset_chart->show();
                            }
                        }
                        else
                        {
                            ui->PtpHcCurrentDsOffsetValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcCurrentDsOffsetValue->setText("NA");
                    }

                    temp_string = ui->PtpHcDelayMechanismValue->currentText();
                    if (temp_string == "P2P")
                    {
                        // peer delay
                        ui->PtpHcCurrentDsDelayValue->setText("NA");
                    }
                    else if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_CurrentDs4Reg, temp_data))
                    {
                        // end to end delay
                        temp_delay = temp_data;
                        temp_delay = temp_delay << 32;
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_CurrentDs5Reg, temp_data))
                        {
                            temp_delay |= temp_data;
                            temp_delay = temp_delay >> 16;
                            ui->PtpHcCurrentDsDelayValue->setText(QString::number(temp_delay));

                            if (true == ptp_hc_timer->isActive())
                            {
                                ptp_hc_delay_series.at(0)->append(ptp_hc_delay_number_of_points, temp_delay);

                                if (ptp_hc_delay_number_of_points < 20)
                                {
                                    ptp_hc_delay_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_hc_delay_series.at(0)->count(); j++)
                                    {
                                        QPointF temp_point = ptp_hc_delay_series.at(0)->at(j);
                                        ptp_hc_delay_series.at(0)->replace(j, (j-1), temp_point.y());
                                    }
                                    ptp_hc_delay_series.at(0)->remove(0);
                                }

                                temp_min = 0;
                                temp_max = 0;
                                for (int j = 0; j < ptp_hc_delay_series.at(0)->count(); j++)
                                {
                                    QPointF temp_point = ptp_hc_delay_series.at(0)->at(j);
                                    if (j == 0)
                                    {
                                        temp_min = temp_point.y();
                                        temp_max = temp_point.y();
                                    }
                                    if (temp_min > temp_point.y())
                                    {
                                        temp_min = temp_point.y();
                                    }
                                    if (temp_max < temp_point.y())
                                    {
                                        temp_max = temp_point.y();
                                    }
                                }
                                temp_max = ((temp_max/100)+1)*100;
                                temp_min = ((temp_min/100)-1)*100;
                                if (temp_min < 0)
                                {
                                    temp_min = 0;
                                }
                                ptp_hc_delay_chart->axisY()->setMin(temp_min);
                                ptp_hc_delay_chart->axisY()->setMax(temp_max);

                                ptp_hc_delay_chart->show();
                            }
                        }
                        else
                        {
                            ui->PtpHcCurrentDsDelayValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcCurrentDsDelayValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
                    ui->PtpHcCurrentDsOffsetValue->setText("NA");
                }

            }
            else
            {
                ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
                ui->PtpHcCurrentDsOffsetValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
        ui->PtpHcCurrentDsOffsetValue->setText("NA");
    }

    //********************************
    // parent dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ParentDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // parent clock id and port id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs1Reg, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs2Reg, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(".");
                            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs3Reg, temp_data))
                            {
                                temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                                ui->PtpHcParentDsParentClockIdValue->setText(temp_string);
                            }
                            else
                            {
                                ui->PtpHcParentDsParentClockIdValue->setText("NA");
                            }
                        }
                        else
                        {
                            ui->PtpHcParentDsParentClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcParentDsParentClockIdValue->setText("NA");
                    }

                    // gm clock id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs4Reg, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs5Reg, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            ui->PtpHcParentDsGmClockIdValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpHcParentDsGmClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcParentDsGmClockIdValue->setText("NA");
                    }

                    // gm priority 1 & 2
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs3Reg, temp_data))
                    {
                        ui->PtpHcParentDsGmPriority2Value->setText(QString("0x%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpHcParentDsGmPriority1Value->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpHcParentDsGmPriority1Value->setText("NA");
                        ui->PtpHcParentDsGmPriority2Value->setText("NA");
                    }

                    // variance, accuracy ,class
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs6Reg, temp_data))
                    {
                        ui->PtpHcParentDsGmVarianceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                        ui->PtpHcParentDsGmAccuracyValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
                        ui->PtpHcParentDsGmClassValue->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpHcParentDsGmVarianceValue->setText("NA");
                        ui->PtpHcParentDsGmAccuracyValue->setText("NA");
                        ui->PtpHcParentDsGmClassValue->setText("NA");
                    }

                    // gm short id
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs7Reg, temp_data))
                    {
                        ui->PtpHcParentDsGmShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpHcParentDsGmShortIdValue->setText("NA");
                    }

                    // gm inaccuracy
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs8Reg, temp_data))
                    {
                        ui->PtpHcParentDsGmInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
                    }

                    // nw inaccuracy
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_ParentDs9Reg, temp_data))
                    {
                        ui->PtpHcParentDsNwInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpHcParentDsNwInaccuracyValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpHcParentDsParentClockIdValue->setText("NA");
                    ui->PtpHcParentDsGmClockIdValue->setText("NA");
                    ui->PtpHcParentDsGmPriority1Value->setText("NA");
                    ui->PtpHcParentDsGmPriority2Value->setText("NA");
                    ui->PtpHcParentDsGmAccuracyValue->setText("NA");
                    ui->PtpHcParentDsGmClassValue->setText("NA");
                    ui->PtpHcParentDsGmShortIdValue->setText("NA");
                    ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
                    ui->PtpHcParentDsNwInaccuracyValue->setText("NA");

                }

            }
            else
            {
                ui->PtpHcParentDsParentClockIdValue->setText("NA");
                ui->PtpHcParentDsGmClockIdValue->setText("NA");
                ui->PtpHcParentDsGmPriority1Value->setText("NA");
                ui->PtpHcParentDsGmPriority2Value->setText("NA");
                ui->PtpHcParentDsGmAccuracyValue->setText("NA");
                ui->PtpHcParentDsGmClassValue->setText("NA");
                ui->PtpHcParentDsGmShortIdValue->setText("NA");
                ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
                ui->PtpHcParentDsNwInaccuracyValue->setText("NA");

            }
        }
    }
    else
    {
        ui->PtpHcParentDsParentClockIdValue->setText("NA");
        ui->PtpHcParentDsGmClockIdValue->setText("NA");
        ui->PtpHcParentDsGmPriority1Value->setText("NA");
        ui->PtpHcParentDsGmPriority2Value->setText("NA");
        ui->PtpHcParentDsGmAccuracyValue->setText("NA");
        ui->PtpHcParentDsGmClassValue->setText("NA");
        ui->PtpHcParentDsGmShortIdValue->setText("NA");
        ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
        ui->PtpHcParentDsNwInaccuracyValue->setText("NA");

    }

    //********************************
    // time properties dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // time source, ptp timescale, freq traceable, time traceable, lep61, leap 59, ut offset val, utc offset
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
                    {
                        ui->PtpHcTimePropertiesDsTimeSourceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        if ((temp_data & 0x00000100) != 0)
                        {
                            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00000200) != 0)
                        {
                            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00000400) != 0)
                        {
                            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00000800) != 0)
                        {
                            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00001000) != 0)
                        {
                            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00002000) != 0)
                        {
                            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                        }

                        ui->PtpHcTimePropertiesDsUtcOffsetValue->setText(QString::number((signed short)((temp_data >> 16) & 0x0000FFFF)));
                    }
                    else
                    {
                        ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
                        ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                        ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                        ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                        ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
                        ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
                        ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                        ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
                    }

                    // current offset
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs2Reg, temp_data))
                    {
                        ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
                    }

                    // jump seconds
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs3Reg, temp_data))
                    {
                        ui->PtpHcTimePropertiesDsJumpSecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
                    }

                    // next jump
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs4Reg, temp_data))
                    {
                        temp_next_jump = temp_data;
                        temp_next_jump = temp_next_jump << 32;
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs5Reg, temp_data))
                        {
                            temp_next_jump |= temp_data;
                            ui->PtpHcTimePropertiesDsNextJumpValue->setText(QString::number(temp_next_jump));
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
                    }

                    // display name
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs6Reg, temp_length))
                    {
                        for (int j=0; j<3; j++)
                        {
                            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs7Reg + (j*4), temp_data))
                            {
                                temp_string.append(((temp_data >> 0) & 0x000000FF));
                                temp_string.append(((temp_data >> 8) & 0x000000FF));
                                temp_string.append(((temp_data >> 16) & 0x000000FF));
                                temp_string.append(((temp_data >> 24) & 0x000000FF));
                            }
                            else
                            {
                                temp_string.clear();
                                temp_string.append("NA");
                                break;
                            }
                        }
                        temp_string.truncate(temp_length);
                        ui->PtpHcTimePropertiesDsDisplayNameValue->setText(temp_string);
                    }
                    else
                    {
                        ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
                    }
                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
                    ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                    ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                    ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                    ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
                    ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
                    ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                    ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
                    ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
                    ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
                    ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
                    ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
                }

            }
            else
            {
                ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
                ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
                ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
                ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
                ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
                ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
                ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
                ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
        ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
        ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
        ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
        ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
        ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
        ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
        ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
        ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
        ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
        ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_VersionReg, temp_data))
    {
        ui->PtpHcVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->PtpHcVersionValue->setText("NA");
    }


    // PTP TC Part
    //********************************
    // default dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // nr of ports
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDs4Reg, temp_data))
                    {
                        temp_data = temp_data - 1;
                        ui->PtpHcDefaultDsNrOfPortsValue->setText(QString::number(temp_data));
                        nr_of_ports = temp_data;
                        if (0 == ui->PtpHcPortDsPortNrComboBox->count())
                        {
                            ui->PtpHcPortDsPortNrComboBox->clear();
                            for (unsigned int i = 0; i < temp_data; i++)
                            {
                                ui->PtpHcPortDsPortNrComboBox->addItem(QString::number(i));
                            }
                        }
                    }
                    else
                    {
                        ui->PtpHcPortDsPortNrComboBox->clear();
                        ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
                    ui->PtpHcPortDsPortNrComboBox->clear();
                }

            }
            else
            {
                ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
                ui->PtpHcPortDsPortNrComboBox->clear();
            }
        }
    }
    else
    {
        ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
        ui->PtpHcPortDsPortNrComboBox->clear();
    }

    //********************************
    // port dataset
    //********************************
    temp_string = ui->PtpHcPortDsPortNrComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    temp_data = ((temp_data & 0x000000FF) << 16);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
    {
        temp_data |= 0x40000000;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
        {
            for (int i = 0; i < 10; i++)
            {
                if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
                {
                    if ((temp_data & 0x80000000) != 0)
                    {

                        temp_string = ui->PtpHcDelayMechanismValue->currentText();
                        if (temp_string == "E2E")
                        {
                            // end to end delay
                            ui->PtpHcPortDsPeerDelayValue->setText("NA");
                        }
                        else if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs1Reg, temp_data))
                        {
                            // peer delay
                            temp_delay = temp_data;
                            temp_delay = temp_delay << 32;
                            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs2Reg, temp_data))
                            {
                                temp_delay |= temp_data;
                                temp_delay = temp_delay >> 16;
                                ui->PtpHcPortDsPeerDelayValue->setText(QString::number(temp_delay));
                            }
                            else
                            {
                                ui->PtpHcPortDsPeerDelayValue->setText("NA");
                            }
                        }
                        else
                        {
                            ui->PtpHcPortDsPeerDelayValue->setText("NA");
                        }

                        // pdelay log msg interval
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs3Reg, temp_data))
                        {
                            ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));

                        }
                        else
                        {
                            ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        }

                        // asymmetry
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs4Reg, temp_data))
                        {
                            ui->PtpHcPortDsAsymmetryValue->setText(QString::number((signed int)temp_data));

                        }
                        else
                        {
                            ui->PtpHcPortDsAsymmetryValue->setText("NA");
                        }

                        // max pdelay
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs5Reg, temp_data))
                        {
                            ui->PtpHcPortDsMaxPeerDelayValue->setText(QString::number(temp_data));

                        }
                        else
                        {
                            ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
                        }

                        break;
                    }
                    else if (i == 9)
                    {
                        cout << "ERROR: " << "read did not complete" << endl;
                        ui->PtpHcPortDsPeerDelayValue->setText("NA");
                        ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        ui->PtpHcPortDsAsymmetryValue->setText("NA");
                        ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
                    }

                }
                else
                {
                    ui->PtpHcPortDsPeerDelayValue->setText("NA");
                    ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsAsymmetryValue->setText("NA");
                    ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
                }
            }
        }
        else
        {
            ui->PtpHcPortDsPeerDelayValue->setText("NA");
            ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsAsymmetryValue->setText("NA");
            ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
        }

    }
    else
    {
        ui->PtpHcPortDsPeerDelayValue->setText("NA");
        ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
        ui->PtpHcPortDsAsymmetryValue->setText("NA");
        ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
    }

    if (temp_string == "E2E")
    {
        // nothing here
    }
    else
    {
        // Chart
        if (nr_of_ports > 3) // limit to the first 3
        {
            nr_of_ports = 3;
        }
        temp_min = 0;
        temp_max = 0;
        for (unsigned int k= 0; k < nr_of_ports; k++)
        {
            temp_data = ((k & 0x000000FF) << 16);
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
            {
                temp_data |= 0x40000000;
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
                {
                    for (int i = 0; i < 10; i++)
                    {
                        if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
                        {
                            if ((temp_data & 0x80000000) != 0)
                            {

                                // peer delay
                                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs1Reg, temp_data))
                                {
                                    temp_delay = temp_data;
                                    temp_delay = temp_delay << 32;
                                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs2Reg, temp_data))
                                    {
                                        temp_delay |= temp_data;
                                        temp_delay = temp_delay >> 16;

                                        if (true == ptp_hc_timer->isActive())
                                        {
                                            ptp_hc_delay_series.at(k)->append(ptp_hc_delay_number_of_points, temp_delay);

                                            if (ptp_hc_delay_number_of_points < 20)
                                            {
                                                if (k == (nr_of_ports-1))
                                                {
                                                    ptp_hc_delay_number_of_points++;
                                                }
                                            }
                                            else
                                            {
                                                for (int j = 1; j < ptp_hc_delay_series.at(k)->count(); j++)
                                                {
                                                    QPointF temp_point = ptp_hc_delay_series.at(k)->at(j);
                                                    ptp_hc_delay_series.at(k)->replace(j, (j-1), temp_point.y());
                                                }
                                                ptp_hc_delay_series.at(k)->remove(0);
                                            }

                                            for (int j = 0; j < ptp_hc_delay_series.at(k)->count(); j++)
                                            {
                                                QPointF temp_point = ptp_hc_delay_series.at(k)->at(j);
                                                if ((j == 0) && (k == 0))
                                                {
                                                    temp_min = temp_point.y();
                                                    temp_max = temp_point.y();
                                                }
                                                if (temp_min > temp_point.y())
                                                {
                                                    temp_min = temp_point.y();
                                                }
                                                if (temp_max < temp_point.y())
                                                {
                                                    temp_max = temp_point.y();
                                                }
                                            }

                                            if (k == (nr_of_ports-1))
                                            {
                                                temp_max = ((temp_max/100)+1)*100;
                                                temp_min = ((temp_min/100)-1)*100;
                                                if (temp_min < 0)
                                                {
                                                    temp_min = 0;
                                                }
                                                ptp_hc_delay_chart->axisY()->setMin(temp_min);
                                                ptp_hc_delay_chart->axisY()->setMax(temp_max);

                                                ptp_hc_delay_chart->show();
                                            }                                            
                                        }
                                    }
                                }
                                break;
                            }
                            else if (i == 9)
                            {
                                cout << "ERROR: " << "read did not complete" << endl;
                            }
                        }
                    }
                }
            }
        }
    }

}

void Ucm_PtpHcTab::ptp_hc_write_values(void)
{
    unsigned long long temp_next_jump;
    unsigned long long temp_mac;
    unsigned long temp_ip;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->PtpHcInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PtpHybridClockCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PtpHcVlanValue->setText("NA");
            ui->PtpHcProfileValue->setCurrentText("NA");
            ui->PtpHcLayerValue->setCurrentText("NA");
            ui->PtpHcDelayMechanismValue->setCurrentText("NA");
            ui->PtpHcIpValue->setText("NA");
            ui->PtpHcVersionValue->setText("NA");

            ui->PtpHcDefaultDsClockIdValue->setText("NA");
            ui->PtpHcDefaultDsDomainValue->setText("NA");
            ui->PtpHcDefaultDsPriority1Value->setText("NA");
            ui->PtpHcDefaultDsPriority2Value->setText("NA");
            ui->PtpHcDefaultDsVarianceValue->setText("NA");
            ui->PtpHcDefaultDsAccuracyValue->setText("NA");
            ui->PtpHcDefaultDsClassValue->setText("NA");
            ui->PtpHcDefaultDsShortIdValue->setText("NA");
            ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
            ui->PtpHcDefaultDsNrOfPortsValue->setText("NA");
            ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
            ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);

            ui->PtpHcPortDsPeerDelayValue->setText("NA");
            ui->PtpHcPortDsPortNrComboBox->clear();
            ui->PtpHcPortDsStateValue->setText("NA");
            ui->PtpHcPortDsAsymmetryValue->setText("NA");
            ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
            ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
            ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");

            ui->PtpHcCurrentDsStepsRemovedValue->setText("NA");
            ui->PtpHcCurrentDsOffsetValue->setText("NA");

            ui->PtpHcParentDsParentClockIdValue->setText("NA");
            ui->PtpHcParentDsGmClockIdValue->setText("NA");
            ui->PtpHcParentDsGmPriority1Value->setText("NA");
            ui->PtpHcParentDsGmPriority2Value->setText("NA");
            ui->PtpHcParentDsGmAccuracyValue->setText("NA");
            ui->PtpHcParentDsGmClassValue->setText("NA");
            ui->PtpHcParentDsGmShortIdValue->setText("NA");
            ui->PtpHcParentDsGmInaccuracyValue->setText("NA");
            ui->PtpHcParentDsNwInaccuracyValue->setText("NA");

            ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
            ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
            ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
            ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
            ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
            ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");

            ui->PtpHcVlanEnableCheckBox->setChecked(false);
            ui->PtpHcVersionValue->setText("NA");
            return;
        }
    }

    // PTP OC Part
    // profile and layer
    temp_string = ui->PtpHcProfileValue->currentText();
    if (temp_string == "Default")
    {
        temp_data = 0x00000000;
    }
    else if (temp_string == "Power")
    {
        temp_data = 0x00000001;
    }
    else if (temp_string == "Utility")
    {
        temp_data = 0x00000002;
    }
    else if (temp_string == "TSN")
    {
        temp_data = 0x00000003;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else
    {
        if (true == ui->PtpHcDefaultDsTwoStepCheckBox->isChecked())
        {
            temp_data |= 0x00000100;
        }

        if (true == ui->PtpHcDefaultDsSignalingCheckBox->isChecked())
        {
            temp_data |= 0x00000200;
        }

        temp_string = ui->PtpHcLayerValue->currentText();
        if (temp_string == "Layer 2")
        {
            temp_data |= 0x00000000;
        }
        else if (temp_string == "Layer 3")
        {
            temp_data |= 0x00010000;
        }
        else
        {
            temp_data |= 0x00000000;
        }

        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_string = ui->PtpHcDelayMechanismValue->currentText();
            if (temp_string == "P2P")
            {
                temp_data |= 0x00000000;
            }
            else if (temp_string == "E2E")
            {
                temp_data |= 0x01000000;
            }
            else
            {
                temp_data |= 0x00000000;
            }

            if (temp_string == "NA")
            {
                //nothing
            }
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ConfigProfileReg, temp_data))
            {
                switch (temp_data & 0x00000003)
                {
                case 0:
                    ui->PtpHcProfileValue->setCurrentText("Default");
                    break;
                case 1:
                    ui->PtpHcProfileValue->setCurrentText("Power");
                    break;
                case 2:
                    ui->PtpHcProfileValue->setCurrentText("Utility");
                    break;
                case 3:
                    ui->PtpHcProfileValue->setCurrentText("TSN");
                    break;
                default:
                    ui->PtpHcProfileValue->setCurrentText("NA");
                    break;
                }

                switch ((temp_data >> 8) & 0x00000001)
                {
                case 0:
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                    break;
                case 1:
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(true);
                    break;
                default:
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                    break;
                }

                switch ((temp_data >> 9) & 0x00000001)
                {
                case 0:
                    ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
                    break;
                case 1:
                    ui->PtpHcDefaultDsSignalingCheckBox->setChecked(true);
                    break;
                default:
                    ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
                    break;
                }

                switch ((temp_data >> 16) & 0x00000001)
                {
                case 0:
                    ui->PtpHcLayerValue->setCurrentText("Layer 2");
                    break;
                case 1:
                    ui->PtpHcLayerValue->setCurrentText("Layer 3");
                    break;
                default:
                    ui->PtpHcLayerValue->setCurrentText("NA");
                    break;
                }

                switch ((temp_data >> 24) & 0x00000001)
                {
                case 0:
                    ui->PtpHcDelayMechanismValue->setCurrentText("P2P");
                    break;
                case 1:
                    ui->PtpHcDelayMechanismValue->setCurrentText("E2E");
                    break;
                default:
                    ui->PtpHcDelayMechanismValue->setCurrentText("NA");
                    break;
                }

                temp_data = 0x00000001; // write
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpHcProfileValue->setCurrentText("NA");
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                    ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
                    ui->PtpHcLayerValue->setCurrentText("NA");
                    ui->PtpHcDelayMechanismValue->setCurrentText("NA");
                }
            }
            else
            {
                ui->PtpHcProfileValue->setCurrentText("NA");
                ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                ui->PtpHcDefaultDsSignalingCheckBox->setChecked(false);
                ui->PtpHcLayerValue->setCurrentText("NA");
                ui->PtpHcDelayMechanismValue->setCurrentText("NA");
            }
        }
    }

    // vlan
    temp_string = ui->PtpHcVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if(true == ui->PtpHcVlanEnableCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // enable
    }
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ConfigVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->PtpHcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcVlanEnableCheckBox->setChecked(false);
            ui->PtpHcVlanValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcVlanEnableCheckBox->setChecked(false);
        ui->PtpHcVlanValue->setText("NA");
    }

    // ip
    temp_string = ui->PtpHcIpValue->text();
    temp_ip = QHostAddress(temp_string).toIPv4Address();

    temp_data = 0x00000000;
    temp_data |= (temp_ip >> 0) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 8) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 16) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 24) & 0x000000FF;

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ConfigIpReg, temp_data))
    {
        temp_string.clear();

        temp_ip = 0x00000000;
        temp_ip |= (temp_data >> 0) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 8) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 16) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 24) & 0x000000FF;

        temp_string = QHostAddress(temp_ip).toString();

        temp_data = 0x00000004; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ConfigControlReg, temp_data))
        {
            ui->PtpHcIpValue->setText(temp_string);
        }
        else
        {
            ui->PtpHcIpValue->setText("NA");
        }

    }
    else
    {
        ui->PtpHcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    // clock id
    temp_string = ui->PtpHcDefaultDsClockIdValue->text();
    temp_string.remove(QChar(':'), Qt::CaseInsensitive);
    temp_mac = temp_string.toULongLong(nullptr, 16);
    temp_data = 0x00000000;
    temp_data |= (temp_mac >> 32) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 40) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 48) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 56) & 0x000000FF;

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs1Reg, temp_data))
    {
        temp_string.clear();
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");

        temp_data = 0x00000000;
        temp_data |= (temp_mac >> 0) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 8) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 16) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 24) & 0x000000FF;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs2Reg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
            {
                ui->PtpHcDefaultDsClockIdValue->setText(temp_string);
            }
            else
            {
                ui->PtpHcDefaultDsClockIdValue->setText("NA");
            }
        }
        else
        {
            ui->PtpHcDefaultDsClockIdValue->setText("NA");
        }

    }
    else
    {
        ui->PtpHcDefaultDsClockIdValue->setText("NA");
    }

    // domain
    temp_string = ui->PtpHcDefaultDsDomainValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->PtpHcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcDefaultDsDomainValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcDefaultDsDomainValue->setText("NA");
    }


    // priority 1
    temp_string = ui->PtpHcDefaultDsPriority1Value->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data = temp_data << 24;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, temp_data))
    {
        ui->PtpHcDefaultDsPriority1Value->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

        temp_data = 0x00000008; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcDefaultDsPriority1Value->setText("NA");
        }
    }
    else
    {
        ui->PtpHcDefaultDsPriority1Value->setText("NA");
    }

    // priority 2
    temp_string = ui->PtpHcDefaultDsPriority2Value->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data = temp_data << 16;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs3Reg, temp_data))
    {
        ui->PtpHcDefaultDsPriority2Value->setText(QString("0x%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));

        temp_data = 0x00000010; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcDefaultDsPriority2Value->setText("NA");
        }
    }
    else
    {
        ui->PtpHcDefaultDsPriority2Value->setText("NA");
    }

    // accuracy, class, variance
    temp_string = ui->PtpHcDefaultDsClassValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data = temp_data << 8;
    if (temp_string == "NA")
    {
        //nothing
    }
    else
    {
        temp_string = ui->PtpHcDefaultDsAccuracyValue->text();
        temp_data |= temp_string.toUInt(nullptr, 10);
        temp_data = temp_data << 16;
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_string = ui->PtpHcDefaultDsVarianceValue->text();
            temp_data |= temp_string.toUInt(nullptr, 16);
            if (temp_string == "NA")
            {
                //nothing
            }
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs4Reg, temp_data))
            {
                temp_data = 0x00000004; // write
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpHcDefaultDsVarianceValue->setText("NA");
                    ui->PtpHcDefaultDsAccuracyValue->setText("NA");
                    ui->PtpHcDefaultDsClassValue->setText("NA");
                }
            }
            else
            {
                ui->PtpHcDefaultDsVarianceValue->setText("NA");
                ui->PtpHcDefaultDsAccuracyValue->setText("NA");
                ui->PtpHcDefaultDsClassValue->setText("NA");
            }
        }
    }

    // short id
    temp_string = ui->PtpHcDefaultDsShortIdValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs5Reg, temp_data))
    {
        ui->PtpHcDefaultDsShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000020; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcDefaultDsShortIdValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcDefaultDsShortIdValue->setText("NA");
    }


    // inaccuracy
    temp_string = ui->PtpHcDefaultDsInaccuracyValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDs6Reg, temp_data))
    {
        ui->PtpHcDefaultDsInaccuracyValue->setText(QString::number(temp_data));

        temp_data = 0x00000040; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcDefaultDsInaccuracyValue->setText("NA");
    }


    //********************************
    // port dataset
    //********************************
    // delay mechanism
    temp_data = 0x00000001;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, temp_data))
    {
        //nothing
    }
    else
    {
        ui->PtpHcDelayMechanismValue->setCurrentText("NA");
    }

    if (true == ui->PtpHcPortDsSetCustomIntervalsCheckBox->isChecked())
    {
        // delay message intervals
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpOc_PortDs4Reg, temp_data))
        {
            // keep peer delay req message interval
            temp_data &= 0x000000FF;
            // merge with delay req message interval
            temp_string = ui->PtpHcPortDsDelayReqLogMsgIntervalValue->text();
            temp_data |= ((temp_string.toInt(nullptr, 10) & 0x000000FF) << 8);
            if (temp_string == "NA")
            {
                //nothing
            }
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_PortDs4Reg, temp_data))
            {
                // announce message intervals
                temp_string = ui->PtpHcPortDsAnnounceReceiptTimeoutValue->text();
                temp_data = (temp_string.toUInt(nullptr, 10) & 0x000000FF);
                temp_data = temp_data << 8;
                temp_string = ui->PtpHcPortDsAnnounceLogMsgIntervalValue->text();
                temp_data |= (temp_string.toInt(nullptr, 10) & 0x000000FF);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_PortDs5Reg, temp_data))
                {
                    // sync message interval
                    temp_string = ui->PtpHcPortDsSyncReceiptTimeoutValue->text();
                    temp_data = (temp_string.toUInt(nullptr, 10) & 0x000000FF);
                    temp_data = temp_data << 8;
                    temp_string = ui->PtpHcPortDsSyncLogMsgIntervalValue->text();
                    temp_data |= (temp_string.toInt(nullptr, 10) & 0x000000FF);
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_PortDs6Reg, temp_data))
                    {
                        // set intervals
                        temp_data = 0x00000002;
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_PortDsControlReg, temp_data))
                        {
                            //nothing
                        }
                        else
                        {
                            ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                            ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                            ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                            ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                            ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                        ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                        ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                        ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                        ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                    ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                    ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
                }
            }
            else
            {
                ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
                ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
                ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
                ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
                ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
            }
        }
        else
        {
            ui->PtpHcPortDsDelayReqLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsAnnounceReceiptTimeoutValue->setText("NA");
            ui->PtpHcPortDsAnnounceLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsSyncLogMsgIntervalValue->setText("NA");
            ui->PtpHcPortDsSyncReceiptTimeoutValue->setText("NA");
        }
    }

    //********************************
    // current dataset
    //********************************
    // all RO

    //********************************
    // parent dataset
    //********************************
    // all RO

    //********************************
    // time properties dataset
    //********************************
    // time source
    temp_string = ui->PtpHcTimePropertiesDsTimeSourceValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->PtpHcTimePropertiesDsTimeSourceValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

        temp_data = 0x00000080; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsTimeSourceValue->setText("NA");
    }

    // ptp timescale
    if(true == ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->isChecked())
    {
        temp_data = 0x00000100; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        if ((temp_data & 0x00000100) != 0)
        {
            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        }

        temp_data = 0x00000040; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
    }

    // freq traceable
    if(true == ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->isChecked())
    {
        temp_data = 0x00000200; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        if ((temp_data & 0x00000200) != 0)
        {
            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
        }

        temp_data = 0x00000020; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
    }

    // time traceable
    if(true == ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->isChecked())
    {
        temp_data = 0x00000400; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        if ((temp_data & 0x00000400) != 0)
        {
            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
        }

        temp_data = 0x00000010; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
    }

    // leap 61
    if(true == ui->PtpHcTimePropertiesDsLeap61CheckBox->isChecked())
    {
        temp_data = 0x00000800; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        if ((temp_data & 0x00000800) != 0)
        {
            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(true);
        }
        else
        {
            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
        }

        temp_data = 0x00000008; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsLeap61CheckBox->setChecked(false);
    }

    // leap 59
    if(true == ui->PtpHcTimePropertiesDsLeap59CheckBox->isChecked())
    {
        temp_data = 0x00001000; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        if ((temp_data & 0x00001000) != 0)
        {
            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(true);
        }
        else
        {
            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
        }

        temp_data = 0x00000004; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsLeap59CheckBox->setChecked(false);
    }

    // utc offset val
    if(true == ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->isChecked())
    {
        temp_data = 0x00002000; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        if ((temp_data & 0x00002000) != 0)
        {
            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
        }

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
    }


    // utc offset
    temp_string = ui->PtpHcTimePropertiesDsUtcOffsetValue->text();
    temp_data = temp_string.toInt(nullptr, 10);
    temp_data = temp_data << 16;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs1Reg, temp_data))
    {
        ui->PtpHcTimePropertiesDsUtcOffsetValue->setText(QString::number((temp_data >> 16) & 0x0000FFFF));

        temp_data = 0x00000001; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsUtcOffsetValue->setText("NA");
    }


    // current offset
    temp_string = ui->PtpHcTimePropertiesDsCurrentOffsetValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs2Reg, temp_data))
    {
        ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText(QString::number(temp_data));
        temp_data = 0x00000100; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsCurrentOffsetValue->setText("NA");
    }

    // jump seconds
    temp_string = ui->PtpHcTimePropertiesDsJumpSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs3Reg, temp_data))
    {
        ui->PtpHcTimePropertiesDsJumpSecondsValue->setText(QString::number(temp_data));
        temp_data = 0x00000200; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsJumpSecondsValue->setText("NA");
    }

    // next jump
    temp_string = ui->PtpHcTimePropertiesDsNextJumpValue->text();
    temp_next_jump = temp_string.toULongLong(nullptr, 10);
    temp_data = temp_next_jump >> 32;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs4Reg, temp_data))
    {
        temp_data = temp_next_jump & 0x00000000FFFFFFFF;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs5Reg, temp_data))
        {
            ui->PtpHcTimePropertiesDsNextJumpValue->setText(QString::number(temp_next_jump));
            temp_data = 0x00000400; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
            }
        }
        else
        {
            ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcTimePropertiesDsNextJumpValue->setText("NA");
    }



    // display name
    temp_string = ui->PtpHcTimePropertiesDsDisplayNameValue->text();
    if (temp_string == "NA")
    {
        //nothing
    }
    else
    {
        for (int j=0; j<3; j++)
        {
            temp_data = 0x00000000;
            if ((j*4)+0 < temp_string.size())
            {
                temp_data |= ((temp_string.at((j*4)+0).toLatin1()) & 0x000000FF) << 0;
            }

            if ((j*4)+1 < temp_string.size())
            {
                temp_data |= ((temp_string.at((j*4)+1).toLatin1()) & 0x000000FF) << 8;
            }

            if ((j*4)+2 < temp_string.size())
            {
                temp_data |= ((temp_string.at((j*4)+2).toLatin1()) & 0x000000FF) << 16;
            }

            if ((j*4)+3 < temp_string.size())
            {
                temp_data |= ((temp_string.at((j*4)+3).toLatin1()) & 0x000000FF) << 24;
            }

            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs7Reg + (j*4), temp_data))
            {
                if (j == 2)
                {
                    temp_data = temp_string.size(); // length
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDs6Reg, temp_data))
                    {
                        temp_data = 0x00001800; // write
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_TimePropertiesDsControlReg, temp_data))
                        {
                            ui->PtpHcTimePropertiesDsDisplayNameValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
                    }
                }
            }
            else
            {
                ui->PtpHcTimePropertiesDsDisplayNameValue->setText("NA");
                break;
            }
        }
    }

    // PTP TC Part
    // profile and layer
    temp_string = ui->PtpHcProfileValue->currentText();
    if (temp_string == "Default")
    {
        temp_data = 0x00000000;
    }
    else if (temp_string == "Power")
    {
        temp_data = 0x00000001;
    }
    else if (temp_string == "Utility")
    {
        temp_data = 0x00000002;
    }
    else if (temp_string == "TSN")
    {
        temp_data = 0x00000003;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else
    {
        if (true == ui->PtpHcDefaultDsTwoStepCheckBox->isChecked())
        {
            temp_data |= 0x00000100;
        }

        temp_string = ui->PtpHcLayerValue->currentText();
        if (temp_string == "Layer 2")
        {
            temp_data |= 0x00000000;
        }
        else if (temp_string == "Layer 3")
        {
            temp_data |= 0x00010000;
        }
        else
        {
            temp_data |= 0x00000000;
        }

        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_string = ui->PtpHcDelayMechanismValue->currentText();
            if (temp_string == "P2P")
            {
                temp_data |= 0x00000000;
            }
            else if (temp_string == "E2E")
            {
                temp_data |= 0x01000000;
            }
            else
            {
                temp_data |= 0x00000000;
            }

            if (temp_string == "NA")
            {
                //nothing
            }
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ConfigProfileReg, temp_data))
            {
                switch (temp_data & 0x00000003)
                {
                case 0:
                    ui->PtpHcProfileValue->setCurrentText("Default");
                    break;
                case 1:
                    ui->PtpHcProfileValue->setCurrentText("Power");
                    break;
                case 2:
                    ui->PtpHcProfileValue->setCurrentText("Utility");
                    break;
                case 3:
                    ui->PtpHcProfileValue->setCurrentText("TSN");
                    break;
                default:
                    ui->PtpHcProfileValue->setCurrentText("NA");
                    break;
                }

                switch ((temp_data >> 8) & 0x00000001)
                {
                case 0:
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                    break;
                case 1:
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(true);
                    break;
                default:
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                    break;
                }

                switch ((temp_data >> 16) & 0x00000001)
                {
                case 0:
                    ui->PtpHcLayerValue->setCurrentText("Layer 2");
                    break;
                case 1:
                    ui->PtpHcLayerValue->setCurrentText("Layer 3");
                    break;
                default:
                    ui->PtpHcLayerValue->setCurrentText("NA");
                    break;
                }

                switch ((temp_data >> 24) & 0x00000001)
                {
                case 0:
                    ui->PtpHcDelayMechanismValue->setCurrentText("P2P");
                    break;
                case 1:
                    ui->PtpHcDelayMechanismValue->setCurrentText("E2E");
                    break;
                default:
                    ui->PtpHcDelayMechanismValue->setCurrentText("NA");
                    break;
                }

                temp_data = 0x00000001; // write
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ConfigControlReg, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpHcProfileValue->setCurrentText("NA");
                    ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                    ui->PtpHcLayerValue->setCurrentText("NA");
                    ui->PtpHcDelayMechanismValue->setCurrentText("NA");
                }
            }
            else
            {
                ui->PtpHcProfileValue->setCurrentText("NA");
                ui->PtpHcDefaultDsTwoStepCheckBox->setChecked(false);
                ui->PtpHcLayerValue->setCurrentText("NA");
                ui->PtpHcDelayMechanismValue->setCurrentText("NA");
            }
        }
    }

    // vlan
    temp_string = ui->PtpHcVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if(true == ui->PtpHcVlanEnableCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // enable
    }
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ConfigVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->PtpHcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcVlanEnableCheckBox->setChecked(false);
            ui->PtpHcVlanValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcVlanEnableCheckBox->setChecked(false);
        ui->PtpHcVlanValue->setText("NA");
    }


    // ip
    temp_string = ui->PtpHcIpValue->text();
    temp_data = QHostAddress(temp_string).toIPv4Address();

    temp_data = 0x00000000;
    temp_data |= (temp_ip >> 0) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 8) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 16) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 24) & 0x000000FF;

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ConfigIpReg, temp_data))
    {
        temp_string.clear();

        temp_ip = 0x00000000;
        temp_ip |= (temp_data >> 0) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 8) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 16) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 24) & 0x000000FF;

        temp_string = QHostAddress(temp_ip).toString();

        temp_data = 0x00000004; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ConfigControlReg, temp_data))
        {
            ui->PtpHcIpValue->setText(temp_string);
        }
        else
        {
            ui->PtpHcIpValue->setText("NA");
        }

    }
    else
    {
        ui->PtpHcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    // clock id
    temp_string = ui->PtpHcDefaultDsClockIdValue->text();
    temp_string.remove(QChar(':'), Qt::CaseInsensitive);
    temp_mac = temp_string.toULongLong(nullptr, 16);
    temp_data = 0x00000000;
    temp_data |= (temp_mac >> 32) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 40) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 48) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 56) & 0x000000FF;

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDs1Reg, temp_data))
    {
        temp_string.clear();
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");

        temp_data = 0x00000000;
        temp_data |= (temp_mac >> 0) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 8) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 16) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 24) & 0x000000FF;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + 0x00000108, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDsControlReg, temp_data))
            {
                ui->PtpHcDefaultDsClockIdValue->setText(temp_string);
            }
            else
            {
                ui->PtpHcDefaultDsClockIdValue->setText("NA");
            }
        }
        else
        {
            ui->PtpHcDefaultDsClockIdValue->setText("NA");
        }

    }
    else
    {
        ui->PtpHcDefaultDsClockIdValue->setText("NA");
    }

    // domain
    temp_string = ui->PtpHcDefaultDsDomainValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + 0x0000010C, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->PtpHcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpHcDefaultDsDomainValue->setText("NA");
        }
    }
    else
    {
        ui->PtpHcDefaultDsDomainValue->setText("NA");
    }


    //********************************
    // port dataset
    //********************************
    // port id
    for (unsigned int i = 0; i < (ui->PtpHcDefaultDsNrOfPortsValue->text().toUInt(nullptr, 10) + 1); i++)
    {
        temp_data = ((i & 0x000000FF) << 16);
        temp_data |= 0x00000001; // set the delay mechanism
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
        {
            temp_data = 0x00000001; // write for each port
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_DefaultDsControlReg, temp_data))
            {
               // ok
            }
            else
            {
                ui->PtpHcDefaultDsClockIdValue->setText("NA");
            }

            if (true == ui->PtpHcPortDsSetCustomIntervalsCheckBox->isChecked())
            {
                // only for the selected port
                if (i == ui->PtpHcPortDsPortNrComboBox->currentText().toUInt(nullptr, 10))
                {
                    temp_string = ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->text();
                    temp_data = (temp_string.toInt(nullptr, 10) & 0x0000000FF);
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs3Reg, temp_data))
                    {
                        temp_data = ((i & 0x000000FF) << 16);
                        temp_data |= 0x00000002; // set the message interval
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
                        {
                            // ok
                        }
                        else
                        {
                            ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpHcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    }

                    // port to OC always the same
                }
            }

            // only for the selected port
            if (i == ui->PtpHcPortDsPortNrComboBox->currentText().toUInt(nullptr, 10))
            {
                temp_string = ui->PtpHcPortDsAsymmetryValue->text();
                temp_data = temp_string.toInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs4Reg, temp_data))
                {
                    temp_data = ((i & 0x000000FF) << 16);
                    temp_data |= 0x00000004; // set the asymmetry
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
                    {
                        // ok
                    }
                    else
                    {
                        ui->PtpHcPortDsAsymmetryValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpHcPortDsAsymmetryValue->setText("NA");
                }
            }

            // only for the selected port
            if (i == ui->PtpHcPortDsPortNrComboBox->currentText().toUInt(nullptr, 10))
            {
                temp_string = ui->PtpHcPortDsMaxPeerDelayValue->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDs5Reg, temp_data))
                {
                    temp_data = ((i & 0x000000FF) << 16);
                    temp_data |= 0x00000008; // set the max pdelay
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_PortDsControlReg, temp_data))
                    {
                        // ok
                    }
                    else
                    {
                        ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpHcPortDsMaxPeerDelayValue->setText("NA");
                }
            }
        }
        else
        {
            // port clock id wrong
        }
    }

    // PTP OC and PTP TC enable
    temp_data = 0x00000000; // nothing
    if(true == ui->PtpHcEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if ((0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpHc_TcOffset + Ucm_PtpTc_ControlReg, temp_data)) &&
        (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpOc_ControlReg, temp_data)))
    {
        // nothing
    }
    else
    {
        ui->PtpHcEnableCheckBox->setChecked(false);
    }
}

void Ucm_PtpHcTab::ptp_hc_read_values_button_clicked(void)
{
    ptp_hc_read_values();
}

void Ucm_PtpHcTab::ptp_hc_write_values_button_clicked(void)
{
    ptp_hc_write_values();
    ptp_hc_read_values();
}

void Ucm_PtpHcTab::ptp_hc_auto_refresh_button_clicked(void)
{
    if (ui->PtpHcAutoRefreshButton->text() == "Start Refresh")
    {
        ui->PtpHcAutoRefreshButton->setEnabled(false);

        ui->PtpHcInstanceComboBox->setEnabled(false);
        ui->PtpHcReadValuesButton->setEnabled(false);
        ui->PtpHcWriteValuesButton->setEnabled(false);

        ptp_hc_offset_number_of_points = 0;
        ptp_hc_offset_series-> clear();

        ptp_hc_delay_number_of_points = 0;
        for (int i=0; i<3; i++)
        {
            ptp_hc_delay_series.at(i)->clear();
        }

        ptp_hc_timer->start(1000);

        ui->PtpHcAutoRefreshButton->setText("Stop Refresh");
        ui->PtpHcAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->PtpHcAutoRefreshButton->setEnabled(false);

        ptp_hc_timer->stop();

        ui->PtpHcInstanceComboBox->setEnabled(true);
        ui->PtpHcReadValuesButton->setEnabled(true);
        ui->PtpHcWriteValuesButton->setEnabled(true);

        ui->PtpHcAutoRefreshButton->setText("Start Refresh");
        ui->PtpHcAutoRefreshButton->setEnabled(true);
    }
}


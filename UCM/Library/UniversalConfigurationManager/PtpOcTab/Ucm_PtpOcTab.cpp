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

#include "Ucm_PtpOcTab.h"
#include "ui_Ucm_PtpOcTab.h"

Ucm_PtpOcTab::Ucm_PtpOcTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_PtpOcTab();
    ui->setupUi(this);

    ptp_oc_timer = new QTimer(this);
    ptp_oc_timer->stop();
    connect(ui->PtpOcReadValuesButton, SIGNAL(clicked()), this, SLOT(ptp_oc_read_values_button_clicked()));
    connect(ui->PtpOcWriteValuesButton, SIGNAL(clicked()), this, SLOT(ptp_oc_write_values_button_clicked()));
    connect(ui->PtpOcAutoRefreshButton, SIGNAL(clicked()), this, SLOT(ptp_oc_auto_refresh_button_clicked()));
    connect(ptp_oc_timer, SIGNAL(timeout()), this, SLOT(ptp_oc_read_values_button_clicked()));

    ptp_oc_offset_series = new QLineSeries();
    ptp_oc_offset_number_of_points = 0;

    ptp_oc_offset_chart = new QChart();
    ptp_oc_offset_chart->legend()->hide();
    ptp_oc_offset_chart->addSeries(ptp_oc_offset_series);

    QValueAxis* ptp_oc_offset_chart_x_axis = new QValueAxis;
    ptp_oc_offset_chart_x_axis->setLabelFormat("%i");
    ptp_oc_offset_chart_x_axis->setTickCount(10);
    ptp_oc_offset_chart_x_axis->setMin(0);
    ptp_oc_offset_chart_x_axis->setMax(19);
    ptp_oc_offset_chart->addAxis(ptp_oc_offset_chart_x_axis, Qt::AlignBottom);
    ptp_oc_offset_series->attachAxis(ptp_oc_offset_chart_x_axis);

    QValueAxis* ptp_oc_offset_chart_y_axis = new QValueAxis;
    ptp_oc_offset_chart_y_axis->setLabelFormat("%i");
    ptp_oc_offset_chart_y_axis->setTickCount(5);
    ptp_oc_offset_chart_y_axis->setMin(-500);
    ptp_oc_offset_chart_y_axis->setMax(500);
    ptp_oc_offset_chart->addAxis(ptp_oc_offset_chart_y_axis, Qt::AlignLeft);
    ptp_oc_offset_series->attachAxis(ptp_oc_offset_chart_y_axis);

    ptp_oc_offset_chart_view = new QChartView(ptp_oc_offset_chart);
    ptp_oc_offset_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->PtpOcOffsetChartLayout->addWidget(ptp_oc_offset_chart_view, 0, 0);


    ptp_oc_delay_series = new QLineSeries();
    ptp_oc_delay_number_of_points = 0;

    ptp_oc_delay_chart = new QChart();
    ptp_oc_delay_chart->legend()->hide();
    ptp_oc_delay_chart->addSeries(ptp_oc_delay_series);

    QValueAxis* ptp_oc_delay_chart_x_axis = new QValueAxis;
    ptp_oc_delay_chart_x_axis->setLabelFormat("%i");
    ptp_oc_delay_chart_x_axis->setTickCount(10);
    ptp_oc_delay_chart_x_axis->setMin(0);
    ptp_oc_delay_chart_x_axis->setMax(19);
    ptp_oc_delay_chart->addAxis(ptp_oc_delay_chart_x_axis, Qt::AlignBottom);
    ptp_oc_delay_series->attachAxis(ptp_oc_delay_chart_x_axis);

    QValueAxis* ptp_oc_delay_chart_y_axis = new QValueAxis;
    ptp_oc_delay_chart_y_axis->setLabelFormat("%i");
    ptp_oc_delay_chart_y_axis->setTickCount(5);
    ptp_oc_delay_chart_y_axis->setMin(0);
    ptp_oc_delay_chart_y_axis->setMax(500);
    ptp_oc_delay_chart->addAxis(ptp_oc_delay_chart_y_axis, Qt::AlignLeft);
    ptp_oc_delay_series->attachAxis(ptp_oc_delay_chart_y_axis);

    ptp_oc_delay_chart_view = new QChartView(ptp_oc_delay_chart);
    ptp_oc_delay_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->PtpOcDelayChartLayout->addWidget(ptp_oc_delay_chart_view, 0, 0);

}

Ucm_PtpOcTab::~Ucm_PtpOcTab()
{
    ptp_oc_timer->stop();
    delete ui;
    delete ptp_oc_timer;
    delete ptp_oc_offset_series;
    delete ptp_oc_offset_chart_view;
    delete ptp_oc_offset_chart;
    delete ptp_oc_delay_series;
    delete ptp_oc_delay_chart_view;
    delete ptp_oc_delay_chart;
}

void Ucm_PtpOcTab::ptp_oc_add_instance(unsigned int instance)
{
    ui->PtpOcInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_PtpOcTab::ptp_oc_enable(void)
{
    return 0;
}

int Ucm_PtpOcTab::ptp_oc_disable(void)
{
    ptp_oc_timer->stop();
    ui->PtpOcAutoRefreshButton->setText("Start Refresh");
    ui->PtpOcReadValuesButton->setEnabled(true);
    ui->PtpOcWriteValuesButton->setEnabled(true);
    ui->PtpOcInstanceComboBox->clear();

    ui->PtpOcVlanValue->setText("NA");
    ui->PtpOcProfileValue->setCurrentText("NA");
    ui->PtpOcLayerValue->setCurrentText("NA");
    ui->PtpOcIpValue->setText("NA");
    ui->PtpOcVersionValue->setText("NA");

    ui->PtpOcDefaultDsClockIdValue->setText("NA");
    ui->PtpOcDefaultDsDomainValue->setText("NA");
    ui->PtpOcDefaultDsPriority1Value->setText("NA");
    ui->PtpOcDefaultDsPriority2Value->setText("NA");
    ui->PtpOcDefaultDsAccuracyValue->setText("NA");
    ui->PtpOcDefaultDsClassValue->setText("NA");
    ui->PtpOcDefaultDsShortIdValue->setText("NA");
    ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
    ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");

    ui->PtpOcPortDsPeerDelayValue->setText("NA");
    ui->PtpOcPortDsStateValue->setText("NA");

    ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
    ui->PtpOcCurrentDsOffsetValue->setText("NA");

    ui->PtpOcParentDsParentClockIdValue->setText("NA");
    ui->PtpOcParentDsGmClockIdValue->setText("NA");
    ui->PtpOcParentDsGmPriority1Value->setText("NA");
    ui->PtpOcParentDsGmPriority2Value->setText("NA");
    ui->PtpOcParentDsGmAccuracyValue->setText("NA");
    ui->PtpOcParentDsGmClassValue->setText("NA");
    ui->PtpOcParentDsGmShortIdValue->setText("NA");
    ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
    ui->PtpOcParentDsNwInaccuracyValue->setText("NA");

    ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
    ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
    ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
    ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
    ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
    ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
    ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
    ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
    ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
    ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
    ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
    ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");

    ui->PtpOcVlanEnableCheckBox->setChecked(false);
    ui->PtpOcVersionValue->setText("NA");

    return 0;
}

void Ucm_PtpOcTab::ptp_oc_read_values(void)
{
    unsigned long long temp_next_jump;
    unsigned long long temp_delay;
    unsigned long long temp_offset;
    long long temp_signed_offset;
    unsigned int temp_length;
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->PtpOcInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PtpOrdinaryClockCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PtpOcVlanValue->setText("NA");
            ui->PtpOcProfileValue->setCurrentText("NA");
            ui->PtpOcLayerValue->setCurrentText("NA");
            ui->PtpOcIpValue->setText("NA");
            ui->PtpOcVersionValue->setText("NA");

            ui->PtpOcDefaultDsClockIdValue->setText("NA");
            ui->PtpOcDefaultDsDomainValue->setText("NA");
            ui->PtpOcDefaultDsPriority1Value->setText("NA");
            ui->PtpOcDefaultDsPriority2Value->setText("NA");
            ui->PtpOcDefaultDsAccuracyValue->setText("NA");
            ui->PtpOcDefaultDsClassValue->setText("NA");
            ui->PtpOcDefaultDsShortIdValue->setText("NA");
            ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
            ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");

            ui->PtpOcPortDsPeerDelayValue->setText("NA");
            ui->PtpOcPortDsStateValue->setText("NA");

            ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
            ui->PtpOcCurrentDsOffsetValue->setText("NA");

            ui->PtpOcParentDsParentClockIdValue->setText("NA");
            ui->PtpOcParentDsGmClockIdValue->setText("NA");
            ui->PtpOcParentDsGmPriority1Value->setText("NA");
            ui->PtpOcParentDsGmPriority2Value->setText("NA");
            ui->PtpOcParentDsGmAccuracyValue->setText("NA");
            ui->PtpOcParentDsGmClassValue->setText("NA");
            ui->PtpOcParentDsGmShortIdValue->setText("NA");
            ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
            ui->PtpOcParentDsNwInaccuracyValue->setText("NA");

            ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
            ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
            ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
            ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
            ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");

            ui->PtpOcVlanEnableCheckBox->setChecked(false);
            ui->PtpOcVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PtpOcEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PtpOcEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->PtpOcEnableCheckBox->setChecked(false);
    }


    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000088, temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ui->PtpOcVlanEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PtpOcVlanEnableCheckBox->setChecked(true);
        }

        temp_data &= 0x0000FFFF;

        ui->PtpOcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->PtpOcVlanEnableCheckBox->setChecked(false);
        ui->PtpOcVlanValue->setText("NA");
    }


    // profile and layer
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000084, temp_data))
    {
        switch (temp_data & 0x00000003)
        {
        case 0:
            ui->PtpOcProfileValue->setCurrentText("Default");
            break;
        case 1:
            ui->PtpOcProfileValue->setCurrentText("Power");
            break;
        case 2:
            ui->PtpOcProfileValue->setCurrentText("Utility");
            break;
        default:
            ui->PtpOcProfileValue->setCurrentText("NA");
            break;
        }

        switch ((temp_data >> 16) & 0x00000001)
        {
        case 0:
            ui->PtpOcLayerValue->setCurrentText("Layer 2");
            break;
        case 1:
            ui->PtpOcLayerValue->setCurrentText("Layer 3");
            break;
        default:
            ui->PtpOcLayerValue->setCurrentText("NA");
            break;
        }

    }
    else
    {
        ui->PtpOcProfileValue->setCurrentText("NA");
        ui->PtpOcLayerValue->setCurrentText("NA");
    }

    // ip
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000008C, temp_data))
    {
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(".");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(".");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(".");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        ui->PtpOcIpValue->setText(temp_string);

    }
    else
    {
        ui->PtpOcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + 0x00000100, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // clock id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000104, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000108, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            ui->PtpOcDefaultDsClockIdValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpOcDefaultDsClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcDefaultDsClockIdValue->setText("NA");
                    }

                    // domain
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000010C, temp_data))
                    {
                        ui->PtpOcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

                    }
                    else
                    {
                        ui->PtpOcDefaultDsDomainValue->setText("NA");
                    }

                    // priority 1 & 2, accuracy ,class
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000110, temp_data))
                    {
                        ui->PtpOcDefaultDsPriority2Value->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpOcDefaultDsPriority1Value->setText(QString("0x%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpOcDefaultDsAccuracyValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
                        ui->PtpOcDefaultDsClassValue->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpOcDefaultDsPriority1Value->setText("NA");
                        ui->PtpOcDefaultDsPriority2Value->setText("NA");
                        ui->PtpOcDefaultDsAccuracyValue->setText("NA");
                        ui->PtpOcDefaultDsClassValue->setText("NA");
                    }

                    // short id
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000114, temp_data))
                    {
                        ui->PtpOcDefaultDsShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpOcDefaultDsShortIdValue->setText("NA");
                    }

                    // inaccuracy
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000118, temp_data))
                    {
                        ui->PtpOcDefaultDsInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
                    }

                    // nr of ports
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000011C, temp_data))
                    {
                        ui->PtpOcDefaultDsNrOfPortsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpOcDefaultDsClockIdValue->setText("NA");
                    ui->PtpOcDefaultDsDomainValue->setText("NA");
                    ui->PtpOcDefaultDsPriority1Value->setText("NA");
                    ui->PtpOcDefaultDsPriority2Value->setText("NA");
                    ui->PtpOcDefaultDsAccuracyValue->setText("NA");
                    ui->PtpOcDefaultDsClassValue->setText("NA");
                    ui->PtpOcDefaultDsShortIdValue->setText("NA");
                    ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
                    ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");
                }

            }
            else
            {
                ui->PtpOcDefaultDsClockIdValue->setText("NA");
                ui->PtpOcDefaultDsDomainValue->setText("NA");
                ui->PtpOcDefaultDsPriority1Value->setText("NA");
                ui->PtpOcDefaultDsPriority2Value->setText("NA");
                ui->PtpOcDefaultDsAccuracyValue->setText("NA");
                ui->PtpOcDefaultDsClassValue->setText("NA");
                ui->PtpOcDefaultDsShortIdValue->setText("NA");
                ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
                ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpOcDefaultDsClockIdValue->setText("NA");
        ui->PtpOcDefaultDsDomainValue->setText("NA");
        ui->PtpOcDefaultDsPriority1Value->setText("NA");
        ui->PtpOcDefaultDsPriority2Value->setText("NA");
        ui->PtpOcDefaultDsAccuracyValue->setText("NA");
        ui->PtpOcDefaultDsClassValue->setText("NA");
        ui->PtpOcDefaultDsShortIdValue->setText("NA");
        ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
        ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");
    }

    //********************************
    // port dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000200, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + 0x00000200, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // peer delay
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000204, temp_data))
                    {
                        temp_delay = temp_data;
                        temp_delay = temp_delay << 32;
                        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000208, temp_data))
                        {
                            temp_delay |= temp_data;
                            temp_delay = temp_delay >> 16;
                            ui->PtpOcPortDsPeerDelayValue->setText(QString::number(temp_delay));

                            if (true == ptp_oc_timer->isActive())
                            {
                                ptp_oc_delay_series->append(ptp_oc_delay_number_of_points, temp_delay);

                                if (ptp_oc_delay_number_of_points < 20)
                                {
                                    ptp_oc_delay_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_oc_delay_series->count(); j++)
                                    {
                                        QPointF temp_point = ptp_oc_delay_series->at(j);
                                        ptp_oc_delay_series->replace(j, (j-1), temp_point.y());
                                    }
                                    ptp_oc_delay_series->remove(0);
                                }

                                ptp_oc_delay_chart->show();
                            }
                        }
                        else
                        {
                            ui->PtpOcPortDsPeerDelayValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcPortDsPeerDelayValue->setText("NA");
                    }

                    // state
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000020C, temp_data))
                    {
                        switch (temp_data)
                        {
                        case 0x0000001:
                            ui->PtpOcPortDsStateValue->setText("INITIALIZING");
                            break;
                        case 0x00000002:
                            ui->PtpOcPortDsStateValue->setText("FAULTY");
                            break;
                        case 0x00000003:
                            ui->PtpOcPortDsStateValue->setText("DISABLED");
                            break;
                        case 0x00000004:
                            ui->PtpOcPortDsStateValue->setText("LISTENING");
                            break;
                        case 0x00000005:
                            ui->PtpOcPortDsStateValue->setText("PREMASTER");
                            break;
                        case 0x00000006:
                            ui->PtpOcPortDsStateValue->setText("MASTER");
                            break;
                        case 0x00000007:
                            ui->PtpOcPortDsStateValue->setText("PASSIVE");
                            break;
                        case 0x00000008:
                            ui->PtpOcPortDsStateValue->setText("UNCALIBRATED");
                            break;
                        case 0x00000009:
                            ui->PtpOcPortDsStateValue->setText("SLAVE");
                            break;
                        default:
                            ui->PtpOcPortDsStateValue->setText("NA");
                            break;
                        }                    }
                    else
                    {
                        ui->PtpOcPortDsStateValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpOcPortDsPeerDelayValue->setText("NA");
                    ui->PtpOcPortDsStateValue->setText("NA");
                }

            }
            else
            {
                ui->PtpOcPortDsPeerDelayValue->setText("NA");
                ui->PtpOcPortDsStateValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpOcPortDsPeerDelayValue->setText("NA");
        ui->PtpOcPortDsStateValue->setText("NA");
    }

    //********************************
    // current dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000300, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + 0x00000300, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // steps removed
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000304, temp_data))
                    {
                        ui->PtpOcCurrentDsStepsRemovedValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
                    }

                    // offset
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000308, temp_data))
                    {
                        temp_offset = temp_data;
                        temp_offset = temp_offset << 32;
                        if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000030C, temp_data))
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

                            ui->PtpOcCurrentDsOffsetValue->setText(QString::number(temp_signed_offset));

                            if (true == ptp_oc_timer->isActive())
                            {
                                ptp_oc_offset_series->append(ptp_oc_offset_number_of_points, temp_signed_offset);

                                if (ptp_oc_offset_number_of_points < 20)
                                {
                                    ptp_oc_offset_number_of_points++;
                                }
                                else
                                {
                                    for (int j = 1; j < ptp_oc_offset_series->count(); j++)
                                    {
                                        QPointF temp_point = ptp_oc_offset_series->at(j);
                                        ptp_oc_offset_series->replace(j, (j-1), temp_point.y());
                                    }
                                    ptp_oc_offset_series->remove(0);
                                }

                                ptp_oc_offset_chart->show();
                            }
                        }
                        else
                        {
                            ui->PtpOcCurrentDsOffsetValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcCurrentDsOffsetValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
                    ui->PtpOcCurrentDsOffsetValue->setText("NA");
                }

            }
            else
            {
                ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
                ui->PtpOcCurrentDsOffsetValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
        ui->PtpOcCurrentDsOffsetValue->setText("NA");
    }

    //********************************
    // parent dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000400, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + 0x00000400, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // parent clock id and port id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000404, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000408, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(".");
                            if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000040C, temp_data))
                            {
                                temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x0000FFFF), 4, 16, QLatin1Char('0')));
                                ui->PtpOcParentDsParentClockIdValue->setText(temp_string);
                            }
                            else
                            {
                                ui->PtpOcParentDsParentClockIdValue->setText("NA");
                            }
                        }
                        else
                        {
                            ui->PtpOcParentDsParentClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcParentDsParentClockIdValue->setText("NA");
                    }

                    // gm clock id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000410, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000414, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            ui->PtpOcParentDsGmClockIdValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpOcParentDsGmClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcParentDsGmClockIdValue->setText("NA");
                    }

                    // gm priority 1 & 2, accuracy ,class
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000418, temp_data))
                    {
                        ui->PtpOcParentDsGmPriority2Value->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpOcParentDsGmPriority1Value->setText(QString("0x%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        ui->PtpOcParentDsGmAccuracyValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
                        ui->PtpOcParentDsGmClassValue->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpOcParentDsGmPriority1Value->setText("NA");
                        ui->PtpOcParentDsGmPriority2Value->setText("NA");
                        ui->PtpOcParentDsGmAccuracyValue->setText("NA");
                        ui->PtpOcParentDsGmClassValue->setText("NA");
                    }

                    // gm short id
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000041C, temp_data))
                    {
                        ui->PtpOcParentDsGmShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                    }
                    else
                    {
                        ui->PtpOcParentDsGmShortIdValue->setText("NA");
                    }

                    // gm inaccuracy
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000420, temp_data))
                    {
                        ui->PtpOcParentDsGmInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
                    }

                    // nw inaccuracy
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000424, temp_data))
                    {
                        ui->PtpOcParentDsNwInaccuracyValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcParentDsNwInaccuracyValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpOcParentDsParentClockIdValue->setText("NA");
                    ui->PtpOcParentDsGmClockIdValue->setText("NA");
                    ui->PtpOcParentDsGmPriority1Value->setText("NA");
                    ui->PtpOcParentDsGmPriority2Value->setText("NA");
                    ui->PtpOcParentDsGmAccuracyValue->setText("NA");
                    ui->PtpOcParentDsGmClassValue->setText("NA");
                    ui->PtpOcParentDsGmShortIdValue->setText("NA");
                    ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
                    ui->PtpOcParentDsNwInaccuracyValue->setText("NA");

                }

            }
            else
            {
                ui->PtpOcParentDsParentClockIdValue->setText("NA");
                ui->PtpOcParentDsGmClockIdValue->setText("NA");
                ui->PtpOcParentDsGmPriority1Value->setText("NA");
                ui->PtpOcParentDsGmPriority2Value->setText("NA");
                ui->PtpOcParentDsGmAccuracyValue->setText("NA");
                ui->PtpOcParentDsGmClassValue->setText("NA");
                ui->PtpOcParentDsGmShortIdValue->setText("NA");
                ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
                ui->PtpOcParentDsNwInaccuracyValue->setText("NA");

            }
        }
    }
    else
    {
        ui->PtpOcParentDsParentClockIdValue->setText("NA");
        ui->PtpOcParentDsGmClockIdValue->setText("NA");
        ui->PtpOcParentDsGmPriority1Value->setText("NA");
        ui->PtpOcParentDsGmPriority2Value->setText("NA");
        ui->PtpOcParentDsGmAccuracyValue->setText("NA");
        ui->PtpOcParentDsGmClassValue->setText("NA");
        ui->PtpOcParentDsGmShortIdValue->setText("NA");
        ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
        ui->PtpOcParentDsNwInaccuracyValue->setText("NA");

    }

    //********************************
    // time properties dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + 0x00000500, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {

                    // time source, ptp timescale, freq traceable, time traceable, lep61, leap 59, ut offset val, utc offset
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000504, temp_data))
                    {
                        ui->PtpOcTimePropertiesDsTimeSourceValue->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        if ((temp_data & 0x00000100) != 0)
                        {
                            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00000200) != 0)
                        {
                            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00000400) != 0)
                        {
                            ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00000800) != 0)
                        {
                            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00001000) != 0)
                        {
                            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
                        }
                        if ((temp_data & 0x00002000) != 0)
                        {
                            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(true);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                        }

                        ui->PtpOcTimePropertiesDsUtcOffsetValue->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
                        ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                        ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                        ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                        ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
                        ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
                        ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                        ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
                    }

                    // current offset
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000508, temp_data))
                    {
                        ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
                    }

                    // jump seconds
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000050C, temp_data))
                    {
                        ui->PtpOcTimePropertiesDsJumpSecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
                    }

                    // next jump
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000510, temp_data))
                    {
                        temp_next_jump = temp_data;
                        temp_next_jump = temp_next_jump << 32;
                        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000514, temp_data))
                        {
                            temp_next_jump |= temp_data;
                            ui->PtpOcTimePropertiesDsNextJumpValue->setText(QString::number(temp_next_jump));
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
                    }

                    // display name
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000518, temp_length))
                    {
                        for (int j=0; j<3; j++)
                        {
                            if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000051C + (j*4), temp_data))
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
                        ui->PtpOcTimePropertiesDsDisplayNameValue->setText(temp_string);
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                    }
                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
                    ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                    ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                    ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                    ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
                    ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
                    ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                    ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
                    ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
                    ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
                    ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
                    ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                }

            }
            else
            {
                ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
                ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
                ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
                ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
                ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
                ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
                ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
                ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
                ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
                ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
                ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
                ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
            }
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
        ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
        ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
        ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
        ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
        ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
        ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
        ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
        ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
        ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
        ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
    }


    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->PtpOcVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->PtpOcVersionValue->setText("NA");
    }
}

void Ucm_PtpOcTab::ptp_oc_write_values(void)
{
    unsigned long long temp_next_jump;
    unsigned long long temp_mac;
    unsigned long temp_ip;
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->PtpOcInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PtpOrdinaryClockCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PtpOcVlanValue->setText("NA");
            ui->PtpOcProfileValue->setCurrentText("NA");
            ui->PtpOcLayerValue->setCurrentText("NA");
            ui->PtpOcIpValue->setText("NA");
            ui->PtpOcVersionValue->setText("NA");

            ui->PtpOcDefaultDsClockIdValue->setText("NA");
            ui->PtpOcDefaultDsDomainValue->setText("NA");
            ui->PtpOcDefaultDsPriority1Value->setText("NA");
            ui->PtpOcDefaultDsPriority2Value->setText("NA");
            ui->PtpOcDefaultDsAccuracyValue->setText("NA");
            ui->PtpOcDefaultDsClassValue->setText("NA");
            ui->PtpOcDefaultDsShortIdValue->setText("NA");
            ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
            ui->PtpOcDefaultDsNrOfPortsValue->setText("NA");

            ui->PtpOcPortDsPeerDelayValue->setText("NA");
            ui->PtpOcPortDsStateValue->setText("NA");

            ui->PtpOcCurrentDsStepsRemovedValue->setText("NA");
            ui->PtpOcCurrentDsOffsetValue->setText("NA");

            ui->PtpOcParentDsParentClockIdValue->setText("NA");
            ui->PtpOcParentDsGmClockIdValue->setText("NA");
            ui->PtpOcParentDsGmPriority1Value->setText("NA");
            ui->PtpOcParentDsGmPriority2Value->setText("NA");
            ui->PtpOcParentDsGmAccuracyValue->setText("NA");
            ui->PtpOcParentDsGmClassValue->setText("NA");
            ui->PtpOcParentDsGmShortIdValue->setText("NA");
            ui->PtpOcParentDsGmInaccuracyValue->setText("NA");
            ui->PtpOcParentDsNwInaccuracyValue->setText("NA");

            ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsTimeTraceableCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
            ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
            ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
            ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
            ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
            ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");

            ui->PtpOcVlanEnableCheckBox->setChecked(false);
            ui->PtpOcVersionValue->setText("NA");
            return;
        }
    }

    // vlan
    temp_string = ui->PtpOcVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if(true == ui->PtpOcVlanEnableCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // enable
    }
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000088, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->PtpOcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000080, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcVlanEnableCheckBox->setChecked(false);
            ui->PtpOcVlanValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcVlanEnableCheckBox->setChecked(false);
        ui->PtpOcVlanValue->setText("NA");
    }


    // profile and layer
    temp_string = ui->PtpOcProfileValue->currentText();
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
        temp_string = ui->PtpOcLayerValue->currentText();
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
        else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000084, temp_data))
        {
            switch ((temp_data >> 16) & 0x00000001)
            {
            case 0:
                ui->PtpOcLayerValue->setCurrentText("Layer 2");
                break;
            case 1:
                ui->PtpOcLayerValue->setCurrentText("Layer 3");
                break;
            default:
                ui->PtpOcLayerValue->setCurrentText("NA");
                break;
            }

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000080, temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcProfileValue->setCurrentText("NA");
            }
        }
        else
        {
            ui->PtpOcProfileValue->setCurrentText("NA");
        }
    }


    // ip
    temp_string = ui->PtpOcIpValue->text();
    temp_string.remove(QChar('.'), Qt::CaseInsensitive);
    temp_ip = temp_string.toULong(nullptr, 16);
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
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x0000008C, temp_data))
    {
        temp_string.clear();
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(".");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(".");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(".");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

        temp_data = 0x00000004; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000080, temp_data))
        {
            ui->PtpOcIpValue->setText(temp_string);
        }
        else
        {
            ui->PtpOcIpValue->setText("NA");
        }

    }
    else
    {
        ui->PtpOcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    // clock id
    temp_string = ui->PtpOcDefaultDsClockIdValue->text();
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
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000104, temp_data))
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
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000108, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
            {
                ui->PtpOcDefaultDsClockIdValue->setText(temp_string);
            }
            else
            {
                ui->PtpOcDefaultDsClockIdValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsClockIdValue->setText("NA");
        }

    }
    else
    {
        ui->PtpOcDefaultDsClockIdValue->setText("NA");
    }

    // domain
    temp_string = ui->PtpOcDefaultDsDomainValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x0000010C, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->PtpOcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcDefaultDsDomainValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcDefaultDsDomainValue->setText("NA");
    }


    // priority 1
    temp_string = ui->PtpOcDefaultDsPriority1Value->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data = temp_data << 8;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000110, temp_data))
    {
        ui->PtpOcDefaultDsPriority1Value->setText(QString("0x%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));

        temp_data = 0x00000008; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcDefaultDsPriority1Value->setText("NA");
        }
    }
    else
    {
        ui->PtpOcDefaultDsPriority1Value->setText("NA");
    }

    // priority 2
    temp_string = ui->PtpOcDefaultDsPriority2Value->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000110, temp_data))
    {
        ui->PtpOcDefaultDsPriority2Value->setText(QString("0x%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));

        temp_data = 0x00000010; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcDefaultDsPriority2Value->setText("NA");
        }
    }
    else
    {
        ui->PtpOcDefaultDsPriority2Value->setText("NA");
    }

    // accuracy, class
    temp_string = ui->PtpOcDefaultDsClassValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data = temp_data << 8;
    if (temp_string == "NA")
    {
        //nothing
    }
    else
    {
        temp_string = ui->PtpOcDefaultDsAccuracyValue->text();
        temp_data |= temp_string.toUInt(nullptr, 10);
        temp_data = temp_data << 16;
        if (temp_string == "NA")
        {
            //nothing
        }
        else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000110, temp_data))
        {
            ui->PtpOcDefaultDsAccuracyValue->setText(QString::number(((temp_data >> 16) & 0x000000FF)));
            ui->PtpOcDefaultDsClassValue->setText(QString("0x%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000004; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcDefaultDsAccuracyValue->setText("NA");
                ui->PtpOcDefaultDsClassValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcDefaultDsAccuracyValue->setText("NA");
            ui->PtpOcDefaultDsClassValue->setText("NA");
        }
    }


    // short id
    temp_string = ui->PtpOcDefaultDsShortIdValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000114, temp_data))
    {
        ui->PtpOcDefaultDsShortIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000020; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcDefaultDsShortIdValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcDefaultDsShortIdValue->setText("NA");
    }


    // inaccuracy
    temp_string = ui->PtpOcDefaultDsInaccuracyValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000114, temp_data))
    {
        ui->PtpOcDefaultDsInaccuracyValue->setText(QString::number(temp_data));

        temp_data = 0x00000040; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000100, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcDefaultDsInaccuracyValue->setText("NA");
    }


    //********************************
    // port dataset
    //********************************
    // all RO

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
    temp_string = ui->PtpOcTimePropertiesDsTimeSourceValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->PtpOcTimePropertiesDsTimeSourceValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

        temp_data = 0x00000080; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsTimeSourceValue->setText("NA");
    }

    // ptp timescale
    if(true == ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->isChecked())
    {
        temp_data = 0x00000100; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        if ((temp_data & 0x00000100) != 0)
        {
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        }

        temp_data = 0x00000040; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
    }

    // freq traceable
    if(true == ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->isChecked())
    {
        temp_data = 0x00000200; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        if ((temp_data & 0x00000200) != 0)
        {
            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
        }

        temp_data = 0x00000020; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsFreqTraceableCheckBox->setChecked(false);
    }

    // time traceable
    if(true == ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->isChecked())
    {
        temp_data = 0x00000400; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        if ((temp_data & 0x00000400) != 0)
        {
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        }

        temp_data = 0x00000010; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsPtpTimescaleCheckBox->setChecked(false);
    }

    // leap 61
    if(true == ui->PtpOcTimePropertiesDsLeap61CheckBox->isChecked())
    {
        temp_data = 0x00000800; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        if ((temp_data & 0x00000800) != 0)
        {
            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(true);
        }
        else
        {
            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
        }

        temp_data = 0x00000008; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsLeap61CheckBox->setChecked(false);
    }

    // leap 59
    if(true == ui->PtpOcTimePropertiesDsLeap59CheckBox->isChecked())
    {
        temp_data = 0x00001000; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        if ((temp_data & 0x00001000) != 0)
        {
            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(true);
        }
        else
        {
            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
        }

        temp_data = 0x00000004; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsLeap59CheckBox->setChecked(false);
    }

    // utc offset val
    if(true == ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->isChecked())
    {
        temp_data = 0x00002000; // set
    }
    else
    {
        temp_data = 0x00000000; // not set
    }

    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        if ((temp_data & 0x00002000) != 0)
        {
            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(true);
        }
        else
        {
            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
        }

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsUtcOffsetValCheckBox->setChecked(false);
    }


    // utc offset
    temp_string = ui->PtpOcTimePropertiesDsUtcOffsetValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    temp_data = temp_data << 16;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000504, temp_data))
    {
        ui->PtpOcTimePropertiesDsUtcOffsetValue->setText(QString::number((temp_data >> 16) & 0x0000FFFF));

        temp_data = 0x00000001; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsUtcOffsetValue->setText("NA");
    }


    // current offset
    temp_string = ui->PtpOcTimePropertiesDsCurrentOffsetValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000508, temp_data))
    {
        ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText(QString::number(temp_data));
        temp_data = 0x00000100; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsCurrentOffsetValue->setText("NA");
    }

    // jump seconds
    temp_string = ui->PtpOcTimePropertiesDsJumpSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x0000050C, temp_data))
    {
        ui->PtpOcTimePropertiesDsJumpSecondsValue->setText(QString::number(temp_data));
        temp_data = 0x00000200; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsJumpSecondsValue->setText("NA");
    }

    // next jump
    temp_string = ui->PtpOcTimePropertiesDsNextJumpValue->text();
    temp_next_jump = temp_string.toULongLong(nullptr, 10);
    temp_data = temp_next_jump >> 32;
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000510, temp_data))
    {
        temp_data = temp_next_jump & 0x00000000FFFFFFFF;
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000514, temp_data))
        {
            ui->PtpOcTimePropertiesDsNextJumpValue->setText(QString::number(temp_next_jump));
            temp_data = 0x00000400; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
            {
                // nothing
            }
            else
            {
                ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
            }
        }
        else
        {
            ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
        }
    }
    else
    {
        ui->PtpOcTimePropertiesDsNextJumpValue->setText("NA");
    }



    // display name
    temp_string = ui->PtpOcTimePropertiesDsDisplayNameValue->text();
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

            if (0 == ucm->com_lib.write_reg(temp_addr + 0x0000051C + (j*4), temp_data))
            {
                if (j == 2)
                {
                    temp_data = temp_string.size(); // length
                    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000518, temp_data))
                    {
                        temp_data = 0x00001800; // write
                        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000500, temp_data))
                        {
                            ui->PtpOcTimePropertiesDsDisplayNameValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                    }
                }
            }
            else
            {
                ui->PtpOcTimePropertiesDsDisplayNameValue->setText("NA");
                break;
            }
        }
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->PtpOcEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->PtpOcEnableCheckBox->setChecked(false);
    }
}

void Ucm_PtpOcTab::ptp_oc_read_values_button_clicked(void)
{
    ptp_oc_read_values();
}

void Ucm_PtpOcTab::ptp_oc_write_values_button_clicked(void)
{
    ptp_oc_write_values();
    ptp_oc_read_values();
}

void Ucm_PtpOcTab::ptp_oc_auto_refresh_button_clicked(void)
{
    if (ui->PtpOcAutoRefreshButton->text() == "Start Refresh")
    {
        ui->PtpOcAutoRefreshButton->setEnabled(false);

        ui->PtpOcReadValuesButton->setEnabled(false);
        ui->PtpOcWriteValuesButton->setEnabled(false);

        ptp_oc_offset_number_of_points = 0;
        ptp_oc_offset_series-> clear();

        ptp_oc_delay_number_of_points = 0;
        ptp_oc_delay_series->clear();

        ptp_oc_timer->start(1000);

        ui->PtpOcAutoRefreshButton->setText("Stop Refresh");
        ui->PtpOcAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->PtpOcAutoRefreshButton->setEnabled(false);

        ptp_oc_timer->stop();

        ui->PtpOcReadValuesButton->setEnabled(true);
        ui->PtpOcWriteValuesButton->setEnabled(true);

        ui->PtpOcAutoRefreshButton->setText("Start Refresh");
        ui->PtpOcAutoRefreshButton->setEnabled(true);
    }
}


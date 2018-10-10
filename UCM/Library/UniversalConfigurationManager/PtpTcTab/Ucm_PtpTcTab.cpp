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

#include <Ucm_PtpTcTab.h>
#include <ui_Ucm_PtpTcTab.h>
#include <QHostAddress>

Ucm_PtpTcTab::Ucm_PtpTcTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_PtpTcTab();
    ui->setupUi(this);

    ptp_tc_timer = new QTimer(this);
    ptp_tc_timer->stop();
    connect(ui->PtpTcReadValuesButton, SIGNAL(clicked()), this, SLOT(ptp_tc_read_values_button_clicked()));
    connect(ui->PtpTcWriteValuesButton, SIGNAL(clicked()), this, SLOT(ptp_tc_write_values_button_clicked()));
    connect(ui->PtpTcAutoRefreshButton, SIGNAL(clicked()), this, SLOT(ptp_tc_auto_refresh_button_clicked()));
    connect(ptp_tc_timer, SIGNAL(timeout()), this, SLOT(ptp_tc_read_values_button_clicked()));

    // max 4 ports
    for (int i=0; i<4; i++)
    {
        QLineSeries* temp_line_series = new QLineSeries();
        ptp_tc_delay_series.append(temp_line_series);
    }
    ptp_tc_delay_number_of_points = 0;

    ptp_tc_delay_chart = new QChart();
    ptp_tc_delay_chart->setContentsMargins(0, 0, 0, 0);
    ptp_tc_delay_chart->setBackgroundRoundness(0);
    ptp_tc_delay_chart->setBackgroundBrush(Qt::white);
    ptp_tc_delay_chart->legend()->hide();
    for (int i=0; i<4; i++)
    {
        ptp_tc_delay_chart->addSeries(ptp_tc_delay_series.at(i));
    }

    QValueAxis* ptp_tc_delay_chart_x_axis = new QValueAxis;
    ptp_tc_delay_chart_x_axis->setLabelFormat("%i");
    ptp_tc_delay_chart_x_axis->setTickCount(10);
    ptp_tc_delay_chart_x_axis->setMin(0);
    ptp_tc_delay_chart_x_axis->setMax(19);
    ptp_tc_delay_chart->addAxis(ptp_tc_delay_chart_x_axis, Qt::AlignBottom);
    for (int i=0; i<4; i++)
    {
        ptp_tc_delay_series.at(i)->attachAxis(ptp_tc_delay_chart_x_axis);
    }

    QValueAxis* ptp_tc_delay_chart_y_axis = new QValueAxis;
    ptp_tc_delay_chart_y_axis->setLabelFormat("%i");
    ptp_tc_delay_chart_y_axis->setTickCount(5);
    ptp_tc_delay_chart_y_axis->setMin(0);
    ptp_tc_delay_chart_y_axis->setMax(500);
    ptp_tc_delay_chart->addAxis(ptp_tc_delay_chart_y_axis, Qt::AlignLeft);
    for (int i=0; i<4; i++)
    {
        ptp_tc_delay_series.at(i)->attachAxis(ptp_tc_delay_chart_y_axis);
    }
    ptp_tc_delay_chart_view = new QChartView(ptp_tc_delay_chart);
    ptp_tc_delay_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->PtpTcDelayChartLayout->addWidget(ptp_tc_delay_chart_view, 0, 0);

}

Ucm_PtpTcTab::~Ucm_PtpTcTab()
{
    ptp_tc_timer->stop();
    delete ui;
    delete ptp_tc_timer;
    for (int i=0; i<4; i++)
    {
        delete ptp_tc_delay_series.at(i);
    }
    delete ptp_tc_delay_chart_view;
    delete ptp_tc_delay_chart;
}

int Ucm_PtpTcTab::ptp_tc_resize(int height, int width)
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

    ui->PtpTcDelayChartValue->setFixedHeight(210);
    ui->PtpTcDelayChartValue->setFixedWidth(450+(width_delta/2));
    ui->PtpTcDelayChartLabel->setFixedWidth(450+(width_delta/2));

    updateGeometry();

    return 0;
}

void Ucm_PtpTcTab::ptp_tc_add_instance(unsigned int instance)
{
    ui->PtpTcInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_PtpTcTab::ptp_tc_enable(void)
{
    return 0;
}

int Ucm_PtpTcTab::ptp_tc_disable(void)
{
    ptp_tc_timer->stop();
    ui->PtpTcAutoRefreshButton->setText("Start Refresh");
    ui->PtpTcInstanceComboBox->setEnabled(true);
    ui->PtpTcReadValuesButton->setEnabled(true);
    ui->PtpTcWriteValuesButton->setEnabled(true);
    ui->PtpTcInstanceComboBox->clear();
    
    ui->PtpTcVlanValue->setText("NA");
    ui->PtpTcProfileValue->setCurrentText("NA");
    ui->PtpTcLayerValue->setCurrentText("NA");
    ui->PtpTcDelayMechanismValue->setCurrentText("NA");
    ui->PtpTcIpValue->setText("NA");

    ui->PtpTcDefaultDsClockIdValue->setText("NA");
    ui->PtpTcDefaultDsDomainValue->setText("NA");
    ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
    ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);

    ui->PtpTcPortDsPortNrComboBox->clear();
    ui->PtpTcPortDsPeerDelayValue->setText("NA");
    ui->PtpTcPortDsAsymmetryValue->setText("NA");
    ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");

    ui->PtpTcVlanEnableCheckBox->setChecked(false);
    ui->PtpTcVersionValue->setText("NA");

    ptp_tc_delay_number_of_points = 0;
    for (int i=0; i<4; i++)
    {
        ptp_tc_delay_series.at(i)->clear();
    }

    return 0;
}

void Ucm_PtpTcTab::ptp_tc_read_values(void)
{
    unsigned long long temp_delay;
    unsigned long temp_ip = 0;
    unsigned int temp_max = 0;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;
    unsigned int nr_of_ports = 0;

    temp_string = ui->PtpTcInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PtpTransparentClockCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PtpTcVlanValue->setText("NA");
            ui->PtpTcProfileValue->setCurrentText("NA");
            ui->PtpTcLayerValue->setCurrentText("NA");
            ui->PtpTcDelayMechanismValue->setCurrentText("NA");
            ui->PtpTcIpValue->setText("NA");

            ui->PtpTcDefaultDsClockIdValue->setText("NA");
            ui->PtpTcDefaultDsDomainValue->setText("NA");
            ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
            ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);

            ui->PtpTcPortDsPortNrComboBox->clear();
            ui->PtpTcPortDsPeerDelayValue->setText("NA");
            ui->PtpTcPortDsAsymmetryValue->setText("NA");
            ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");

            ui->PtpTcVlanEnableCheckBox->setChecked(false);
            ui->PtpTcVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PtpTcEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PtpTcEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->PtpTcEnableCheckBox->setChecked(false);
    }


    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_ConfigVlanReg, temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ui->PtpTcVlanEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PtpTcVlanEnableCheckBox->setChecked(true);
        }

        temp_data &= 0x0000FFFF;

        ui->PtpTcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->PtpTcVlanEnableCheckBox->setChecked(false);
        ui->PtpTcVlanValue->setText("NA");
    }


    // profile and layer
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_ConfigProfileReg, temp_data))
    {
        switch (temp_data & 0x00000003)
        {
        case 0:
            ui->PtpTcProfileValue->setCurrentText("Default");
            break;
        case 1:
            ui->PtpTcProfileValue->setCurrentText("Power");
            break;
        case 2:
            ui->PtpTcProfileValue->setCurrentText("Utility");
            break;
        case 3:
            ui->PtpTcProfileValue->setCurrentText("TSN");
            break;
        default:
            ui->PtpTcProfileValue->setCurrentText("NA");
            break;
        }

        switch ((temp_data >> 8) & 0x00000001)
        {
        case 0:
            ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
            break;
        case 1:
            ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(true);
            break;
        default:
            ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
            break;
        }

        switch ((temp_data >> 16) & 0x00000001)
        {
        case 0:
            ui->PtpTcLayerValue->setCurrentText("Layer 2");
            break;
        case 1:
            ui->PtpTcLayerValue->setCurrentText("Layer 3");
            break;
        default:
            ui->PtpTcLayerValue->setCurrentText("NA");
            break;
        }

        switch ((temp_data >> 24) & 0x00000001)
        {
        case 0:
            ui->PtpTcDelayMechanismValue->setCurrentText("P2P");
            break;
        case 1:
            ui->PtpTcDelayMechanismValue->setCurrentText("E2E");
            break;
        default:
            ui->PtpTcDelayMechanismValue->setCurrentText("NA");
            break;
        }
    }
    else
    {
        ui->PtpTcProfileValue->setCurrentText("NA");
        ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
        ui->PtpTcLayerValue->setCurrentText("NA");
        ui->PtpTcDelayMechanismValue->setCurrentText("NA");
    }

    // ip
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_ConfigIpReg, temp_data))
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

        ui->PtpTcIpValue->setText(temp_string);

    }
    else
    {
        ui->PtpTcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    temp_data = 0x40000000;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDsControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_DefaultDsControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // clock id
                    temp_string.clear();
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_DefaultDs1Reg, temp_data))
                    {
                        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                        temp_string.append(":");
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_DefaultDs2Reg, temp_data))
                        {
                            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            temp_string.append(":");
                            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
                            ui->PtpTcDefaultDsClockIdValue->setText(temp_string);
                        }
                        else
                        {
                            ui->PtpTcDefaultDsClockIdValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpTcDefaultDsClockIdValue->setText("NA");
                    }

                    // domain
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_DefaultDs3Reg, temp_data))
                    {
                        ui->PtpTcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

                    }
                    else
                    {
                        ui->PtpTcDefaultDsDomainValue->setText("NA");
                    }

                    // nr of ports
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_DefaultDs4Reg, temp_data))
                    {
                        ui->PtpTcDefaultDsNrOfPortsValue->setText(QString::number(temp_data));
                        nr_of_ports = temp_data;
                        if (0 == ui->PtpTcPortDsPortNrComboBox->count())
                        {
                            ui->PtpTcPortDsPortNrComboBox->clear();
                            for (unsigned int i = 0; i < temp_data; i++)
                            {
                                ui->PtpTcPortDsPortNrComboBox->addItem(QString::number(i));
                            }
                        }
                    }
                    else
                    {
                        ui->PtpTcPortDsPortNrComboBox->clear();
                        ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->PtpTcDefaultDsClockIdValue->setText("NA");
                    ui->PtpTcDefaultDsDomainValue->setText("NA");
                    ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
                    ui->PtpTcPortDsPortNrComboBox->clear();
                }

            }
            else
            {
                ui->PtpTcDefaultDsClockIdValue->setText("NA");
                ui->PtpTcDefaultDsDomainValue->setText("NA");
                ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
                ui->PtpTcPortDsPortNrComboBox->clear();
            }
        }
    }
    else
    {
        ui->PtpTcDefaultDsClockIdValue->setText("NA");
        ui->PtpTcDefaultDsDomainValue->setText("NA");
        ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
        ui->PtpTcPortDsPortNrComboBox->clear();
    }

    //********************************
    // port dataset
    //********************************
    temp_string = ui->PtpTcPortDsPortNrComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    temp_data = ((temp_data & 0x000000FF) << 16);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
    {
        temp_data |= 0x40000000;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
        {
            for (int i = 0; i < 10; i++)
            {
                if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
                {
                    if ((temp_data & 0x80000000) != 0)
                    {

                        // peer delay
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDs1Reg, temp_data))
                        {
                            temp_delay = temp_data;
                            temp_delay = temp_delay << 32;
                            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDs2Reg, temp_data))
                            {
                                temp_delay |= temp_data;
                                temp_delay = temp_delay >> 16;
                                ui->PtpTcPortDsPeerDelayValue->setText(QString::number(temp_delay));
                            }
                            else
                            {
                                ui->PtpTcPortDsPeerDelayValue->setText("NA");
                            }
                        }
                        else
                        {
                            ui->PtpTcPortDsPeerDelayValue->setText("NA");
                        }

                        // pdelay log msg interval
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDs3Reg, temp_data))
                        {
                            ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText(QString::number((signed char)(temp_data & 0x000000FF)));

                        }
                        else
                        {
                            ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        }

                        // asymmetry
                        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDs4Reg, temp_data))
                        {
                            ui->PtpTcPortDsAsymmetryValue->setText(QString::number((signed int)temp_data));

                        }
                        else
                        {
                            ui->PtpTcPortDsAsymmetryValue->setText("NA");
                        }

                        break;
                    }
                    else if (i == 9)
                    {
                        cout << "ERROR: " << "read did not complete" << endl;
                        ui->PtpTcPortDsPeerDelayValue->setText("NA");
                        ui->PtpTcPortDsAsymmetryValue->setText("NA");
                        ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    }

                }
                else
                {
                    ui->PtpTcPortDsPeerDelayValue->setText("NA");
                    ui->PtpTcPortDsAsymmetryValue->setText("NA");
                    ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                }
            }
        }
        else
        {
            ui->PtpTcPortDsPeerDelayValue->setText("NA");
            ui->PtpTcPortDsAsymmetryValue->setText("NA");
            ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
        }

    }
    else
    {
        ui->PtpTcPortDsPeerDelayValue->setText("NA");
        ui->PtpTcPortDsAsymmetryValue->setText("NA");
        ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
    }

    // Chart
    if (nr_of_ports > 4) // limit to the first 4
    {
        nr_of_ports = 4;
    }

    temp_max = 0;
    for (unsigned int k= 0; k < nr_of_ports; k++)
    {
        temp_data = ((k & 0x000000FF) << 16);
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
        {
            temp_data |= 0x40000000;
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
            {
                for (int i = 0; i < 10; i++)
                {
                    if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
                    {
                        if ((temp_data & 0x80000000) != 0)
                        {

                            // peer delay
                            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDs1Reg, temp_data))
                            {
                                temp_delay = temp_data;
                                temp_delay = temp_delay << 32;
                                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_PortDs2Reg, temp_data))
                                {
                                    temp_delay |= temp_data;
                                    temp_delay = temp_delay >> 16;

                                    if (true == ptp_tc_timer->isActive())
                                    {
                                        ptp_tc_delay_series.at(k)->append(ptp_tc_delay_number_of_points, temp_delay);

                                        if (ptp_tc_delay_number_of_points < 20)
                                        {
                                            if (k == (nr_of_ports-1))
                                            {
                                                ptp_tc_delay_number_of_points++;
                                            }
                                        }
                                        else
                                        {
                                            for (int j = 1; j < ptp_tc_delay_series.at(k)->count(); j++)
                                            {
                                                QPointF temp_point = ptp_tc_delay_series.at(k)->at(j);
                                                ptp_tc_delay_series.at(k)->replace(j, (j-1), temp_point.y());
                                            }
                                            ptp_tc_delay_series.at(k)->remove(0);
                                        }

                                        for (int j = 0; j < ptp_tc_delay_series.at(k)->count(); j++)
                                        {
                                            QPointF temp_point = ptp_tc_delay_series.at(k)->at(j);
                                            if (temp_max < temp_point.y())
                                            {
                                                temp_max = temp_point.y();
                                            }
                                        }

                                        if (k == (nr_of_ports-1))
                                        {
                                            temp_max = (temp_max * 5) / 4;
                                            temp_max = temp_max + (100 - temp_max%100);
                                            ptp_tc_delay_chart->axisY()->setMax(temp_max);

                                            ptp_tc_delay_chart->show();
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

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PtpTc_VersionReg, temp_data))
    {
        ui->PtpTcVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->PtpTcVersionValue->setText("NA");
    }
}

void Ucm_PtpTcTab::ptp_tc_write_values(void)
{
    unsigned long long temp_mac;
    unsigned long temp_ip;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->PtpTcInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PtpTransparentClockCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PtpTcVlanValue->setText("NA");
            ui->PtpTcProfileValue->setCurrentText("NA");
            ui->PtpTcLayerValue->setCurrentText("NA");
            ui->PtpTcDelayMechanismValue->setCurrentText("NA");
            ui->PtpTcIpValue->setText("NA");

            ui->PtpTcDefaultDsClockIdValue->setText("NA");
            ui->PtpTcDefaultDsDomainValue->setText("NA");
            ui->PtpTcDefaultDsNrOfPortsValue->setText("NA");
            ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);

            ui->PtpTcPortDsPortNrComboBox->clear();
            ui->PtpTcPortDsPeerDelayValue->setText("NA");
            ui->PtpTcPortDsAsymmetryValue->setText("NA");
            ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");

            ui->PtpTcVlanEnableCheckBox->setChecked(false);
            ui->PtpTcVersionValue->setText("NA");
            return;
        }
    }

    // profile and layer
    temp_string = ui->PtpTcProfileValue->currentText();
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
        if (true == ui->PtpTcDefaultDsTwoStepCheckBox->isChecked())
        {
            temp_data |= 0x00000100;
        }

        temp_string = ui->PtpTcLayerValue->currentText();
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
            temp_string = ui->PtpTcDelayMechanismValue->currentText();
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
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ConfigProfileReg, temp_data))
            {
                switch (temp_data & 0x00000003)
                {
                case 0:
                    ui->PtpTcProfileValue->setCurrentText("Default");
                    break;
                case 1:
                    ui->PtpTcProfileValue->setCurrentText("Power");
                    break;
                case 2:
                    ui->PtpTcProfileValue->setCurrentText("Utility");
                    break;
                case 3:
                    ui->PtpTcProfileValue->setCurrentText("TSN");
                    break;
                default:
                    ui->PtpTcProfileValue->setCurrentText("NA");
                    break;
                }

                switch ((temp_data >> 8) & 0x00000001)
                {
                case 0:
                    ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
                    break;
                case 1:
                    ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(true);
                    break;
                default:
                    ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
                    break;
                }

                switch ((temp_data >> 16) & 0x00000001)
                {
                case 0:
                    ui->PtpTcLayerValue->setCurrentText("Layer 2");
                    break;
                case 1:
                    ui->PtpTcLayerValue->setCurrentText("Layer 3");
                    break;
                default:
                    ui->PtpTcLayerValue->setCurrentText("NA");
                    break;
                }

                switch ((temp_data >> 24) & 0x00000001)
                {
                case 0:
                    ui->PtpTcDelayMechanismValue->setCurrentText("P2P");
                    break;
                case 1:
                    ui->PtpTcDelayMechanismValue->setCurrentText("E2E");
                    break;
                default:
                    ui->PtpTcDelayMechanismValue->setCurrentText("NA");
                    break;
                }

                temp_data = 0x00000001; // write
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ConfigControlReg, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->PtpTcProfileValue->setCurrentText("NA");
                    ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
                    ui->PtpTcLayerValue->setCurrentText("NA");
                    ui->PtpTcDelayMechanismValue->setCurrentText("NA");
                }
            }
            else
            {
                ui->PtpTcProfileValue->setCurrentText("NA");
                ui->PtpTcDefaultDsTwoStepCheckBox->setChecked(false);
                ui->PtpTcLayerValue->setCurrentText("NA");
                ui->PtpTcDelayMechanismValue->setCurrentText("NA");
            }
        }
    }


    // vlan
    temp_string = ui->PtpTcVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if(true == ui->PtpTcVlanEnableCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // enable
    }
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ConfigVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->PtpTcVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpTcVlanEnableCheckBox->setChecked(false);
            ui->PtpTcVlanValue->setText("NA");
        }
    }
    else
    {
        ui->PtpTcVlanEnableCheckBox->setChecked(false);
        ui->PtpTcVlanValue->setText("NA");
    }


    // ip
    temp_string = ui->PtpTcIpValue->text();
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
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ConfigIpReg, temp_data))
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
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ConfigControlReg, temp_data))
        {
            ui->PtpTcIpValue->setText(temp_string);
        }
        else
        {
            ui->PtpTcIpValue->setText("NA");
        }

    }
    else
    {
        ui->PtpTcIpValue->setText("NA");
    }

    //********************************
    // default dataset
    //********************************
    // clock id
    temp_string = ui->PtpTcDefaultDsClockIdValue->text();
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
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDs1Reg, temp_data))
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
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDs2Reg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDsControlReg, temp_data))
            {
                ui->PtpTcDefaultDsClockIdValue->setText(temp_string);
            }
            else
            {
                ui->PtpTcDefaultDsClockIdValue->setText("NA");
            }
        }
        else
        {
            ui->PtpTcDefaultDsClockIdValue->setText("NA");
        }

    }
    else
    {
        ui->PtpTcDefaultDsClockIdValue->setText("NA");
    }

    // domain
    temp_string = ui->PtpTcDefaultDsDomainValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDs3Reg, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->PtpTcDefaultDsDomainValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDsControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->PtpTcDefaultDsDomainValue->setText("NA");
        }
    }
    else
    {
        ui->PtpTcDefaultDsDomainValue->setText("NA");
    }


    //********************************
    // port dataset
    //********************************
    // port id
    for (unsigned int i = 0; i < ui->PtpTcDefaultDsNrOfPortsValue->text().toUInt(nullptr, 10); i++)
    {
        temp_data = ((i & 0x000000FF) << 16);
        temp_data |= 0x00000001; // set the delay mechanism
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
        {
            temp_data = 0x00000001; // write for each port
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_DefaultDsControlReg, temp_data))
            {
               // ok
            }
            else
            {
                ui->PtpTcDefaultDsClockIdValue->setText("NA");
            }

            if (true == ui->PtpTcPortDsSetCustomIntervalsCheckBox->isChecked())
            {
                // only for the selected port
                if (i == ui->PtpTcPortDsPortNrComboBox->currentText().toUInt(nullptr, 10))
                {
                    temp_string = ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->text();
                    temp_data = (temp_string.toInt(nullptr, 10) & 0x0000000FF);
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDs3Reg, temp_data))
                    {
                        temp_data = ((i & 0x000000FF) << 16);
                        temp_data |= 0x00000002; // set the message interval
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
                        {
                            // ok
                        }
                        else
                        {
                            ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PtpTcPortDsPDelayReqLogMsgIntervalValue->setText("NA");
                    }
                }
            }

            // only for the selected port
            if (i == ui->PtpTcPortDsPortNrComboBox->currentText().toUInt(nullptr, 10))
            {
                temp_string = ui->PtpTcPortDsAsymmetryValue->text();
                temp_data = temp_string.toInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDs4Reg, temp_data))
                {
                    temp_data = ((i & 0x000000FF) << 16);
                    temp_data |= 0x00000004; // set the asymmetry
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_PortDsControlReg, temp_data))
                    {
                        // ok
                    }
                    else
                    {
                        ui->PtpTcPortDsAsymmetryValue->setText("NA");
                    }
                }
                else
                {
                    ui->PtpTcPortDsAsymmetryValue->setText("NA");
                }
            }
        }
        else
        {
            // port clock id wrong
        }
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->PtpTcEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PtpTc_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->PtpTcEnableCheckBox->setChecked(false);
    }
}

void Ucm_PtpTcTab::ptp_tc_read_values_button_clicked(void)
{
    ptp_tc_read_values();
}

void Ucm_PtpTcTab::ptp_tc_write_values_button_clicked(void)
{
    ptp_tc_write_values();
    ptp_tc_read_values();
}

void Ucm_PtpTcTab::ptp_tc_auto_refresh_button_clicked(void)
{
    if (ui->PtpTcAutoRefreshButton->text() == "Start Refresh")
    {
        ui->PtpTcAutoRefreshButton->setEnabled(false);

        ui->PtpTcInstanceComboBox->setEnabled(false);
        ui->PtpTcReadValuesButton->setEnabled(false);
        ui->PtpTcWriteValuesButton->setEnabled(false);

        ptp_tc_delay_number_of_points = 0;
        for (int i=0; i<4; i++)
        {
            ptp_tc_delay_series.at(i)->clear();
        }

        ptp_tc_timer->start(1000);

        ui->PtpTcAutoRefreshButton->setText("Stop Refresh");
        ui->PtpTcAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->PtpTcAutoRefreshButton->setEnabled(false);

        ptp_tc_timer->stop();

        ui->PtpTcInstanceComboBox->setEnabled(true);
        ui->PtpTcReadValuesButton->setEnabled(true);
        ui->PtpTcWriteValuesButton->setEnabled(true);

        ui->PtpTcAutoRefreshButton->setText("Start Refresh");
        ui->PtpTcAutoRefreshButton->setEnabled(true);
    }
}


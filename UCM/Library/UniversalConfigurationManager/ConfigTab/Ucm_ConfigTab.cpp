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

#include "Ucm_ConfigTab.h"
#include "ui_Ucm_ConfigTab.h"

Ucm_ConfigTab::Ucm_ConfigTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QString com_port;

    ucm = parent;

    ui = new Ui::Ucm_ConfigTab();
    ui->setupUi(this);

    cout << "INFO: " << "Available Ports:" << endl;
    for (int i = 0; i < com_ports.size(); i++)
    {
        com_port = com_ports[i].portName();
        cout << com_port.toLatin1().constData() << endl;
        ui->ConfigComPortComboBox->addItem(com_port, QVariant());
    }
    cout << endl;

    connect(ui->ConfigComPortButton, SIGNAL(clicked()), this, SLOT(config_com_port_button_clicked()));
}

Ucm_ConfigTab::~Ucm_ConfigTab()
{
    delete ui;
}

void Ucm_ConfigTab::config_com_port_button_clicked(void)
{
    unsigned int temp_data;
    Ucm_CoreConfig temp_config;
    QString temp_string;

    if (ui->ConfigComPortButton->text() == "open")
    {
        ui->ConfigComPortButton->setEnabled(false);
        ui->ConfigComPortComboBox->setEnabled(false);
        if (0 == ucm->com_lib.open_port(ui->ConfigComPortComboBox->currentText()))
        {
            // Advanced Tab
            ucm->advanced_tab->advanced_enable();

            for (int i = 0; i < 256; i++)
            {
                if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * 16) + 0)), temp_data))
                {
                    if ((i == 0) && (temp_data != 0x00010001))
                    {
                        cout << "ERROR: " << "not a conf block at the address expected" << endl;
                        break;
                    }
                    else if (temp_data == 0)
                    {
                        break;
                    }
                    else
                    {
                        temp_config.core_type = ((temp_data >> 16) & 0x0000FFFF);
                        temp_config.core_instance_nr = ((temp_data >> 0) & 0x0000FFFF);
                    }
                }
                else
                {
                    break;
                }

                if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * 16) + 4)), temp_data))
                {
                    temp_config.address_range_low = temp_data;
                }
                else
                {
                    break;
                }

                if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * 16) + 8)), temp_data))
                {
                    temp_config.address_range_high= temp_data;
                }
                else
                {
                    break;
                }

                if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * 16) + 12)), temp_data))
                {
                    temp_config.interrupt_mask = temp_data;
                }
                else
                {
                    break;
                }

                ucm->core_config.append(temp_config);
            }

            if (false == ucm->core_config.isEmpty())
            {
                ui->ConfigAddressMapValue->clear();
            }
            else
            {
                temp_string.append("NA");
            }

            for (int i = 0; i < ucm->core_config.size(); i++)
            {
                temp_string.append("0x");
                temp_string.append(QString("%1").arg(ucm->core_config.at(i).address_range_low, 8, 16, QLatin1Char('0')).toUpper());
                temp_string.append(" - ");
                temp_string.append("0x");
                temp_string.append(QString("%1").arg(ucm->core_config.at(i).address_range_high, 8, 16, QLatin1Char('0')).toUpper());
                temp_string.append(", ");
                temp_string.append("InstNr: ");
                temp_string.append(QString("%1").arg(ucm->core_config.at(i).core_instance_nr, 4, 10, QLatin1Char('0')).toUpper());
                temp_string.append(", ");
                temp_string.append("Type: ");

                switch (ucm->core_config.at(i).core_type)
                {
                case Ucm_CoreConfig_ConfSlaveCoreType:
                    temp_string.append("CONF Slave");
                    break;
                case Ucm_CoreConfig_ClkClockCoreType:
                    ucm->clk_clock_tab->clk_clock_enable();
                    ucm->clk_clock_tab->clk_clock_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->clk_clock_tab, "CLK Clock");
                    temp_string.append("CLK Clock");
                    break;
                case Ucm_CoreConfig_ClkSignalGeneratorCoreType:
                    ucm->clk_sg_tab->clk_sg_enable();
                    ucm->clk_sg_tab->clk_sg_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->clk_sg_tab, "CLK Sg");
                    temp_string.append("CLK SignalGenearator");
                    break;
                case Ucm_CoreConfig_ClkSignalTimestamperCoreType:
                    ucm->clk_ts_tab->clk_ts_enable();
                    ucm->clk_ts_tab->clk_ts_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->clk_ts_tab, "CLK Ts");
                    temp_string.append("CLK SignalTimestamper");
                    break;
                case Ucm_CoreConfig_IrigSlaveCoreType:
                    ucm->irig_slave_tab->irig_slave_enable();
                    ucm->irig_slave_tab->irig_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->irig_slave_tab, "IRIG Slave");
                    temp_string.append("IRIG Slave");
                    break;
                case Ucm_CoreConfig_IrigMasterCoreType:
                    ucm->irig_master_tab->irig_master_enable();
                    ucm->irig_master_tab->irig_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->irig_master_tab, "IRIG Master");
                    temp_string.append("IRIG Master");
                    break;
                case Ucm_CoreConfig_PpsSlaveCoreType:
                    ucm->pps_slave_tab->pps_slave_enable();
                    ucm->pps_slave_tab->pps_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->pps_slave_tab, "PPS Slave");
                    temp_string.append("PPS Slave");
                    break;
                case Ucm_CoreConfig_PpsMasterCoreType:
                    ucm->pps_master_tab->pps_master_enable();
                    ucm->pps_master_tab->pps_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->pps_master_tab, "PPS Master");
                    temp_string.append("PPS Master");
                    break;
                case Ucm_CoreConfig_PtpOrdinaryClockCoreType:
                    ucm->ptp_oc_tab->ptp_oc_enable();
                    ucm->ptp_oc_tab->ptp_oc_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->ptp_oc_tab, "PTP Oc");
                    temp_string.append("PTP OrdinaryClock");
                    break;
                case Ucm_CoreConfig_PtpTransparentClockCoreType:
                    ucm->ptp_tc_tab->ptp_tc_enable();
                    ucm->ptp_tc_tab->ptp_tc_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->ptp_tc_tab, "PTP Tc");
                    temp_string.append("PTP TransparentClock");
                    break;
                case Ucm_CoreConfig_PtpHybridClockCoreType:
                    ucm->ptp_hc_tab->ptp_hc_enable();
                    ucm->ptp_hc_tab->ptp_hc_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->ptp_hc_tab, "PTP Hc");
                    temp_string.append("PTP HybridClock");
                    break;
                case Ucm_CoreConfig_RedHsrPrpCoreType:
                    ucm->red_hsrprp_tab->red_hsrprp_enable();
                    ucm->red_hsrprp_tab->red_hsrprp_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->red_hsrprp_tab, "RED HsrPrp");
                    temp_string.append("RED HsrPrp");
                    break;
                case Ucm_CoreConfig_RtcSlaveCoreType:
                    // TODO
                    temp_string.append("RTC Slave");
                    break;
                case Ucm_CoreConfig_RtcMasterCoreType:
                    ucm->rtc_master_tab->rtc_master_enable();
                    ucm->rtc_master_tab->rtc_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->rtc_master_tab, "RTC Master");
                    temp_string.append("RTC Master");
                    break;
                case Ucm_CoreConfig_TodSlaveCoreType:
                    ucm->tod_slave_tab->tod_slave_enable();
                    ucm->tod_slave_tab->tod_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                    ucm->MainTab->addTab(ucm->tod_slave_tab, "TOD Slave");
                    temp_string.append("TOD Slave");
                    break;
                case Ucm_CoreConfig_TodMasterCoreType:
                    // TODO
                    temp_string.append("TOD Master");
                    break;
                default:
                    temp_string.append("UNKNOWN");
                    break;
                }
                temp_string.append("\n");
            }
            ui->ConfigAddressMapValue->setText(temp_string);

            ui->ConfigComPortButton->setText("close");
            ui->ConfigComPortComboBox->setEnabled(false);
        }
        else
        {
            ui->ConfigComPortComboBox->setEnabled(true);
        }
        ui->ConfigComPortButton->setEnabled(true);
    }
    else
    {
        ui->ConfigComPortButton->setEnabled(false);
        if (0 == ucm->com_lib.close_port())
        {
            // Config Tab
            ui->ConfigAddressMapValue->setText("NA");
            ui->ConfigComPortComboBox->setEnabled(true);
            ui->ConfigComPortButton->setText("open");
            ucm->core_config.clear();

            // Advanced Tab
            ucm->advanced_tab->advanced_disable();

            //Remove all other tabs
            while (ucm->MainTab->count() > 2)
            {
                ucm->MainTab->removeTab(2);
            }

            // CLK Clock Tab
            ucm->clk_clock_tab->clk_clock_disable();

            // CLK Ts Tab
            ucm->clk_ts_tab->clk_ts_disable();

            // CLK Sg Tab
            ucm->clk_sg_tab->clk_sg_disable();

            // RTC Master Tab
            ucm->rtc_master_tab->rtc_master_disable();

            // PPS Slave Tab
            ucm->pps_slave_tab->pps_slave_disable();

            // PPS Master Tab
            ucm->pps_master_tab->pps_master_disable();

            // TOD Slave Tab
            ucm->tod_slave_tab->tod_slave_disable();

            // RED HsrPrp Tab
            ucm->red_hsrprp_tab->red_hsrprp_disable();

            // PTP Oc Tab
            ucm->ptp_oc_tab->ptp_oc_disable();

            // PTP Tc Tab
            ucm->ptp_tc_tab->ptp_tc_disable();

            // PTP Hc Tab
            ucm->ptp_hc_tab->ptp_hc_disable();

            // IRIG Slave Tab
            ucm->irig_slave_tab->irig_slave_disable();

            // IRIG Master Tab
            ucm->irig_master_tab->irig_master_disable();

        }
        ui->ConfigComPortButton->setEnabled(true);
    }
}

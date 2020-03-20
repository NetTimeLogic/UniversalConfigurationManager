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

#include <Ucm_ConfigTab.h>
#include <ui_Ucm_ConfigTab.h>

Ucm_ConfigTab::Ucm_ConfigTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QString com_port;

    ucm = parent;

    ui = new Ui::Ucm_ConfigTab();
    ui->setupUi(this);

    config_timer = new QTimer(this);
    config_timer->stop();

    cout << "INFO: " << "Available Ports:" << endl;
    for (int i = 0; i < com_ports.size(); i++)
    {
        com_port = com_ports[i].portName();
        cout << com_port.toLatin1().constData() << endl;
        ui->ConfigComPortComboBox->addItem(com_port, QVariant());
    }
    cout << endl;

    connect(ui->ConfigComPortButton, SIGNAL(clicked()), this, SLOT(config_com_port_button_clicked()));
    connect(ui->ConfigEthPortButton, SIGNAL(clicked()), this, SLOT(config_eth_port_button_clicked()));
    connect(ui->ConfigFullScreenButton, SIGNAL(clicked()), this, SLOT(config_full_screen_button_clicked()));
    connect(config_timer, SIGNAL(timeout()), this, SLOT(config_port_check_timer()));
}

Ucm_ConfigTab::~Ucm_ConfigTab()
{
    config_timer->stop();
    delete ui;
    delete config_timer;
}

int Ucm_ConfigTab::config_resize(int height, int width)
{
    int height_delta = 0;
    int width_delta = 0;

    height_delta = (height-Ucm_MainHeight);
    width_delta = (width-Ucm_MainWidth);

    ui->ConfigAddressMapValue->setFixedHeight(630+height_delta);
    ui->ConfigAddressMapValue->setFixedWidth(1340+width_delta);

    updateGeometry();

    return 0;
}

void Ucm_ConfigTab::config_com_port_button_clicked(void)
{
    if (ui->ConfigComPortButton->text() == "Open")
    {
        ui->ConfigComPortButton->setEnabled(false);
        ui->ConfigComPortComboBox->setEnabled(false);
        if (0 == ucm->com_lib.open_port(ui->ConfigComPortComboBox->currentText()))
        {
            config_port_button_clicked(1);
            config_timer->start(2000);
            ui->ConfigComPortButton->setText("Close");
            ui->ConfigComPortComboBox->setEnabled(false);
            ui->ConfigEthPortButton->setEnabled(false);
            ui->ConfigEthPortValue->setEnabled(false);
        }
        else
        {
            QMessageBox::information(this, tr("Open COM Port"), tr("Opening COM Port failed"));
            ui->ConfigComPortComboBox->setEnabled(true);
        }
        ui->ConfigComPortButton->setEnabled(true);
    }
    else
    {
        ui->ConfigComPortButton->setEnabled(false);
        if (0 == ucm->com_lib.close_port())
        {
            config_port_button_clicked(0);
            config_timer->stop();
            ui->ConfigComPortButton->setText("Open");
            ui->ConfigComPortComboBox->setEnabled(true);
            ui->ConfigEthPortValue->setEnabled(true);
            ui->ConfigComPortButton->setEnabled(true);
        }
        ui->ConfigEthPortButton->setEnabled(true);
    }
}

void Ucm_ConfigTab::config_eth_port_button_clicked(void)
{
    if (ui->ConfigEthPortButton->text() == "Open")
    {
            ui->ConfigEthPortButton->setEnabled(false);
            ui->ConfigEthPortValue->setEnabled(false);
        if (0 == ucm->com_lib.open_port(ui->ConfigEthPortValue->text()))
        {
            config_port_button_clicked(1);
            config_timer->start(2000);
            ui->ConfigEthPortButton->setText("Close");
            ui->ConfigEthPortValue->setEnabled(false);
            ui->ConfigComPortButton->setEnabled(false);
            ui->ConfigComPortComboBox->setEnabled(false);
        }
        else
        {
            QMessageBox::information(this, tr("Open ETH Port"), tr("Opening ETH Port failed"));
            ui->ConfigEthPortValue->setEnabled(true);
        }
        ui->ConfigEthPortButton->setEnabled(true);
    }
    else
    {
        ui->ConfigEthPortButton->setEnabled(false);
        if (0 == ucm->com_lib.close_port())
        {
            config_port_button_clicked(0);
            config_timer->stop();
            ui->ConfigEthPortButton->setText("Open");
            ui->ConfigEthPortValue->setEnabled(true);
            ui->ConfigComPortButton->setEnabled(true);
            ui->ConfigComPortComboBox->setEnabled(true);
        }
        ui->ConfigEthPortButton->setEnabled(true);
    }
}

void Ucm_ConfigTab::config_port_button_clicked(int open)
{
    unsigned int temp_data = 0;
    Ucm_CoreConfig temp_config;
    QString temp_string;

    if (open == 1)
    {
        // Advanced Tab
        ucm->advanced_tab->advanced_enable();

        for (int i = 0; i < 256; i++)
        {
            if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_TypeInstanceReg)), temp_data))
            {
                if ((i == 0) &&
                    ((((temp_data >> 16) & 0x0000FFFF) != Ucm_CoreConfig_ConfSlaveCoreType) ||
                     (((temp_data >> 0) & 0x0000FFFF) != 1)))
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

            if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_BaseAddrLReg)), temp_data))
            {
                temp_config.address_range_low = temp_data;
            }
            else
            {
                break;
            }

            if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_BaseAddrHReg)), temp_data))
            {
                temp_config.address_range_high= temp_data;
            }
            else
            {
                break;
            }

            if (0 == ucm->com_lib.read_reg((0x00000000 + ((i * Ucm_Config_BlockSize) + Ucm_Config_IrqMaskReg)), temp_data))
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
                ucm->Ucm_MainTab->addTab(ucm->clk_clock_tab, "CLK Clock");
                temp_string.append("CLK Clock");
                break;
            case Ucm_CoreConfig_ClkSignalGeneratorCoreType:
                ucm->clk_sg_tab->clk_sg_enable();
                ucm->clk_sg_tab->clk_sg_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->clk_sg_tab, "CLK Sg");
                temp_string.append("CLK SignalGenearator");
                break;
            case Ucm_CoreConfig_ClkSignalTimestamperCoreType:
                ucm->clk_ts_tab->clk_ts_enable();
                ucm->clk_ts_tab->clk_ts_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->clk_ts_tab, "CLK Ts");
                temp_string.append("CLK SignalTimestamper");
                break;
            case Ucm_CoreConfig_IrigSlaveCoreType:
                ucm->irig_slave_tab->irig_slave_enable();
                ucm->irig_slave_tab->irig_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->irig_slave_tab, "IRIG Slave");
                temp_string.append("IRIG Slave");
                break;
            case Ucm_CoreConfig_IrigMasterCoreType:
                ucm->irig_master_tab->irig_master_enable();
                ucm->irig_master_tab->irig_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->irig_master_tab, "IRIG Master");
                temp_string.append("IRIG Master");
                break;
            case Ucm_CoreConfig_PpsSlaveCoreType:
                ucm->pps_slave_tab->pps_slave_enable();
                ucm->pps_slave_tab->pps_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->pps_slave_tab, "PPS Slave");
                temp_string.append("PPS Slave");
                break;
            case Ucm_CoreConfig_PpsMasterCoreType:
                ucm->pps_master_tab->pps_master_enable();
                ucm->pps_master_tab->pps_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->pps_master_tab, "PPS Master");
                temp_string.append("PPS Master");
                break;
            case Ucm_CoreConfig_PtpOrdinaryClockCoreType:
                ucm->ptp_oc_tab->ptp_oc_enable();
                ucm->ptp_oc_tab->ptp_oc_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->ptp_oc_tab, "PTP Oc");
                temp_string.append("PTP OrdinaryClock");
                break;
            case Ucm_CoreConfig_PtpTransparentClockCoreType:
                ucm->ptp_tc_tab->ptp_tc_enable();
                ucm->ptp_tc_tab->ptp_tc_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->ptp_tc_tab, "PTP Tc");
                temp_string.append("PTP TransparentClock");
                break;
            case Ucm_CoreConfig_PtpHybridClockCoreType:
                ucm->ptp_hc_tab->ptp_hc_enable();
                ucm->ptp_hc_tab->ptp_hc_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->ptp_hc_tab, "PTP Hc");
                temp_string.append("PTP HybridClock");
                break;
            case Ucm_CoreConfig_RedHsrPrpCoreType:
                ucm->red_hsrprp_tab->red_hsrprp_enable();
                ucm->red_hsrprp_tab->red_hsrprp_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->red_hsrprp_tab, "RED HsrPrp");
                temp_string.append("RED HsrPrp");
                break;
            case Ucm_CoreConfig_RedTsnCoreType:
                ucm->red_tsn_tab->red_tsn_enable();
                ucm->red_tsn_tab->red_tsn_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->red_tsn_tab, "RED Tsn");
                temp_string.append("RED Tsn");
                break;
            case Ucm_CoreConfig_RtcSlaveCoreType:
                // TODO
                temp_string.append("RTC Slave");
                break;
            case Ucm_CoreConfig_RtcMasterCoreType:
                ucm->rtc_master_tab->rtc_master_enable();
                ucm->rtc_master_tab->rtc_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->rtc_master_tab, "RTC Master");
                temp_string.append("RTC Master");
                break;
            case Ucm_CoreConfig_DcfSlaveCoreType:
                ucm->dcf_slave_tab->dcf_slave_enable();
                ucm->dcf_slave_tab->dcf_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->dcf_slave_tab, "DCF Slave");
                temp_string.append("DCF Slave");
                break;
            case Ucm_CoreConfig_DcfMasterCoreType:
                ucm->dcf_master_tab->dcf_master_enable();
                ucm->dcf_master_tab->dcf_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->dcf_master_tab, "DCF Master");
                temp_string.append("DCF Master");
                break;
            case Ucm_CoreConfig_TodSlaveCoreType:
                ucm->tod_slave_tab->tod_slave_enable();
                ucm->tod_slave_tab->tod_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->tod_slave_tab, "TOD Slave");
                temp_string.append("TOD Slave");
                break;
            case Ucm_CoreConfig_TodMasterCoreType:
                ucm->tod_master_tab->tod_master_enable();
                ucm->tod_master_tab->tod_master_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->tod_master_tab, "TOD Master");
                temp_string.append("TOD Master");
                break;
            case Ucm_CoreConfig_TapSlaveCoreType:
                ucm->tap_slave_tab->tap_slave_enable();
                ucm->tap_slave_tab->tap_slave_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->tap_slave_tab, "TAP Slave");
                temp_string.append("TAP Slave");
                break;
            case Ucm_CoreConfig_TsnIicCoreType:
                ucm->tsn_iic_tab->tsn_iic_enable();
                ucm->tsn_iic_tab->tsn_iic_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->tsn_iic_tab, "TSN Iic");
                temp_string.append("TSN Iic");
                break;
            case Ucm_CoreConfig_PhyConfigurationCoreType:
                ucm->phy_conf_tab->phy_conf_enable();
                ucm->phy_conf_tab->phy_conf_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->phy_conf_tab, "PHY Conf");
                temp_string.append("PHY Configuration");
                break;
            case Ucm_CoreConfig_I2cConfigurationCoreType:
                ucm->i2c_conf_tab->i2c_conf_enable();
                ucm->i2c_conf_tab->i2c_conf_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->i2c_conf_tab, "I2C Conf");
                temp_string.append("I2C Configuration");
                break;
            case Ucm_CoreConfig_IoConfigurationCoreType:
                ucm->io_conf_tab->io_conf_enable();
                ucm->io_conf_tab->io_conf_add_instance(ucm->core_config.at(i).core_instance_nr);
                ucm->Ucm_MainTab->addTab(ucm->io_conf_tab, "IO Conf");
                temp_string.append("IO Configuration");
                break;
            default:
                temp_string.append("UNKNOWN");
                break;
            }
            temp_string.append("\n");
        }
        ui->ConfigAddressMapValue->setText(temp_string);

    }
    else
    {
        // Config Tab
        ui->ConfigAddressMapValue->setText("NA");
        ucm->core_config.clear();

        // Advanced Tab
        ucm->advanced_tab->advanced_disable();

        //Remove all other tabs
        while (ucm->Ucm_MainTab->count() > 2)
        {
            ucm->Ucm_MainTab->removeTab(2);
        }

        // CLK Clock Tab
        ucm->clk_clock_tab->clk_clock_disable();

        // CLK Ts Tab
        ucm->clk_ts_tab->clk_ts_disable();

        // CLK Sg Tab
        ucm->clk_sg_tab->clk_sg_disable();

        // RTC Master Tab
        ucm->rtc_master_tab->rtc_master_disable();

        // DCF Slave Tab
        ucm->dcf_slave_tab->dcf_slave_disable();

        // DCF Master Tab
        ucm->dcf_master_tab->dcf_master_disable();

        // PPS Slave Tab
        ucm->pps_slave_tab->pps_slave_disable();

        // PPS Master Tab
        ucm->pps_master_tab->pps_master_disable();

        // TOD Slave Tab
        ucm->tod_slave_tab->tod_slave_disable();

        // TOD Master Tab
        ucm->tod_master_tab->tod_master_disable();

        // RED HsrPrp Tab
        ucm->red_hsrprp_tab->red_hsrprp_disable();

        // RED Tsn Tab
        ucm->red_tsn_tab->red_tsn_disable();

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

        // TAP Slave Tab
        ucm->tap_slave_tab->tap_slave_disable();
        
        // TSN Iic Tab
        ucm->tsn_iic_tab->tsn_iic_disable();
        
        // PHY Conf Tab
        ucm->phy_conf_tab->phy_conf_disable();

        // I2C Conf Tab
        ucm->i2c_conf_tab->i2c_conf_disable();

        // IO Conf Tab
        ucm->io_conf_tab->io_conf_disable();
    }
}

void Ucm_ConfigTab::config_full_screen_button_clicked(void)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    if (ui->ConfigFullScreenButton->text() == "Full Screen")
    {
        ucm->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint);
        ucm->showFullScreen();
        ui->ConfigFullScreenButton->setText("Normal Screen");
    }
    else
    {
        ucm->setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        ucm->showNormal();
        ui->ConfigFullScreenButton->setText("Full Screen");
        if (height > (Ucm_MainHeight+2))
        {
            height = (Ucm_MainHeight+2);
        }
        if (width > Ucm_MainWidth)
        {
            width = Ucm_MainWidth;
        }
        ucm->resize(width, height);
    }

}

void Ucm_ConfigTab::config_port_check_timer(void)
{
    static unsigned int error_cnt;

    if (0 == ucm->com_lib.check_port())
    {
        error_cnt = 0;
    }
    else
    {
        error_cnt++;
    }

    if (error_cnt > 2)
    {
        cout << "ERROR: " << "port is not ok anymore closing it" << endl;
        if (ucm->com_lib.port_type == Ucm_CommunicationLib_ComType)
        {
            QMessageBox::information(this, tr("COM Port Errror"), tr("Communication with target failed, COM port closed"));
            ucm->Ucm_MainTab->setCurrentIndex(0);
            config_com_port_button_clicked();
        }
        else if(ucm->com_lib.port_type == Ucm_CommunicationLib_EthType)
        {
            QMessageBox::information(this, tr("ETH Port Errror"), tr("Communication with target failed, ETH port closed"));
            ucm->Ucm_MainTab->setCurrentIndex(0);
            config_eth_port_button_clicked();
        }
        else
        {
            cout << "ERROR: " << "unknown port type" << endl;
        }
        error_cnt = 0;
    }
}

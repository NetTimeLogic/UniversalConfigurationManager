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

#include "Ucm_UniversalConfigurationManager.h"

using namespace std;
#include <iostream>
#include <streambuf>
#include <string>

Ucm_UniversalConfigurationManager::Ucm_UniversalConfigurationManager(QMainWindow *parent) : QMainWindow(parent)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    cout << "INFO: " << "Screen size: " << width << "x" << height << endl;

    setupUi(this);

    // File menu
    connect(Ucm_Exit, SIGNAL(triggered()), this, SLOT(ucm_file_exit_clicked()));

    // Config Tab
    config_tab = new Ucm_ConfigTab(this);
    Ucm_MainTab->addTab(config_tab, "Config"); // show this at startup

    // Advanced Tab
    advanced_tab = new Ucm_AdvancedTab(this);
    Ucm_MainTab->addTab(advanced_tab, "Advanced"); // show this at startup

    // CLK Clock Tab
    clk_clock_tab = new Ucm_ClkClockTab(this);

    // CLK Ts Tab
    clk_ts_tab = new Ucm_ClkTsTab(this);

    // CLK Sg Tab
    clk_sg_tab = new Ucm_ClkSgTab(this);

    // RTC Master Tab
    rtc_master_tab = new Ucm_RtcMasterTab(this);

    // PPS Slave Tab
    pps_slave_tab = new Ucm_PpsSlaveTab(this);

    // PPS Master Tab
    pps_master_tab = new Ucm_PpsMasterTab(this);

    // IRIG Slave Tab
    irig_slave_tab = new Ucm_IrigSlaveTab(this);

    // IRIG Master Tab
    irig_master_tab = new Ucm_IrigMasterTab(this);

    // TOD Slave Tab
    tod_slave_tab = new Ucm_TodSlaveTab(this);

    // TOD Master Tab
    tod_master_tab = new Ucm_TodMasterTab(this);

    // RED HsrPrp Tab
    red_hsrprp_tab = new Ucm_RedHsrPrpTab(this);

    // PTP Oc Tab
    ptp_oc_tab = new Ucm_PtpOcTab(this);

    // PTP Tc Tab
    ptp_tc_tab = new Ucm_PtpTcTab(this);

    // PTP Hc Tab
    ptp_hc_tab = new Ucm_PtpHcTab(this);

    // TAP Slave Tab
    tap_slave_tab = new Ucm_TapSlaveTab(this);

    if (height > 822)
    {
        height = 822;
    }
    if (width > 1380)
    {
        width = 1380;
    }
    this->resize(width, height);
}


Ucm_UniversalConfigurationManager::~Ucm_UniversalConfigurationManager()
{
    delete config_tab;
    delete advanced_tab;
    delete clk_clock_tab;
    delete clk_ts_tab;
    delete clk_sg_tab;
    delete rtc_master_tab;
    delete pps_slave_tab;
    delete pps_master_tab;
    delete irig_slave_tab;
    delete irig_master_tab;
    delete tod_slave_tab;
    delete tod_master_tab;
    delete red_hsrprp_tab;
    delete ptp_oc_tab;
    delete ptp_tc_tab;
    delete ptp_hc_tab;
    delete tap_slave_tab;
    core_config.clear();
}

int Ucm_UniversalConfigurationManager::ucm_resize(int height, int width)
{
    int height_delta = 0;
    int width_delta = 0;

    cout << "INFO: " << "Resize to: " << width << "x" << height << endl;

    height_delta = (height-820);
    width_delta = (width-1380);

    Ucm_MainTab->setFixedHeight(800+height_delta);
    Ucm_MainTab->setFixedWidth(1380+width_delta);

    Ucm_MainMenu->setFixedWidth(1380+width_delta);

    // Config Tab
    config_tab->config_resize(height, width);

    // Advanced Tab
    advanced_tab->advanced_resize(height, width);

    // CLK Clock Tab
    clk_clock_tab->clk_clock_resize(height, width);

    // CLK Ts Tab
    clk_ts_tab->clk_ts_resize(height, width);

    // CLK Sg Tab
    clk_sg_tab->clk_sg_resize(height, width);

    // RTC Master Tab
    rtc_master_tab->rtc_master_resize(height, width);

    // PPS Slave Tab
    pps_slave_tab->pps_slave_resize(height, width);

    // PPS Master Tab
    pps_master_tab->pps_master_resize(height, width);

    // IRIG Slave Tab
    irig_slave_tab->irig_slave_resize(height, width);

    // IRIG Master Tab
    irig_master_tab->irig_master_resize(height, width);

    // TOD Slave Tab
    tod_slave_tab->tod_slave_resize(height, width);

    // TOD Master Tab
    tod_master_tab->tod_master_resize(height, width);

    // RED HsrPrp Tab
    red_hsrprp_tab->red_hsrprp_resize(height, width);

    // PTP Oc Tab
    ptp_oc_tab->ptp_oc_resize(height, width);

    // PTP Tc Tab
    ptp_tc_tab->ptp_tc_resize(height, width);

    // PTP Hc Tab
    ptp_hc_tab->ptp_hc_resize(height, width);

    // TAP Slave Tab
    tap_slave_tab->tap_slave_resize(height, width);

    updateGeometry();

    return 0;
}

/******************************************************************/
// Resize
/******************************************************************/
void Ucm_UniversalConfigurationManager::resizeEvent(QResizeEvent* event)
{
   QSize new_size = this->size();

   ucm_resize(new_size.height(), new_size.width());

}

/******************************************************************/
// X Close
/******************************************************************/
void Ucm_UniversalConfigurationManager::closeEvent(QCloseEvent *event)
{
    com_lib.~Ucm_CommunicationLib();
    exit(0);
}

/******************************************************************/
// File menu
/******************************************************************/
void Ucm_UniversalConfigurationManager::ucm_file_exit_clicked(void)
{
    com_lib.~Ucm_CommunicationLib();
    exit(0);
}


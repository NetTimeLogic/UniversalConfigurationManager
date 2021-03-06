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

#ifndef UCM_UNIVERSALCONFIGURATIONMANAGER_H
#define UCM_UNIVERSALCONFIGURATIONMANAGER_H

#include <QScreen>
#include <QCloseEvent>
#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QValueAxis>
#include <ui_Ucm_UniversalConfigurationManager.h>
#include <CommunicationLib/Ucm_CommunicationLib.h>
#include <CoreConfig/Ucm_CoreConfig.h>
#include <DebugStream/Ucm_DebugStream.h>

#include <ConfigTab/Ucm_ConfigTab.h>
#include <AdvancedTab/Ucm_AdvancedTab.h>
#include <ClkClockTab/Ucm_ClkClockTab.h>
#include <ClkTsTab/Ucm_ClkTsTab.h>
#include <ClkSgTab/Ucm_ClkSgTab.h>
#include <RtcMasterTab/Ucm_RtcMasterTab.h>
#include <DcfSlaveTab/Ucm_DcfSlaveTab.h>
#include <DcfMasterTab/Ucm_DcfMasterTab.h>
#include <PpsSlaveTab/Ucm_PpsSlaveTab.h>
#include <PpsMasterTab/Ucm_PpsMasterTab.h>
#include <IrigSlaveTab/Ucm_IrigSlaveTab.h>
#include <IrigMasterTab/Ucm_IrigMasterTab.h>
#include <TodSlaveTab/Ucm_TodSlaveTab.h>
#include <TodMasterTab/Ucm_TodMasterTab.h>
#include <RedHsrPrpTab/Ucm_RedHsrPrpTab.h>
#include <RedTsnTab/Ucm_RedTsnTab.h>
#include <PtpOcTab/Ucm_PtpOcTab.h>
#include <PtpTcTab/Ucm_PtpTcTab.h>
#include <PtpHcTab/Ucm_PtpHcTab.h>
#include <TapSlaveTab/Ucm_TapSlaveTab.h>
#include <TsnIicTab/Ucm_TsnIicTab.h>
#include <PhyConfTab/Ucm_PhyConfTab.h>
#include <I2cConfTab/Ucm_I2cConfTab.h>
#include <IoConfTab/Ucm_IoConfTab.h>
#include <HelpMenu/Ucm_HelpMenuAbout.h>

using namespace QtCharts;

class Ucm_ConfigTab;
class Ucm_AdvancedTab;
class Ucm_ClkClockTab;
class Ucm_ClkTsTab;
class Ucm_ClkSgTab;
class Ucm_RtcMasterTab;
class Ucm_DcfSlaveTab;
class Ucm_DcfMasterTab;
class Ucm_PpsSlaveTab;
class Ucm_PpsMasterTab;
class Ucm_IrigSlaveTab;
class Ucm_IrigMasterTab;
class Ucm_TodSlaveTab;
class Ucm_TodMasterTab;
class Ucm_RedHsrPrpTab;
class Ucm_RedTsnTab;
class Ucm_PtpOcTab;
class Ucm_PtpTcTab;
class Ucm_PtpHcTab;
class Ucm_TapSlaveTab;
class Ucm_TsnIicTab;
class Ucm_PhyConfTab;
class Ucm_I2cConfTab;
class Ucm_IoConfTab;
class Ucm_HelpMenuAbout;

#define Ucm_Version_Nr               "4.1.00"

#define Ucm_MainHeight               820
#define Ucm_MainWidth                1380

class Ucm_UniversalConfigurationManager : public QMainWindow, public Ui::Ucm_UniversalConfigurationManager
{
    Q_OBJECT

    public:
        Ucm_UniversalConfigurationManager(QMainWindow *parent = 0);
        ~Ucm_UniversalConfigurationManager();

        int ucm_resize(int height, int width);

        Ucm_CommunicationLib com_lib;
        QList<Ucm_CoreConfig> core_config;

        // Config tab
        Ucm_ConfigTab* config_tab;

        // Advcanced tab
        Ucm_AdvancedTab* advanced_tab;

        // CLK Clock tab
        Ucm_ClkClockTab* clk_clock_tab;

        // CLK Ts tab
        Ucm_ClkTsTab* clk_ts_tab;

        // CLK Sg tab
        Ucm_ClkSgTab* clk_sg_tab;

        // RTC Master tab
        Ucm_RtcMasterTab* rtc_master_tab;

        // DCF Slave tab
        Ucm_DcfSlaveTab* dcf_slave_tab;

        // DCF Master tab
        Ucm_DcfMasterTab* dcf_master_tab;

        // PPS Slave tab
        Ucm_PpsSlaveTab* pps_slave_tab;

        // PPS Master tab
        Ucm_PpsMasterTab* pps_master_tab;

        // IRIG Slave tab
        Ucm_IrigSlaveTab* irig_slave_tab;

        // IRIG Master tab
        Ucm_IrigMasterTab* irig_master_tab;

        // TOD Slave tab
        Ucm_TodSlaveTab* tod_slave_tab;

        // TOD Master tab
        Ucm_TodMasterTab* tod_master_tab;

        // RED HsrPrp tab
        Ucm_RedHsrPrpTab* red_hsrprp_tab;

        // RED Tsn tab
        Ucm_RedTsnTab* red_tsn_tab;

        // PTP Oc tab
        Ucm_PtpOcTab* ptp_oc_tab;

        // PTP Tc tab
        Ucm_PtpTcTab* ptp_tc_tab;

        // PTP Hc tab
        Ucm_PtpHcTab* ptp_hc_tab;

        // TAP Slave tab
        Ucm_TapSlaveTab* tap_slave_tab;

        // TAP Slave tab
        Ucm_TsnIicTab* tsn_iic_tab;

        // Phy Config tab
        Ucm_PhyConfTab* phy_conf_tab;

        // I2c Config tab
        Ucm_I2cConfTab* i2c_conf_tab;

        // Io Config tab
        Ucm_IoConfTab* io_conf_tab;

private:
        void resizeEvent(QResizeEvent* event);
        void closeEvent(QCloseEvent *event);

        Ucm_HelpMenuAbout about_screen;

private slots:
        // File menu
        void ucm_file_exit_clicked(void);
        // Help menu
        void ucm_help_about_clicked(void);

};

#endif // UCM_UNIVERSALCONFIGURATIONMANAGER_H

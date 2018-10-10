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

#ifndef UCM_PTPTC_H
#define UCM_PTPTC_H

#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QValueAxis>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_PtpTc_ControlReg                            0x00000000
#define Ucm_PtpTc_StatusReg                             0x00000004
#define Ucm_PtpTc_VersionReg                            0x0000000C
#define Ucm_PtpTc_ConfigControlReg                      0x00000080
#define Ucm_PtpTc_ConfigProfileReg                      0x00000084
#define Ucm_PtpTc_ConfigVlanReg                         0x00000088
#define Ucm_PtpTc_ConfigIpReg                           0x0000008C
#define Ucm_PtpTc_DefaultDsControlReg                   0x00000100
#define Ucm_PtpTc_DefaultDs1Reg                         0x00000104
#define Ucm_PtpTc_DefaultDs2Reg                         0x00000108
#define Ucm_PtpTc_DefaultDs3Reg                         0x0000010C
#define Ucm_PtpTc_DefaultDs4Reg                         0x0000011C
#define Ucm_PtpTc_PortDsControlReg                      0x00000200
#define Ucm_PtpTc_PortDs1Reg                            0x00000204
#define Ucm_PtpTc_PortDs2Reg                            0x00000208
#define Ucm_PtpTc_PortDs3Reg                            0x00000210
#define Ucm_PtpTc_PortDs4Reg                            0x0000021C

namespace Ui {
class Ucm_PtpTcTab;
}

using namespace QtCharts;

class Ucm_PtpTcTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_PtpTcTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_PtpTcTab();

    int ptp_tc_resize(int height, int width);
    void ptp_tc_add_instance(unsigned int instance);
    int ptp_tc_disable(void);
    int ptp_tc_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_PtpTcTab *ui;

    // PTP Tc tab
    QTimer* ptp_tc_timer;

    QList<QLineSeries*> ptp_tc_delay_series;
    QChart* ptp_tc_delay_chart;
    QChartView* ptp_tc_delay_chart_view;
    unsigned int ptp_tc_delay_number_of_points;


    void ptp_tc_read_values(void);
    void ptp_tc_write_values(void);

private slots:
    // PTP Tc tab
    void ptp_tc_read_values_button_clicked(void);
    void ptp_tc_write_values_button_clicked(void);
    void ptp_tc_auto_refresh_button_clicked(void);

};

#endif // UCM_PTPTC_H

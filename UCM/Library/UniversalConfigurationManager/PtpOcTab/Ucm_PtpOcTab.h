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

#ifndef UCM_PTPOC_H
#define UCM_PTPOC_H

#include <QScrollArea>
#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QValueAxis>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_PtpOc_ControlReg                            0x00000000
#define Ucm_PtpOc_StatusReg                             0x00000004
#define Ucm_PtpOc_VersionReg                            0x0000000C
#define Ucm_PtpOc_ConfigControlReg                      0x00000080
#define Ucm_PtpOc_ConfigProfileReg                      0x00000084
#define Ucm_PtpOc_ConfigVlanReg                         0x00000088
#define Ucm_PtpOc_ConfigIpReg                           0x0000008C
#define Ucm_PtpOc_DefaultDsControlReg                   0x00000100
#define Ucm_PtpOc_DefaultDs1Reg                         0x00000104
#define Ucm_PtpOc_DefaultDs2Reg                         0x00000108
#define Ucm_PtpOc_DefaultDs3Reg                         0x0000010C
#define Ucm_PtpOc_DefaultDs4Reg                         0x00000110
#define Ucm_PtpOc_DefaultDs5Reg                         0x00000114
#define Ucm_PtpOc_DefaultDs6Reg                         0x00000118
#define Ucm_PtpOc_DefaultDs7Reg                         0x0000011C
#define Ucm_PtpOc_PortDsControlReg                      0x00000200
#define Ucm_PtpOc_PortDs1Reg                            0x00000204
#define Ucm_PtpOc_PortDs2Reg                            0x00000208
#define Ucm_PtpOc_PortDs3Reg                            0x0000020C
#define Ucm_PtpOc_PortDs4Reg                            0x00000210
#define Ucm_PtpOc_PortDs5Reg                            0x00000214
#define Ucm_PtpOc_PortDs6Reg                            0x00000218
#define Ucm_PtpOc_PortDs7Reg                            0x0000021C
#define Ucm_PtpOc_CurrentDsControlReg                   0x00000300
#define Ucm_PtpOc_CurrentDs1Reg                         0x00000304
#define Ucm_PtpOc_CurrentDs2Reg                         0x00000308
#define Ucm_PtpOc_CurrentDs3Reg                         0x0000030C
#define Ucm_PtpOc_CurrentDs4Reg                         0x00000310
#define Ucm_PtpOc_CurrentDs5Reg                         0x00000314
#define Ucm_PtpOc_ParentDsControlReg                    0x00000400
#define Ucm_PtpOc_ParentDs1Reg                          0x00000404
#define Ucm_PtpOc_ParentDs2Reg                          0x00000408
#define Ucm_PtpOc_ParentDs3Reg                          0x0000040C
#define Ucm_PtpOc_ParentDs4Reg                          0x00000410
#define Ucm_PtpOc_ParentDs5Reg                          0x00000414
#define Ucm_PtpOc_ParentDs6Reg                          0x00000418
#define Ucm_PtpOc_ParentDs7Reg                          0x0000041C
#define Ucm_PtpOc_ParentDs8Reg                          0x00000420
#define Ucm_PtpOc_ParentDs9Reg                          0x00000424
#define Ucm_PtpOc_TimePropertiesDsControlReg            0x00000500
#define Ucm_PtpOc_TimePropertiesDs1Reg                  0x00000504
#define Ucm_PtpOc_TimePropertiesDs2Reg                  0x00000508
#define Ucm_PtpOc_TimePropertiesDs3Reg                  0x0000050C
#define Ucm_PtpOc_TimePropertiesDs4Reg                  0x00000510
#define Ucm_PtpOc_TimePropertiesDs5Reg                  0x00000514
#define Ucm_PtpOc_TimePropertiesDs6Reg                  0x00000518
#define Ucm_PtpOc_TimePropertiesDs7Reg                  0x0000051C
#define Ucm_PtpOc_TimePropertiesDs8Reg                  0x00000520
#define Ucm_PtpOc_TimePropertiesDs9Reg                  0x00000524

namespace Ui {
class Ucm_PtpOcTab;
}

using namespace QtCharts;

class Ucm_PtpOcTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_PtpOcTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_PtpOcTab();

    int ptp_oc_resize(int height, int width);
    void ptp_oc_add_instance(unsigned int instance);
    int ptp_oc_disable(void);
    int ptp_oc_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_PtpOcTab *ui;

    // PTP Oc tab
    QTimer* ptp_oc_timer;

    QLineSeries* ptp_oc_offset_series;
    QChart* ptp_oc_offset_chart;
    QChartView* ptp_oc_offset_chart_view;
    unsigned int ptp_oc_offset_number_of_points;

    QLineSeries* ptp_oc_delay_series;
    QChart* ptp_oc_delay_chart;
    QChartView* ptp_oc_delay_chart_view;
    unsigned int ptp_oc_delay_number_of_points;


    void ptp_oc_read_values(void);
    void ptp_oc_write_values(void);

private slots:
    // PTP Oc tab
    void ptp_oc_read_values_button_clicked(void);
    void ptp_oc_write_values_button_clicked(void);
    void ptp_oc_auto_refresh_button_clicked(void);

};

#endif // UCM_PTPOC_H

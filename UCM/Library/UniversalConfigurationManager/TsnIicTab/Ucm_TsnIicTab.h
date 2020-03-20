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

#ifndef UCM_TSNIIC_H
#define UCM_TSNIIC_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_TsnIic_ControlReg                            0x00000000
#define Ucm_TsnIic_StatusReg                             0x00000004
#define Ucm_TsnIic_PolarityReg                           0x00000008
#define Ucm_TsnIic_VersionReg                            0x0000000C
#define Ucm_TsnIic_CableDelayReg                         0x00000020
#define Ucm_TsnIic_IrqReg                                0x00000030
#define Ucm_TsnIic_IrqMaskReg                            0x00000034
#define Ucm_TsnIic_EvtCountReg                           0x00000038
#define Ucm_TsnIic_CountReg                              0x00000040
#define Ucm_TsnIic_TimeValueLReg                         0x00000044
#define Ucm_TsnIic_TimeValueHReg                         0x00000048
#define Ucm_TsnIic_DataWidthReg                          0x0000004C
#define Ucm_TsnIic_DataReg                               0x00000050

#define Ucm_TsnIic_ControlReg                           0x00000000
#define Ucm_TsnIic_StatusReg                            0x00000004
#define Ucm_TsnIic_VersionReg                           0x0000000C
#define Ucm_TsnIic_TalkerControlReg                     0x00000100
#define Ucm_TsnIic_TalkerMac1Reg                        0x00000104
#define Ucm_TsnIic_TalkerMac2Reg                        0x00000108
#define Ucm_TsnIic_TalkerIdReg                          0x0000010C
#define Ucm_TsnIic_TalkerDelayReg                       0x00000110
#define Ucm_TsnIic_TalkerOffsetReg                      0x00000114
#define Ucm_TsnIic_TalkerIntervalReg                    0x00000118
#define Ucm_TsnIic_TalkerVlanReg                        0x0000011C
#define Ucm_TsnIic_TalkerAppIdReg                       0x0000014C
#define Ucm_TsnIic_TalkerAppDataReg                     0x00000150
#define Ucm_TsnIic_ListenerControlReg                   0x00000200
#define Ucm_TsnIic_ListenerDelayReg                     0x00000210
#define Ucm_TsnIic_ListenerVlanReg                      0x0000021C
#define Ucm_TsnIic_AnalyzerControlReg                   0x00000300
#define Ucm_TsnIic_AnalyzerIpReg                        0x00000304
#define Ucm_TsnIic_AnalyzerUdpPortReg                   0x00000308
#define Ucm_TsnIic_AnalyzerVlanReg                      0x0000031C
#define Ucm_TsnIic_AppLedControlReg                     0x00001000
#define Ucm_TsnIic_AppLedIdReg                          0x0000100C
#define Ucm_TsnIic_AppLedVlanReg                        0x0000101C


namespace Ui {
class Ucm_TsnIicTab;
}

class Ucm_TsnIicTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_TsnIicTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_TsnIicTab();

    int tsn_iic_resize(int height, int width);
    void tsn_iic_add_instance(unsigned int instance);
    int tsn_iic_disable(void);
    int tsn_iic_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_TsnIicTab *ui;

    // CLK Ts tab
    QTimer* tsn_iic_timer;

    void tsn_iic_read_values(void);
    void tsn_iic_write_values(void);

private slots:
    // CLK Ts tab
    void tsn_iic_read_values_button_clicked(void);
    void tsn_iic_write_values_button_clicked(void);
    void tsn_iic_auto_refresh_button_clicked(void);

};

#endif // UCM_TSNIIC_H

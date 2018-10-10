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

#ifndef UCM_REDTSN_H
#define UCM_REDTSN_H

#include <QWidget>
#include <QList>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

#define Ucm_RedTsn_ControlReg                           0x00000000
#define Ucm_RedTsn_StatusReg                            0x00000004
#define Ucm_RedTsn_VersionReg                           0x0000000C
#define Ucm_RedTsn_FrameCountControlReg                 0x00000010
#define Ucm_RedTsn_RxFrameCountAReg                     0x00000020
#define Ucm_RedTsn_RxErrCountAReg                       0x00000024
#define Ucm_RedTsn_TxFrameCountAReg                     0x00000030
#define Ucm_RedTsn_TxErrCountAReg                       0x00000034
#define Ucm_RedTsn_RxFrameCountBReg                     0x00000040
#define Ucm_RedTsn_RxErrCountBReg                       0x00000044
#define Ucm_RedTsn_TxFrameCountBReg                     0x00000050
#define Ucm_RedTsn_TxErrCountBReg                       0x00000054
#define Ucm_RedTsn_RxFrameCountCReg                     0x00000060
#define Ucm_RedTsn_RxErrCountCReg                       0x00000064
#define Ucm_RedTsn_TxFrameCountCReg                     0x00000070
#define Ucm_RedTsn_TxErrCountCReg                       0x00000074
#define Ucm_RedTsn_ConfigControlReg                     0x00000080
#define Ucm_RedTsn_ConfigModeReg                        0x00000084
#define Ucm_RedTsn_ConfigVlanReg                        0x00000088
#define Ucm_RedTsn_MacControlReg                        0x00000100
#define Ucm_RedTsn_Mac1_RegReg                          0x00000104
#define Ucm_RedTsn_Mac2_RegReg                          0x00000108
#define Ucm_RedTsn_CreditControlReg                     0x00000200
#define Ucm_RedTsn_CreditReg                            0x00000204
#define Ucm_RedTsn_CreditMinReg                         0x00000224
#define Ucm_RedTsn_CreditMaxReg                         0x00000244
#define Ucm_RedTsn_CreditPortCControlReg                0x00000270
#define Ucm_RedTsn_CreditPortCReg                       0x00000274
#define Ucm_RedTsn_CreditPortCMinReg                    0x00000278
#define Ucm_RedTsn_CreditPortCMaxReg                    0x0000027C
#define Ucm_RedTsn_MaxDataSizeControlReg                0x00000280
#define Ucm_RedTsn_MaxDataSizeReg                       0x00000284
#define Ucm_RedTsn_PrioPhaseControlReg                  0x00000300
#define Ucm_RedTsn_PrioVlanReg                          0x00000310
#define Ucm_RedTsn_PhaseSplPeriodReg                    0x00000340
#define Ucm_RedTsn_PhaseSplStartReg                     0x00000380
#define Ucm_RedTsn_PhaseSplStopReg                      0x000003A0

namespace Ui {
class Ucm_RedTsnTab;
}

class Ucm_RedTsnTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_RedTsnTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_RedTsnTab();

    int red_tsn_resize(int height, int width);
    void red_tsn_add_instance(unsigned int instance);
    int red_tsn_disable(void);
    int red_tsn_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_RedTsnTab *ui;

    // RED HsrPrp tab
    QList<bool> red_tsn_priority_support;
    QList<int> red_tsn_nr_of_priorities;
    QList<bool> red_tsn_phase_support;
    QList<bool> red_tsn_credit_support;
    QList<bool> red_tsn_cycle_support;
    QList<bool> red_tsn_maxsize_support;
    QList<bool> red_tsn_preemption_support;
    QList<bool> red_tsn_simple_scheduler_support;
    QTimer* red_tsn_timer;

    void red_tsn_capabilities(void);
    void red_tsn_read_values(void);
    void red_tsn_write_values(void);

private slots:
    // RED HsrPrp tab
    void red_tsn_instance_changed(int index);
    void red_tsn_read_values_button_clicked(void);
    void red_tsn_write_values_button_clicked(void);
    void red_tsn_auto_refresh_button_clicked(void);

};

#endif // UCM_REDTSN_H

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

#ifndef UCM_REDHSRPRP_H
#define UCM_REDHSRPRP_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_RedHsrPrp_ControlReg                        0x00000000
#define Ucm_RedHsrPrp_StatusReg                         0x00000004
#define Ucm_RedHsrPrp_VersionReg                        0x0000000C
#define Ucm_RedHsrPrp_FrameCountControlReg              0x00000010
#define Ucm_RedHsrPrp_RxFrameCountAReg                  0x00000020
#define Ucm_RedHsrPrp_RxErrCountAReg                    0x00000024
#define Ucm_RedHsrPrp_TxFrameCountAReg                  0x00000030
#define Ucm_RedHsrPrp_TxErrCountAReg                    0x00000034
#define Ucm_RedHsrPrp_RxFrameCountBReg                  0x00000040
#define Ucm_RedHsrPrp_RxErrCountBReg                    0x00000044
#define Ucm_RedHsrPrp_TxFrameCountBReg                  0x00000050
#define Ucm_RedHsrPrp_TxErrCountBReg                    0x00000054
#define Ucm_RedHsrPrp_RxFrameCountCReg                  0x00000060
#define Ucm_RedHsrPrp_RxErrCountCReg                    0x00000064
#define Ucm_RedHsrPrp_TxFrameCountCReg                  0x00000070
#define Ucm_RedHsrPrp_TxErrCountCReg                    0x00000074
#define Ucm_RedHsrPrp_ConfigControlReg                  0x00000080
#define Ucm_RedHsrPrp_ConfigModeReg                     0x00000084
#define Ucm_RedHsrPrp_ConfigVlanReg                     0x00000088
#define Ucm_RedHsrPrp_MacControlReg                     0x00000100
#define Ucm_RedHsrPrp_Mac1_RegReg                       0x00000104
#define Ucm_RedHsrPrp_Mac2_RegReg                       0x00000108

namespace Ui {
class Ucm_RedHsrPrpTab;
}

class Ucm_RedHsrPrpTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_RedHsrPrpTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_RedHsrPrpTab();

    int red_hsrprp_resize(int height, int width);
    void red_hsrprp_add_instance(unsigned int instance);
    int red_hsrprp_disable(void);
    int red_hsrprp_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_RedHsrPrpTab *ui;

    // RED HsrPrp tab
    QTimer* red_hsrprp_timer;

    void red_hsrprp_read_values(void);
    void red_hsrprp_write_values(void);

private slots:
    // RED HsrPrp tab
    void red_hsrprp_read_values_button_clicked(void);
    void red_hsrprp_write_values_button_clicked(void);
    void red_hsrprp_auto_refresh_button_clicked(void);

};

#endif // UCM_REDHSRPRP_H

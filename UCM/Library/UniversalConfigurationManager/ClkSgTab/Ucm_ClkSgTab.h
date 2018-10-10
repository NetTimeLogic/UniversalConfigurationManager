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

#ifndef UCM_CLKSG_H
#define UCM_CLKSG_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_ClkSg_ControlReg                            0x00000000
#define Ucm_ClkSg_StatusReg                             0x00000004
#define Ucm_ClkSg_PolarityReg                           0x00000008
#define Ucm_ClkSg_VersionReg                            0x0000000C
#define Ucm_ClkSg_CableDelayReg                         0x00000020
#define Ucm_ClkSg_IrqReg                                0x00000030
#define Ucm_ClkSg_IrqMaskReg                            0x00000034
#define Ucm_ClkSg_StartTimeValueLReg                    0x00000040
#define Ucm_ClkSg_StartTimeValueHReg                    0x00000044
#define Ucm_ClkSg_PulseWidthValueLReg                   0x00000048
#define Ucm_ClkSg_PulseWidthValueHReg                   0x0000004C
#define Ucm_ClkSg_PeriodValueLReg                       0x00000050
#define Ucm_ClkSg_PeriodValueHReg                       0x00000054
#define Ucm_ClkSg_RepeatCountReg                        0x00000058

namespace Ui {
class Ucm_ClkSgTab;
}

class Ucm_ClkSgTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_ClkSgTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_ClkSgTab();

    int clk_sg_resize(int height, int width);
    void clk_sg_add_instance(unsigned int instance);
    int clk_sg_disable(void);
    int clk_sg_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_ClkSgTab *ui;

    // CLK Sg tab
    QTimer* clk_sg_timer;

    void clk_sg_read_values(void);
    void clk_sg_write_values(void);

private slots:
    // CLK Sg tab
    void clk_sg_read_values_button_clicked(void);
    void clk_sg_write_values_button_clicked(void);
    void clk_sg_auto_refresh_button_clicked(void);

};

#endif // UCM_CLKSG_H

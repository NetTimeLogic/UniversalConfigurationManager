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

#ifndef UCM_CLKTS_H
#define UCM_CLKTS_H

#include <QWidget>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_ClkTsTab;
}

class Ucm_ClkTsTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_ClkTsTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_ClkTsTab();

    int clk_ts_resize(int height, int width);
    void clk_ts_add_instance(unsigned int instance);
    int clk_ts_disable(void);
    int clk_ts_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_ClkTsTab *ui;

    // CLK Ts tab
    QTimer* clk_ts_timer;

    void clk_ts_read_values(void);
    void clk_ts_write_values(void);

private slots:
    // CLK Ts tab
    void clk_ts_read_values_button_clicked(void);
    void clk_ts_write_values_button_clicked(void);
    void clk_ts_auto_refresh_button_clicked(void);

};

#endif // UCM_CLKTS_H

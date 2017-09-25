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

#ifndef UCM_TODMASTER_H
#define UCM_TODMASTER_H

#include <QWidget>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_TodMasterTab;
}

class Ucm_TodMasterTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_TodMasterTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_TodMasterTab();

    int tod_master_resize(int height, int width);
    void tod_master_add_instance(unsigned int instance);
    int tod_master_disable(void);
    int tod_master_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_TodMasterTab *ui;

    // TOD Master tab
    QTimer* tod_master_timer;

    void tod_master_read_values(void);
    void tod_master_write_values(void);

private slots:
    // TOD Master tab
    void tod_master_read_values_button_clicked(void);
    void tod_master_write_values_button_clicked(void);
    void tod_master_auto_refresh_button_clicked(void);

};

#endif // UCM_TODMASTER_H

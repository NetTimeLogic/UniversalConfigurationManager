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

#ifndef UCM_PPSSLAVE_H
#define UCM_PPSSLAVE_H

#include <QWidget>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_PpsSlaveTab;
}

class Ucm_PpsSlaveTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_PpsSlaveTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_PpsSlaveTab();

    int pps_slave_resize(int height, int width);
    void pps_slave_add_instance(unsigned int instance);
    int pps_slave_disable(void);
    int pps_slave_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_PpsSlaveTab *ui;

    // PPS Slave tab
    QTimer* pps_slave_timer;

    void pps_slave_read_values(void);
    void pps_slave_write_values(void);

private slots:
    // PPS Slave tab
    void pps_slave_read_values_button_clicked(void);
    void pps_slave_write_values_button_clicked(void);
    void pps_slave_auto_refresh_button_clicked(void);

};

#endif // UCM_PPSSLAVE_H

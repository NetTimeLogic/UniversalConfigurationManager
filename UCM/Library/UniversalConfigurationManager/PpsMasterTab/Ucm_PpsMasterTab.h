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

#ifndef UCM_PPSMASTER_H
#define UCM_PPSMASTER_H

#include <QWidget>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

#define Ucm_PpsMaster_ControlReg                        0x00000000
#define Ucm_PpsMaster_StatusReg                         0x00000004
#define Ucm_PpsMaster_PolarityReg                       0x00000008
#define Ucm_PpsMaster_VersionReg                        0x0000000C
#define Ucm_PpsMaster_PulseWidthReg                     0x00000010

namespace Ui {
class Ucm_PpsMasterTab;
}

class Ucm_PpsMasterTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_PpsMasterTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_PpsMasterTab();

    int pps_master_resize(int height, int width);
    void pps_master_add_instance(unsigned int instance);
    int pps_master_disable(void);
    int pps_master_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_PpsMasterTab *ui;

    // PPS Master tab
    QTimer* pps_master_timer;

    void pps_master_read_values(void);
    void pps_master_write_values(void);

private slots:
    // PPS Master tab
    void pps_master_read_values_button_clicked(void);
    void pps_master_write_values_button_clicked(void);
    void pps_master_auto_refresh_button_clicked(void);

};

#endif // UCM_PPSMASTER_H

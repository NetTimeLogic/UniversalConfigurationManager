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

#ifndef UCM_IRIGMASTER_H
#define UCM_IRIGMASTER_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_IrigMaster_ControlReg                       0x00000000
#define Ucm_IrigMaster_StatusReg                        0x00000004
#define Ucm_IrigMaster_VersionReg                       0x0000000C
#define Ucm_IrigMaster_CorrectionReg                    0x00000010
#define Ucm_IrigMaster_ControlBitsReg                   0x00000014

namespace Ui {
class Ucm_IrigMasterTab;
}

class Ucm_IrigMasterTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_IrigMasterTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_IrigMasterTab();

    int irig_master_resize(int height, int width);
    void irig_master_add_instance(unsigned int instance);
    int irig_master_disable(void);
    int irig_master_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_IrigMasterTab *ui;

    // IRIG Master tab
    QTimer* irig_master_timer;

    void irig_master_read_values(void);
    void irig_master_write_values(void);

private slots:
    // IRIG Master tab
    void irig_master_read_values_button_clicked(void);
    void irig_master_write_values_button_clicked(void);
    void irig_master_auto_refresh_button_clicked(void);

};

#endif // UCM_IRIGMASTER_H

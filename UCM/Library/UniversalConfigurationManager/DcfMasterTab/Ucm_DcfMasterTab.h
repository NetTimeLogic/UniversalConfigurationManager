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

#ifndef UCM_DCFMASTER_H
#define UCM_DCFMASTER_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_DcfMaster_ControlReg                        0x00000000
#define Ucm_DcfMaster_StatusReg                         0x00000004
#define Ucm_DcfMaster_VersionReg                        0x0000000C
#define Ucm_DcfMaster_CorrectionReg                     0x00000010

namespace Ui {
class Ucm_DcfMasterTab;
}

class Ucm_DcfMasterTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_DcfMasterTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_DcfMasterTab();

    int dcf_master_resize(int height, int width);
    void dcf_master_add_instance(unsigned int instance);
    int dcf_master_disable(void);
    int dcf_master_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_DcfMasterTab *ui;

    // DCF Master tab
    QTimer* dcf_master_timer;

    void dcf_master_read_values(void);
    void dcf_master_write_values(void);

private slots:
    // DCF Master tab
    void dcf_master_read_values_button_clicked(void);
    void dcf_master_write_values_button_clicked(void);
    void dcf_master_auto_refresh_button_clicked(void);

};

#endif // UCM_DCFMASTER_H

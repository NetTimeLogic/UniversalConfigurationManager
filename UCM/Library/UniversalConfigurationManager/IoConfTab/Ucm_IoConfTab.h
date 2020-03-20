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

#ifndef UCM_IOCONF_H
#define UCM_IOCONF_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_IoConf_OutputDataReg                          0x00000000
#define Ucm_IoConf_OutputEnableReg                        0x00000004
#define Ucm_IoConf_InputDataReg                           0x00000008

namespace Ui {
class Ucm_IoConfTab;
}

class Ucm_IoConfTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_IoConfTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_IoConfTab();

    int io_conf_resize(int height, int width);
    void io_conf_add_instance(unsigned int instance);
    int io_conf_disable(void);
    int io_conf_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_IoConfTab *ui;

    // CLK Sg tab
    QTimer* io_conf_timer;

    void io_conf_read_values(void);
    void io_conf_write_values(void);

private slots:
    // CLK Sg tab
    void io_conf_read_values_button_clicked(void);
    void io_conf_write_values_button_clicked(void);
    void io_conf_auto_refresh_button_clicked(void);

};

#endif // UCM_IOCONF_H

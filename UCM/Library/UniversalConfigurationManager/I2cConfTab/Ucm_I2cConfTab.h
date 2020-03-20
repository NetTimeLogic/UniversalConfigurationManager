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

#ifndef UCM_I2CCONF_H
#define UCM_I2CCONF_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_I2cConf_ControlReg                            0x00000000
#define Ucm_I2cConf_ChipAddrReg                           0x00000004
#define Ucm_I2cConf_RegAddrReg                            0x00000008
#define Ucm_I2cConf_DataReg                               0x0000000C

namespace Ui {
class Ucm_I2cConfTab;
}

class Ucm_I2cConfTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_I2cConfTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_I2cConfTab();

    int i2c_conf_resize(int height, int width);
    void i2c_conf_add_instance(unsigned int instance);
    int i2c_conf_disable(void);
    int i2c_conf_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_I2cConfTab *ui;

    // CLK Sg tab
    QTimer* i2c_conf_timer;

    void i2c_conf_read_values(void);
    void i2c_conf_write_values(void);

private slots:
    // CLK Sg tab
    void i2c_conf_read_values_button_clicked(void);
    void i2c_conf_write_values_button_clicked(void);
    void i2c_conf_auto_refresh_button_clicked(void);

};

#endif // UCM_I2CCONF_H

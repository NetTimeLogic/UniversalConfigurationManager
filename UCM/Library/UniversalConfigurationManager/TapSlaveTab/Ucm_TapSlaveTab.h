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

#ifndef UCM_TAPSLAVE_H
#define UCM_TAPSLAVE_H

#include <QWidget>
#include <Ucm_UniversalConfigurationManager.h>

class Ucm_UniversalConfigurationManager;

#define Ucm_TapSlave_ControlReg                         0x00000000
#define Ucm_TapSlave_StatusReg                          0x00000004
#define Ucm_TapSlave_FilterModeReg                      0x00000008
#define Ucm_TapSlave_VersionReg                         0x0000000C
#define Ucm_TapSlave_NrOfFramesReg                      0x00000010

namespace Ui {
class Ucm_TapSlaveTab;
}

class Ucm_TapSlaveTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_TapSlaveTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_TapSlaveTab();

    int tap_slave_resize(int height, int width);
    void tap_slave_add_instance(unsigned int instance);
    int tap_slave_disable(void);
    int tap_slave_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_TapSlaveTab *ui;

    // TAP Slave tab
    QTimer* tap_slave_timer;

    QLineSeries* tap_slave_frame_rate_series;
    QChart* tap_slave_frame_rate_chart;
    QChartView* tap_slave_frame_rate_chart_view;
    unsigned int tap_slave_frame_rate_number_of_points;
    unsigned int tap_slave_old_nr_of_frames;

    void tap_slave_read_values(void);
    void tap_slave_write_values(void);

private slots:
    // TAP Slave tab
    void tap_slave_read_values_button_clicked(void);
    void tap_slave_write_values_button_clicked(void);
    void tap_slave_auto_refresh_button_clicked(void);

};

#endif // UCM_TAPSLAVE_H

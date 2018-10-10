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

#ifndef UCM_CLKCLOCK_H
#define UCM_CLKCLOCK_H

#include <QScrollArea>
#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QValueAxis>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

#define Ucm_ClkClock_ControlReg                         0x00000000
#define Ucm_ClkClock_StatusReg                          0x00000004
#define Ucm_ClkClock_SelectReg                          0x00000008
#define Ucm_ClkClock_VersionReg                         0x0000000C
#define Ucm_ClkClock_TimeValueLReg                      0x00000010
#define Ucm_ClkClock_TimeValueHReg                      0x00000014
#define Ucm_ClkClock_TimeAdjValueLReg                   0x00000020
#define Ucm_ClkClock_TimeAdjValueHReg                   0x00000024
#define Ucm_ClkClock_OffsetAdjValueReg                  0x00000030
#define Ucm_ClkClock_OffsetAdjIntervalReg               0x00000034
#define Ucm_ClkClock_DriftAdjValueReg                   0x00000040
#define Ucm_ClkClock_DriftAdjIntervalReg                0x00000044
#define Ucm_ClkClock_InSyncThresholdReg                 0x00000050
#define Ucm_ClkClock_ServoOffsetFactorPReg              0x00000060
#define Ucm_ClkClock_ServoOffsetFactorIReg              0x00000064
#define Ucm_ClkClock_ServoDriftFactorPReg               0x00000068
#define Ucm_ClkClock_ServoDriftFactorIReg               0x0000006C
#define Ucm_ClkClock_StatusOffset_Reg_Con               0x00000070
#define Ucm_ClkClock_StatusDrift_Reg_Con                0x00000074

namespace Ui {
class Ucm_ClkClockTab;
}

using namespace QtCharts;

class Ucm_ClkClockTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_ClkClockTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_ClkClockTab();

    int clk_clock_resize(int height, int width);
    void clk_clock_add_instance(unsigned int instance);
    int clk_clock_disable(void);
    int clk_clock_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_ClkClockTab *ui;

    // CLK Clock tab
    QTimer* clk_clock_timer;

    QLineSeries* clk_clock_offset_series;
    QChart* clk_clock_offset_chart;
    QChartView* clk_clock_offset_chart_view;
    unsigned int clk_clock_offset_number_of_points;

    QLineSeries* clk_clock_drift_series;
    QChart* clk_clock_drift_chart;
    QChartView* clk_clock_drift_chart_view;
    unsigned int clk_clock_drift_number_of_points;
    
    void clk_clock_read_values(void);
    void clk_clock_write_values(void);

private slots:
    // CLK Clock tab
    void clk_clock_read_values_button_clicked(void);
    void clk_clock_write_values_button_clicked(void);
    void clk_clock_auto_refresh_button_clicked(void);

};

#endif // UCM_CLKCLOCK_H

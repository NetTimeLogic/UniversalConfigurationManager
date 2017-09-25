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

#ifndef UCM_PTPOC_H
#define UCM_PTPOC_H

#include <QScrollArea>
#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QValueAxis>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_PtpOcTab;
}

using namespace QtCharts;

class Ucm_PtpOcTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_PtpOcTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_PtpOcTab();

    int ptp_oc_resize(int height, int width);
    void ptp_oc_add_instance(unsigned int instance);
    int ptp_oc_disable(void);
    int ptp_oc_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_PtpOcTab *ui;

    // PTP Oc tab
    QTimer* ptp_oc_timer;

    QLineSeries* ptp_oc_offset_series;
    QChart* ptp_oc_offset_chart;
    QChartView* ptp_oc_offset_chart_view;
    unsigned int ptp_oc_offset_number_of_points;

    QLineSeries* ptp_oc_delay_series;
    QChart* ptp_oc_delay_chart;
    QChartView* ptp_oc_delay_chart_view;
    unsigned int ptp_oc_delay_number_of_points;


    void ptp_oc_read_values(void);
    void ptp_oc_write_values(void);

private slots:
    // PTP Oc tab
    void ptp_oc_read_values_button_clicked(void);
    void ptp_oc_write_values_button_clicked(void);
    void ptp_oc_auto_refresh_button_clicked(void);

};

#endif // UCM_PTPOC_H

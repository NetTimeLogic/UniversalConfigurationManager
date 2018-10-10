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

#include "Ucm_ClkClockTab.h"
#include "ui_Ucm_ClkClockTab.h"

Ucm_ClkClockTab::Ucm_ClkClockTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_ClkClockTab();
    ui->setupUi(this);

    clk_clock_timer = new QTimer(this);
    clk_clock_timer->stop();
    connect(ui->ClkClockReadValuesButton, SIGNAL(clicked()), this, SLOT(clk_clock_read_values_button_clicked()));
    connect(ui->ClkClockWriteValuesButton, SIGNAL(clicked()), this, SLOT(clk_clock_write_values_button_clicked()));
    connect(ui->ClkClockAutoRefreshButton, SIGNAL(clicked()), this, SLOT(clk_clock_auto_refresh_button_clicked()));
    connect(clk_clock_timer, SIGNAL(timeout()), this, SLOT(clk_clock_read_values_button_clicked()));
    
    clk_clock_offset_series = new QLineSeries();
    clk_clock_offset_number_of_points = 0;

    clk_clock_offset_chart = new QChart();
    clk_clock_offset_chart->setContentsMargins(0, 0, 0, 0);
    clk_clock_offset_chart->setBackgroundRoundness(0);
    clk_clock_offset_chart->setBackgroundBrush(Qt::white);
    clk_clock_offset_chart->legend()->hide();
    clk_clock_offset_chart->addSeries(clk_clock_offset_series);

    QValueAxis* clk_clock_offset_chart_x_axis = new QValueAxis;
    clk_clock_offset_chart_x_axis->setLabelFormat("%i");
    clk_clock_offset_chart_x_axis->setTickCount(10);
    clk_clock_offset_chart_x_axis->setMin(0);
    clk_clock_offset_chart_x_axis->setMax(49);
    clk_clock_offset_chart->addAxis(clk_clock_offset_chart_x_axis, Qt::AlignBottom);
    clk_clock_offset_series->attachAxis(clk_clock_offset_chart_x_axis);

    QValueAxis* clk_clock_offset_chart_y_axis = new QValueAxis;
    clk_clock_offset_chart_y_axis->setLabelFormat("%i");
    clk_clock_offset_chart_y_axis->setTickCount(5);
    clk_clock_offset_chart_y_axis->setMin(-500);
    clk_clock_offset_chart_y_axis->setMax(500);
    clk_clock_offset_chart->addAxis(clk_clock_offset_chart_y_axis, Qt::AlignLeft);
    clk_clock_offset_series->attachAxis(clk_clock_offset_chart_y_axis);

    clk_clock_offset_chart_view = new QChartView(clk_clock_offset_chart);
    clk_clock_offset_chart_view->setRenderHint(QPainter::Antialiasing);
    ui->ClkClockOffsetChartLayout->addWidget(clk_clock_offset_chart_view, 0, 0);

    clk_clock_drift_series = new QLineSeries();
    clk_clock_drift_number_of_points = 0;

    clk_clock_drift_chart = new QChart();
    clk_clock_drift_chart->setContentsMargins(0, 0, 0, 0);
    clk_clock_drift_chart->setBackgroundRoundness(0);
    clk_clock_drift_chart->setBackgroundBrush(Qt::white);
    clk_clock_drift_chart->legend()->hide();
    clk_clock_drift_chart->addSeries(clk_clock_drift_series);

    QValueAxis* clk_clock_drift_chart_x_axis = new QValueAxis;
    clk_clock_drift_chart_x_axis->setLabelFormat("%i");
    clk_clock_drift_chart_x_axis->setTickCount(10);
    clk_clock_drift_chart_x_axis->setMin(0);
    clk_clock_drift_chart_x_axis->setMax(49);
    clk_clock_drift_chart->addAxis(clk_clock_drift_chart_x_axis, Qt::AlignBottom);
    clk_clock_drift_series->attachAxis(clk_clock_drift_chart_x_axis);

    QValueAxis* clk_clock_drift_chart_y_axis = new QValueAxis;
    clk_clock_drift_chart_y_axis->setLabelFormat("%i");
    clk_clock_drift_chart_y_axis->setTickCount(5);
    clk_clock_drift_chart_y_axis->setMin(0);
    clk_clock_drift_chart_y_axis->setMax(500);
    clk_clock_drift_chart->addAxis(clk_clock_drift_chart_y_axis, Qt::AlignLeft);
    clk_clock_drift_series->attachAxis(clk_clock_drift_chart_y_axis);

    clk_clock_drift_chart_view = new QChartView(clk_clock_drift_chart);
    clk_clock_drift_chart_view->setRenderHint(QPainter::Antialiasing);
    ui->ClkClockDriftChartLayout->addWidget(clk_clock_drift_chart_view, 0, 0);}

Ucm_ClkClockTab::~Ucm_ClkClockTab()
{
    clk_clock_timer->stop();
    delete ui;
    delete clk_clock_timer;
    delete clk_clock_offset_series;
    delete clk_clock_offset_chart_view;
    delete clk_clock_offset_chart;
    delete clk_clock_drift_series;
    delete clk_clock_drift_chart_view;
    delete clk_clock_drift_chart;
}

int Ucm_ClkClockTab::clk_clock_resize(int height, int width)
{
    int height_delta = 0;
    int width_delta = 0;

    if (height <= Ucm_MainHeight)
    {
        height_delta = (height-Ucm_MainHeight);
    }
    if (width <= Ucm_MainWidth)
    {
        width_delta = (width-Ucm_MainWidth);
    }

    ui->ClkClockDriftChartValue->setFixedHeight(210);
    ui->ClkClockDriftChartValue->setFixedWidth(450+(width_delta/2));
    ui->ClkClockDriftChartLabel->setFixedWidth(450+(width_delta/2));

    ui->ClkClockOffsetChartValue->move(910+(width_delta/2), 20);
    ui->ClkClockOffsetChartLabel->move(910+(width_delta/2), 30);

    ui->ClkClockOffsetChartValue->setFixedHeight(210);
    ui->ClkClockOffsetChartValue->setFixedWidth(450+(width_delta/2));
    ui->ClkClockOffsetChartLabel->setFixedWidth(450+(width_delta/2));


    updateGeometry();

    return 0;
}

void Ucm_ClkClockTab::clk_clock_add_instance(unsigned int instance)
{
    ui->ClkClockInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_ClkClockTab::clk_clock_enable(void)
{
    return 0;
}

int Ucm_ClkClockTab::clk_clock_disable(void)
{
    clk_clock_timer->stop();
    ui->ClkClockAutoRefreshButton->setText("Start Refresh");
    ui->ClkClockInstanceComboBox->setEnabled(true);
    ui->ClkClockReadValuesButton->setEnabled(true);
    ui->ClkClockWriteValuesButton->setEnabled(true);
    ui->ClkClockInstanceComboBox->clear();

    ui->ClkClockSecondsValue->setText("NA");
    ui->ClkClockNanosecondsValue->setText("NA");
    ui->ClkClockTimeAdjCheckBox->setChecked(false);
    ui->ClkClockInSyncValue->setText("NA");
    ui->ClkClockInHoldoverValue->setText("NA");
    ui->ClkClockInSyncThresholdValue->setText("NA");
    ui->ClkClockSourceValue->setCurrentText("NA");
    ui->ClkClockEnableCheckBox->setChecked(false);
    ui->ClkClockVersionValue->setText("NA");
    ui->ClkClockDriftValue->setText("NA");
    ui->ClkClockDriftIntervalValue->setText("NA");
    ui->ClkClockDriftAdjCheckBox->setChecked(false);
    ui->ClkClockOffsetValue->setText("NA");
    ui->ClkClockOffsetIntervalValue->setText("NA");
    ui->ClkClockOffsetAdjCheckBox->setChecked(false);

    ui->ClkClockPiOffsetMulPValue->setText("NA");
    ui->ClkClockPiOffsetDivPValue->setText("NA");
    ui->ClkClockPiOffsetMulIValue->setText("NA");
    ui->ClkClockPiOffsetDivIValue->setText("NA");
    ui->ClkClockPiDriftMulPValue->setText("NA");
    ui->ClkClockPiDriftDivPValue->setText("NA");
    ui->ClkClockPiDriftMulIValue->setText("NA");
    ui->ClkClockPiDriftDivIValue->setText("NA");
    ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);

    ui->ClkClockDateValue->setText("NA");

    ui->ClkClockCorrectedOffsetValue->setText("NA");
    ui->ClkClockCorrectedDriftValue->setText("NA");
            
    clk_clock_offset_number_of_points = 0;
    clk_clock_offset_series-> clear();

    clk_clock_drift_number_of_points = 0;
    clk_clock_drift_series->clear();

    return 0;
}

void Ucm_ClkClockTab::clk_clock_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    int temp_drift;
    int temp_offset;
    int temp_min = 0;
    int temp_max = 0;
    QString temp_string;

    temp_string = ui->ClkClockInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i <  ucm->core_config.size(); i++)
    {
        if (( ucm->core_config.at(i).core_type == Ucm_CoreConfig_ClkClockCoreType) && ( ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr =  ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == ( ucm->core_config.size()-1))
        {
            ui->ClkClockSecondsValue->setText("NA");
            ui->ClkClockNanosecondsValue->setText("NA");
            ui->ClkClockTimeAdjCheckBox->setChecked(false);
            ui->ClkClockInSyncValue->setText("NA");
            ui->ClkClockInHoldoverValue->setText("NA");
            ui->ClkClockInSyncThresholdValue->setText("NA");
            ui->ClkClockSourceValue->setCurrentText("NA");
            ui->ClkClockEnableCheckBox->setChecked(false);
            ui->ClkClockVersionValue->setText("NA");
            ui->ClkClockDriftValue->setText("NA");
            ui->ClkClockDriftIntervalValue->setText("NA");
            ui->ClkClockDriftAdjCheckBox->setChecked(false);
            ui->ClkClockOffsetValue->setText("NA");
            ui->ClkClockOffsetIntervalValue->setText("NA");
            ui->ClkClockOffsetAdjCheckBox->setChecked(false);
            ui->ClkClockPiOffsetMulPValue->setText("NA");
            ui->ClkClockPiOffsetDivPValue->setText("NA");
            ui->ClkClockPiOffsetMulIValue->setText("NA");
            ui->ClkClockPiOffsetDivIValue->setText("NA");
            ui->ClkClockPiDriftMulPValue->setText("NA");
            ui->ClkClockPiDriftDivPValue->setText("NA");
            ui->ClkClockPiDriftMulIValue->setText("NA");
            ui->ClkClockPiDriftDivIValue->setText("NA");
            ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
            ui->ClkClockCorrectedOffsetValue->setText("NA");
            ui->ClkClockCorrectedDriftValue->setText("NA");
            ui->ClkClockDateValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkClockEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->ClkClockEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->ClkClockEnableCheckBox->setChecked(false);
    }

    temp_data = 0x40000000;
    if(true == ui->ClkClockEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001;
    }
    if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_ControlReg, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_ControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // seconds
                    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_TimeValueHReg, temp_data))
                    {
                        ui->ClkClockSecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->ClkClockSecondsValue->setText("NA");
                    }

                    // date
                    QDateTime temp_date;
                    temp_date.setTime_t(temp_data);
                    ui->ClkClockDateValue->setText(temp_date.toUTC().toString("dd.MM.yyyy hh:mm:ss"));


                    // nanoseconds
                    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_TimeValueLReg, temp_data))
                    {
                        ui->ClkClockNanosecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->ClkClockNanosecondsValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->ClkClockSecondsValue->setText("NA");
                    ui->ClkClockNanosecondsValue->setText("NA");
                }

            }
            else
            {
                ui->ClkClockSecondsValue->setText("NA");
                ui->ClkClockNanosecondsValue->setText("NA");
            }
        }
    }
    else
    {
        ui->ClkClockSecondsValue->setText("NA");
        ui->ClkClockNanosecondsValue->setText("NA");
    }

    // in sync
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_StatusReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkClockInSyncValue->setText("false");
        }
        else
        {
            ui->ClkClockInSyncValue->setText("true");
        }

        if ((temp_data & 0x00000002) == 0)
        {
            ui->ClkClockInHoldoverValue->setText("false");
        }
        else
        {
            ui->ClkClockInHoldoverValue->setText("true");
        }
    }
    else
    {
        ui->ClkClockInSyncValue->setText("NA");
        ui->ClkClockInHoldoverValue->setText("NA");
    }

    // in sync Threshold
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_InSyncThresholdReg, temp_data))
    {
        ui->ClkClockInSyncThresholdValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockInSyncThresholdValue->setText("NA");
    }

    // offset
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, temp_data))
    {
        int temp_offset = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * temp_offset;
        }
        ui->ClkClockOffsetValue->setText(QString::number(temp_offset));

        if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, temp_data))
        {
            ui->ClkClockOffsetIntervalValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkClockOffsetValue->setText("NA");
            ui->ClkClockOffsetIntervalValue->setText("NA");
            ui->ClkClockOffsetAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->ClkClockOffsetValue->setText("NA");
        ui->ClkClockOffsetIntervalValue->setText("NA");
        ui->ClkClockOffsetAdjCheckBox->setChecked(false);
    }

    // drift
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_DriftAdjValueReg, temp_data))
    {
        int temp_drift = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_drift = -1 * temp_drift;
        }
        ui->ClkClockDriftValue->setText(QString::number(temp_drift));

        if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, temp_data))
        {
            ui->ClkClockDriftIntervalValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkClockDriftValue->setText("NA");
            ui->ClkClockDriftIntervalValue->setText("NA");
            ui->ClkClockDriftAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->ClkClockDriftValue->setText("NA");
        ui->ClkClockDriftIntervalValue->setText("NA");
        ui->ClkClockDriftAdjCheckBox->setChecked(false);
    }

    // source
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_SelectReg, temp_data))
    {
        switch ((temp_data >> 16) & 0x0000FFFF)
        {
        case 0x00000000:
            ui->ClkClockSourceValue->setCurrentText("NONE");
            break;
        case 0x0000001:
            ui->ClkClockSourceValue->setCurrentText("TOD");
            break;
        case 0x00000002:
            ui->ClkClockSourceValue->setCurrentText("IRIG");
            break;
        case 0x00000003:
            ui->ClkClockSourceValue->setCurrentText("PPS");
            break;
        case 0x00000004:
            ui->ClkClockSourceValue->setCurrentText("PTP");
            break;
        case 0x00000005:
            ui->ClkClockSourceValue->setCurrentText("RTC");
            break;
        case 0x00000006:
            ui->ClkClockSourceValue->setCurrentText("DCF");
            break;
        case 0x000000FE:
            ui->ClkClockSourceValue->setCurrentText("REG");
            break;
        case 0x000000FF:
            ui->ClkClockSourceValue->setCurrentText("EXT");
            break;
        default:
            ui->ClkClockSourceValue->setCurrentText("NA");
            break;
        }

    }
    else
    {
        ui->ClkClockSourceValue->setCurrentText("NA");
    }

    // PI servo parameters
    // we consider them Write Only since after the calculations we can not reproduce the value again

    
    // corrected offset
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_StatusOffset_Reg_Con, temp_data))
    {
        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * (temp_data & 0x7FFFFFFF);
        }
        else
        {
            temp_offset = (temp_data & 0x7FFFFFFF);
        }
        
        // limit to one second in display
        if (temp_offset >= 1000000000)
        {
            temp_offset = 1000000000;
        }
        else if (temp_offset <= -1000000000)
        {
            temp_offset = -1000000000;
        }

        ui->ClkClockCorrectedOffsetValue->setText(QString::number(temp_offset));

        if (true == clk_clock_timer->isActive())
        {

            clk_clock_offset_series->append(clk_clock_offset_number_of_points, temp_offset);

            if (clk_clock_offset_number_of_points < 50)
            {
                clk_clock_offset_number_of_points++;
            }
            else
            {
                for (int j = 1; j < clk_clock_offset_series->count(); j++)
                {
                    QPointF temp_point = clk_clock_offset_series->at(j);
                    clk_clock_offset_series->replace(j, (j-1), temp_point.y());
                }
                clk_clock_offset_series->remove(0);
            }

            temp_min = 0;
            temp_max = 0;
            for (int j = 0; j < clk_clock_offset_series->count(); j++)
            {
                QPointF temp_point = clk_clock_offset_series->at(j);
                if (temp_min > temp_point.y())
                {
                    temp_min = temp_point.y();
                }
                if (temp_max < temp_point.y())
                {
                    temp_max = temp_point.y();
                }
            }
            temp_max = ((temp_max + 100)/100)*100;
            temp_min = ((temp_min - 100)/100)*100;
            if (temp_max > 1000000000)
            {
                temp_max = 1000000000;
            }
            if (temp_min < -1000000000)
            {
                temp_min = -1000000000;
            }
            clk_clock_offset_chart->axisY()->setMin(temp_min);
            clk_clock_offset_chart->axisY()->setMax(temp_max);

            clk_clock_offset_chart->show();
        }
    }
    else
    {
        ui->ClkClockCorrectedOffsetValue->setText("NA");
    }

    // corrected drift
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_StatusDrift_Reg_Con, temp_data))
    {
        if ((temp_data & 0x80000000) != 0)
        {
            temp_drift = -1 * (temp_data & 0x7FFFFFFF);
        }
        else
        {
            temp_drift = (temp_data & 0x7FFFFFFF);
        }
        
        // limit to 1000ppm in display
        if (temp_drift >= 1000000)
        {
            temp_drift = 1000000;
        }
        else if (temp_drift <= -1000000)
        {
            temp_drift = -1000000;
        }

        ui->ClkClockCorrectedDriftValue->setText(QString::number(temp_drift));

        if (true == clk_clock_timer->isActive())
        {
            clk_clock_drift_series->append(clk_clock_drift_number_of_points, temp_drift);

            if (clk_clock_drift_number_of_points < 50)
            {
                clk_clock_drift_number_of_points++;
            }
            else
            {
                for (int j = 1; j < clk_clock_drift_series->count(); j++)
                {
                    QPointF temp_point = clk_clock_drift_series->at(j);
                    clk_clock_drift_series->replace(j, (j-1), temp_point.y());
                }
                clk_clock_drift_series->remove(0);
            }

            
            temp_min = 1000000;
            temp_max = -1000000;
            for (int j = 0; j < clk_clock_drift_series->count(); j++)
            {
                QPointF temp_point = clk_clock_drift_series->at(j);
                if (temp_min > temp_point.y())
                {
                    temp_min = temp_point.y();
                }
                if (temp_max < temp_point.y())
                {
                    temp_max = temp_point.y();
                }
            }
            temp_max = ((temp_max + 100)/100)*100;
            temp_min = ((temp_min - 100)/100)*100;
            if (temp_max > 1000000)
            {
                temp_max = 1000000;
            }
            if (temp_min < -1000000)
            {
                temp_min = -1000000;
            }
            clk_clock_drift_chart->axisY()->setMin(temp_min);
            clk_clock_drift_chart->axisY()->setMax(temp_max);

            clk_clock_drift_chart->show();
        }
    }
    else
    {
        ui->ClkClockCorrectedDriftValue->setText("NA");
    }   

    // version
    if (0 ==  ucm->com_lib.read_reg(temp_addr + Ucm_ClkClock_VersionReg, temp_data))
    {
        ui->ClkClockVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->ClkClockVersionValue->setText("NA");
    }
}

void Ucm_ClkClockTab::clk_clock_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->ClkClockInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i <  ucm->core_config.size(); i++)
    {
        if (( ucm->core_config.at(i).core_type == Ucm_CoreConfig_ClkClockCoreType) && ( ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr =  ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == ( ucm->core_config.size()-1))
        {
            ui->ClkClockSecondsValue->setText("NA");
            ui->ClkClockNanosecondsValue->setText("NA");
            ui->ClkClockTimeAdjCheckBox->setChecked(false);
            ui->ClkClockInSyncValue->setText("NA");
            ui->ClkClockInHoldoverValue->setText("NA");
            ui->ClkClockInSyncThresholdValue->setText("NA");
            ui->ClkClockSourceValue->setCurrentText("NA");
            ui->ClkClockEnableCheckBox->setChecked(false);
            ui->ClkClockVersionValue->setText("NA");
            ui->ClkClockDriftValue->setText("NA");
            ui->ClkClockDriftIntervalValue->setText("NA");
            ui->ClkClockDriftAdjCheckBox->setChecked(false);
            ui->ClkClockOffsetValue->setText("NA");
            ui->ClkClockOffsetIntervalValue->setText("NA");
            ui->ClkClockOffsetAdjCheckBox->setChecked(false);
            ui->ClkClockPiOffsetMulPValue->setText("NA");
            ui->ClkClockPiOffsetDivPValue->setText("NA");
            ui->ClkClockPiOffsetMulIValue->setText("NA");
            ui->ClkClockPiOffsetDivIValue->setText("NA");
            ui->ClkClockPiDriftMulPValue->setText("NA");
            ui->ClkClockPiDriftDivPValue->setText("NA");
            ui->ClkClockPiDriftMulIValue->setText("NA");
            ui->ClkClockPiDriftDivIValue->setText("NA");
            ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
            ui->ClkClockCorrectedOffsetValue->setText("NA");
            ui->ClkClockCorrectedDriftValue->setText("NA");
            ui->ClkClockDateValue->setText("NA");
            return;
        }
    }


    // source
    temp_string = ui->ClkClockSourceValue->currentText();
    if (temp_string == "NONE")
    {
        temp_data = 0x00000000;
    }
    else if (temp_string == "TOD")
    {
        temp_data = 0x00000001;
    }
    else if (temp_string == "IRIG")
    {
        temp_data = 0x00000002;
    }
    else if (temp_string == "PPS")
    {
        temp_data = 0x00000003;
    }
    else if (temp_string == "PTP")
    {
        temp_data = 0x00000004;
    }
    else if (temp_string == "RTC")
    {
        temp_data = 0x00000005;
    }
    else if (temp_string == "DCF")
    {
        temp_data = 0x00000006;
    }
    else if (temp_string == "REG")
    {
        temp_data = 0x000000FE;
    }
    else if (temp_string == "EXT")
    {
        temp_data = 0x000000FF;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_SelectReg, temp_data))
    {
        switch (temp_data & 0x0000FFFF)
        {
        case 0x00000000:
            ui->ClkClockSourceValue->setCurrentText("NONE");
            break;
        case 0x00000001:
            ui->ClkClockSourceValue->setCurrentText("TOD");
            break;
        case 0x00000002:
            ui->ClkClockSourceValue->setCurrentText("IRIG");
            break;
        case 0x00000003:
            ui->ClkClockSourceValue->setCurrentText("PPS");
            break;
        case 0x00000004:
            ui->ClkClockSourceValue->setCurrentText("PTP");
            break;
        case 0x00000005:
            ui->ClkClockSourceValue->setCurrentText("RTC");
            break;
        case 0x00000006:
            ui->ClkClockSourceValue->setCurrentText("DCF");
            break;
        case 0x000000FE:
            ui->ClkClockSourceValue->setCurrentText("REG");
            break;
        case 0x000000FF:
            ui->ClkClockSourceValue->setCurrentText("EXT");
            break;
        default:
            ui->ClkClockSourceValue->setCurrentText("NA");
            break;
        }

    }
    else
    {
        ui->ClkClockSourceValue->setCurrentText("NA");
    }

    // in sync Threshold
    temp_string = ui->ClkClockInSyncThresholdValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_InSyncThresholdReg, temp_data))
    {
        ui->ClkClockInSyncThresholdValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockInSyncThresholdValue->setText("NA");
    }

    // seconds
    temp_string = ui->ClkClockSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_TimeAdjValueHReg, temp_data))
    {
        ui->ClkClockSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockTimeAdjCheckBox->setChecked(false);
        ui->ClkClockSecondsValue->setText("NA");
    }

    // nanoseconds
    temp_string = ui->ClkClockNanosecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_TimeAdjValueLReg, temp_data))
    {
        ui->ClkClockNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockTimeAdjCheckBox->setChecked(false);
        ui->ClkClockNanosecondsValue->setText("NA");
    }


    // offset
    temp_string = ui->ClkClockOffsetValue->text();
    int temp_offset = temp_string.toInt(nullptr, 10);
    if (temp_offset < 0)
    {
        temp_data = abs(temp_offset) | 0x80000000;
    }
    else
    {
       temp_data = abs(temp_offset);
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_OffsetAdjValueReg, temp_data))
    {

        temp_offset = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_offset = -1 * temp_offset;
        }
        ui->ClkClockOffsetValue->setText(QString::number(temp_offset));

        temp_string = ui->ClkClockOffsetIntervalValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            //nothing
        }
        else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_OffsetAdjIntervalReg, temp_data))
        {
            ui->ClkClockOffsetIntervalValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkClockOffsetValue->setText("NA");
            ui->ClkClockOffsetIntervalValue->setText("NA");
            ui->ClkClockOffsetAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->ClkClockOffsetValue->setText("NA");
        ui->ClkClockOffsetIntervalValue->setText("NA");
        ui->ClkClockOffsetAdjCheckBox->setChecked(false);
    }

    // drift
    temp_string = ui->ClkClockDriftValue->text();
    int temp_drift = temp_string.toInt(nullptr, 10);
    if (temp_drift < 0)
    {
        temp_data = abs(temp_drift) | 0x80000000;
    }
    else
    {
       temp_data = abs(temp_drift);
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_DriftAdjValueReg, temp_data))
    {
        temp_drift = temp_data & 0x7FFFFFFF;
        if ((temp_data & 0x80000000) != 0)
        {
            temp_drift = -1 * temp_drift;
        }
        ui->ClkClockDriftValue->setText(QString::number(temp_drift));

        temp_string = ui->ClkClockDriftIntervalValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            //nothing
        }
        else if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_DriftAdjIntervalReg, temp_data))
        {
            ui->ClkClockDriftIntervalValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkClockDriftValue->setText("NA");
            ui->ClkClockDriftIntervalValue->setText("NA");
            ui->ClkClockDriftAdjCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->ClkClockDriftValue->setText("NA");
        ui->ClkClockDriftIntervalValue->setText("NA");
        ui->ClkClockDriftAdjCheckBox->setChecked(false);
    }

    // PI servo parameters
    if ((ui->ClkClockPiOffsetMulPValue->text() == "NA") ||
        (ui->ClkClockPiOffsetDivPValue->text() == "NA") ||
        (ui->ClkClockPiOffsetMulIValue->text() == "NA") ||
        (ui->ClkClockPiOffsetDivIValue->text() == "NA"))
    {
        ui->ClkClockPiOffsetMulPValue->setText("NA");
        ui->ClkClockPiOffsetDivPValue->setText("NA");
        ui->ClkClockPiOffsetMulIValue->setText("NA");
        ui->ClkClockPiOffsetDivIValue->setText("NA");
        ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    }
    else
    {
        int temp_mul;
        int temp_div;

        temp_string = ui->ClkClockPiOffsetMulPValue->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ui->ClkClockPiOffsetDivPValue->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000*temp_mul)/temp_div;

        if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_ServoOffsetFactorPReg, temp_data))
        {
            ui->ClkClockPiOffsetMulPValue->setText(QString::number(temp_mul));
            ui->ClkClockPiOffsetDivPValue->setText(QString::number(temp_div));
        }
        else
        {
            ui->ClkClockPiOffsetMulPValue->setText("NA");
            ui->ClkClockPiOffsetDivPValue->setText("NA");
            ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
        }

        temp_string = ui->ClkClockPiOffsetMulIValue->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ui->ClkClockPiOffsetDivIValue->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000*temp_mul)/temp_div;

        if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_ServoOffsetFactorIReg, temp_data))
        {
            ui->ClkClockPiOffsetMulIValue->setText(QString::number(temp_mul));
            ui->ClkClockPiOffsetDivIValue->setText(QString::number(temp_div));
        }
        else
        {
            ui->ClkClockPiOffsetMulIValue->setText("NA");
            ui->ClkClockPiOffsetDivIValue->setText("NA");
            ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
        }
    }

    if ((ui->ClkClockPiDriftMulPValue->text() == "NA") ||
        (ui->ClkClockPiDriftDivPValue->text() == "NA") ||
        (ui->ClkClockPiDriftMulIValue->text() == "NA") ||
        (ui->ClkClockPiDriftDivIValue->text() == "NA"))
    {
        ui->ClkClockPiDriftMulPValue->setText("NA");
        ui->ClkClockPiDriftDivPValue->setText("NA");
        ui->ClkClockPiDriftMulIValue->setText("NA");
        ui->ClkClockPiDriftDivIValue->setText("NA");
        ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
    }
    else
    {
        int temp_mul;
        int temp_div;

        temp_string = ui->ClkClockPiDriftMulPValue->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ui->ClkClockPiDriftDivPValue->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000*temp_mul)/temp_div;

        if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_ServoDriftFactorPReg, temp_data))
        {
            ui->ClkClockPiDriftMulPValue->setText(QString::number(temp_mul));
            ui->ClkClockPiDriftDivPValue->setText(QString::number(temp_div));
        }
        else
        {
            ui->ClkClockPiDriftMulPValue->setText("NA");
            ui->ClkClockPiDriftDivPValue->setText("NA");
            ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
        }

        temp_string = ui->ClkClockPiDriftMulIValue->text();
        temp_mul = temp_string.toInt(nullptr, 10);
        temp_string = ui->ClkClockPiDriftDivIValue->text();
        temp_div = temp_string.toInt(nullptr, 10);

        if (temp_mul > 1024)
        {
            temp_mul = 1024;
        }
        else if (temp_mul < 0)
        {
            temp_mul = 0;
        }

        if (temp_div > 1024)
        {
            temp_div = 1024;
        }
        else if (temp_div <= 0)
        {
            temp_div = 1;
        }

        temp_data = (0x00010000*temp_mul)/temp_div;

        if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_ServoDriftFactorIReg, temp_data))
        {
            ui->ClkClockPiDriftMulIValue->setText(QString::number(temp_mul));
            ui->ClkClockPiDriftDivIValue->setText(QString::number(temp_div));
        }
        else
        {
            ui->ClkClockPiDriftMulIValue->setText("NA");
            ui->ClkClockPiDriftDivIValue->setText("NA");
            ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);
        }
    }

    temp_data = 0x00000000;
    if(true == ui->ClkClockEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if(true == ui->ClkClockTimeAdjCheckBox->isChecked())
    {
        temp_data |= 0x00000002; // set time
    }
    if(true == ui->ClkClockOffsetAdjCheckBox->isChecked())
    {
        temp_data |= 0x00000004; // set offset
    }
    if(true == ui->ClkClockDriftAdjCheckBox->isChecked())
    {
        temp_data |= 0x00000008; // set drift
    }
    if(true == ui->ClkClockPiSetCustomParametersCheckBox->isChecked())
    {
        temp_data |= 0x00000100; // set servo parameters
    }

    if (0 ==  ucm->com_lib.write_reg(temp_addr + Ucm_ClkClock_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkClockSecondsValue->setText("NA");
        ui->ClkClockNanosecondsValue->setText("NA");
    }

    ui->ClkClockTimeAdjCheckBox->setChecked(false);
    ui->ClkClockOffsetAdjCheckBox->setChecked(false);
    ui->ClkClockDriftAdjCheckBox->setChecked(false);
    ui->ClkClockPiSetCustomParametersCheckBox->setChecked(false);

}

void Ucm_ClkClockTab::clk_clock_read_values_button_clicked(void)
{
    clk_clock_read_values();
}

void Ucm_ClkClockTab::clk_clock_write_values_button_clicked(void)
{
    clk_clock_write_values();
    clk_clock_read_values();
}

void Ucm_ClkClockTab::clk_clock_auto_refresh_button_clicked(void)
{
    if (ui->ClkClockAutoRefreshButton->text() == "Start Refresh")
    {
        ui->ClkClockAutoRefreshButton->setEnabled(false);

        ui->ClkClockInstanceComboBox->setEnabled(false);
        ui->ClkClockReadValuesButton->setEnabled(false);
        ui->ClkClockWriteValuesButton->setEnabled(false);

        clk_clock_offset_number_of_points = 0;
        clk_clock_offset_series-> clear();

        clk_clock_drift_number_of_points = 0;
        clk_clock_drift_series->clear();

        clk_clock_timer->start(1000);

        ui->ClkClockAutoRefreshButton->setText("Stop Refresh");
        ui->ClkClockAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->ClkClockAutoRefreshButton->setEnabled(false);

        clk_clock_timer->stop();

        ui->ClkClockInstanceComboBox->setEnabled(true);
        ui->ClkClockReadValuesButton->setEnabled(true);
        ui->ClkClockWriteValuesButton->setEnabled(true);

        ui->ClkClockAutoRefreshButton->setText("Start Refresh");
        ui->ClkClockAutoRefreshButton->setEnabled(true);
    }
}


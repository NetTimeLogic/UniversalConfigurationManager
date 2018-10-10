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

#include "Ucm_TapSlaveTab.h"
#include "ui_Ucm_TapSlaveTab.h"

Ucm_TapSlaveTab::Ucm_TapSlaveTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_TapSlaveTab();
    ui->setupUi(this);

    tap_slave_timer = new QTimer(this);
    tap_slave_timer->stop();
    connect(ui->TapSlaveReadValuesButton, SIGNAL(clicked()), this, SLOT(tap_slave_read_values_button_clicked()));
    connect(ui->TapSlaveWriteValuesButton, SIGNAL(clicked()), this, SLOT(tap_slave_write_values_button_clicked()));
    connect(ui->TapSlaveAutoRefreshButton, SIGNAL(clicked()), this, SLOT(tap_slave_auto_refresh_button_clicked()));
    connect(tap_slave_timer, SIGNAL(timeout()), this, SLOT(tap_slave_read_values_button_clicked()));


    tap_slave_frame_rate_series = new QLineSeries();
    tap_slave_frame_rate_number_of_points = 0;

    tap_slave_frame_rate_chart = new QChart();
    tap_slave_frame_rate_chart->setContentsMargins(0, 0, 0, 0);
    tap_slave_frame_rate_chart->setBackgroundRoundness(0);
    tap_slave_frame_rate_chart->setBackgroundBrush(Qt::white);
    tap_slave_frame_rate_chart->legend()->hide();
    tap_slave_frame_rate_chart->addSeries(tap_slave_frame_rate_series);

    QValueAxis* tap_slave_frame_rate_chart_x_axis = new QValueAxis;
    tap_slave_frame_rate_chart_x_axis->setLabelFormat("%i");
    tap_slave_frame_rate_chart_x_axis->setTickCount(10);
    tap_slave_frame_rate_chart_x_axis->setMin(0);
    tap_slave_frame_rate_chart_x_axis->setMax(19);
    tap_slave_frame_rate_chart->addAxis(tap_slave_frame_rate_chart_x_axis, Qt::AlignBottom);
    tap_slave_frame_rate_series->attachAxis(tap_slave_frame_rate_chart_x_axis);

    QValueAxis* tap_slave_frame_rate_chart_y_axis = new QValueAxis;
    tap_slave_frame_rate_chart_y_axis->setLabelFormat("%i");
    tap_slave_frame_rate_chart_y_axis->setTickCount(5);
    tap_slave_frame_rate_chart_y_axis->setMin(0);
    tap_slave_frame_rate_chart_y_axis->setMax(20);
    tap_slave_frame_rate_chart->addAxis(tap_slave_frame_rate_chart_y_axis, Qt::AlignLeft);
    tap_slave_frame_rate_series->attachAxis(tap_slave_frame_rate_chart_y_axis);

    tap_slave_frame_rate_chart_view = new QChartView(tap_slave_frame_rate_chart);
    tap_slave_frame_rate_chart_view->setRenderHint(QPainter::Antialiasing);

    ui->TapSlaveFrameRateChartLayout->addWidget(tap_slave_frame_rate_chart_view, 0, 0);

}

Ucm_TapSlaveTab::~Ucm_TapSlaveTab()
{
    tap_slave_timer->stop();
    delete ui;
    delete tap_slave_timer;
    delete tap_slave_frame_rate_series;
    delete tap_slave_frame_rate_chart_view;
    delete tap_slave_frame_rate_chart;
}

int Ucm_TapSlaveTab::tap_slave_resize(int height, int width)
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

    ui->TapSlaveFrameRateChartValue->setFixedHeight(210);
    ui->TapSlaveFrameRateChartValue->setFixedWidth(450+(width_delta/2));
    ui->TapSlaveFrameRateChartLabel->setFixedWidth(450+(width_delta/2));

    updateGeometry();

    return 0;
}

void Ucm_TapSlaveTab::tap_slave_add_instance(unsigned int instance)
{
    ui->TapSlaveInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_TapSlaveTab::tap_slave_enable(void)
{
    return 0;
}

int Ucm_TapSlaveTab::tap_slave_disable(void)
{
    tap_slave_timer->stop();
    ui->TapSlaveAutoRefreshButton->setText("Start Refresh");
    ui->TapSlaveInstanceComboBox->setEnabled(true);
    ui->TapSlaveReadValuesButton->setEnabled(true);
    ui->TapSlaveWriteValuesButton->setEnabled(true);
    ui->TapSlaveInstanceComboBox->clear();

    ui->TapSlaveNrOfFramesValue->setText("NA");
    ui->TapSlavePtpOnlyCheckBox->setChecked(false);
    ui->TapSlaveEnableCheckBox->setChecked(false);
    ui->TapSlaveVersionValue->setText("NA");

    tap_slave_frame_rate_number_of_points = 0;
    tap_slave_frame_rate_series->clear();

    return 0;
}

void Ucm_TapSlaveTab::tap_slave_read_values(void)
{
    int temp_max = 0;
    unsigned int temp_rate = 0;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->TapSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TapSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TapSlaveNrOfFramesValue->setText("NA");
            ui->TapSlavePtpOnlyCheckBox->setChecked(false);
            ui->TapSlaveEnableCheckBox->setChecked(false);
            ui->TapSlaveVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TapSlave_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TapSlaveEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TapSlaveEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TapSlaveEnableCheckBox->setChecked(false);
    }

    // ptp only
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TapSlave_FilterModeReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TapSlavePtpOnlyCheckBox->setChecked(false);
        }
        else
        {
            ui->TapSlavePtpOnlyCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TapSlavePtpOnlyCheckBox->setChecked(false);
    }

    // nr of frames
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TapSlave_NrOfFramesReg, temp_data))
    {
        ui->TapSlaveNrOfFramesValue->setText(QString::number(temp_data));

        if (true == tap_slave_timer->isActive())
        {
            if (tap_slave_frame_rate_number_of_points == 0)
            {
                tap_slave_old_nr_of_frames = temp_data;
            }

            temp_rate = temp_data - tap_slave_old_nr_of_frames;
            tap_slave_old_nr_of_frames = temp_data;

            tap_slave_frame_rate_series->append(tap_slave_frame_rate_number_of_points, temp_rate);

            if (tap_slave_frame_rate_number_of_points < 20)
            {
                tap_slave_frame_rate_number_of_points++;
            }
            else
            {
                for (int i = 1; i < tap_slave_frame_rate_series->count(); i++)
                {
                    QPointF temp_point = tap_slave_frame_rate_series->at(i);
                    tap_slave_frame_rate_series->replace(i, (i-1), temp_point.y());
                }
                tap_slave_frame_rate_series->remove(0);
            }

            temp_max = 0;
            for (int i = 0; i < tap_slave_frame_rate_series->count(); i++)
            {
                QPointF temp_point = tap_slave_frame_rate_series->at(i);
                if (temp_max < temp_point.y())
                {
                    temp_max = temp_point.y();
                }
            }
            temp_max = (temp_max * 5) / 4;
            temp_max = temp_max + (20 - temp_max%20);
            tap_slave_frame_rate_chart->axisY()->setMax(temp_max);

            tap_slave_frame_rate_chart->show();
        }
    }
    else
    {
        ui->TapSlaveNrOfFramesValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TapSlave_VersionReg, temp_data))
    {
        ui->TapSlaveVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->TapSlaveVersionValue->setText("NA");
    }
}

void Ucm_TapSlaveTab::tap_slave_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->TapSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TapSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TapSlaveNrOfFramesValue->setText("NA");
            ui->TapSlavePtpOnlyCheckBox->setChecked(false);
            ui->TapSlaveEnableCheckBox->setChecked(false);
            ui->TapSlaveVersionValue->setText("NA");
            return;
        }
    }

    // ptp only
    temp_data = 0x00000000; // nothing
    if(true == ui->TapSlavePtpOnlyCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // ptp filter only
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TapSlave_FilterModeReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TapSlavePtpOnlyCheckBox->setChecked(false);
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->TapSlaveEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TapSlave_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TapSlaveEnableCheckBox->setChecked(false);
    }
}

void Ucm_TapSlaveTab::tap_slave_read_values_button_clicked(void)
{
    tap_slave_read_values();
}

void Ucm_TapSlaveTab::tap_slave_write_values_button_clicked(void)
{
    tap_slave_write_values();
    tap_slave_read_values();
}

void Ucm_TapSlaveTab::tap_slave_auto_refresh_button_clicked(void)
{
    if (ui->TapSlaveAutoRefreshButton->text() == "Start Refresh")
    {
        ui->TapSlaveAutoRefreshButton->setEnabled(false);

        ui->TapSlaveInstanceComboBox->setEnabled(false);
        ui->TapSlaveReadValuesButton->setEnabled(false);
        ui->TapSlaveWriteValuesButton->setEnabled(false);

        tap_slave_frame_rate_number_of_points = 0;
        tap_slave_frame_rate_series->clear();

        tap_slave_timer->start(1000);

        ui->TapSlaveAutoRefreshButton->setText("Stop Refresh");
        ui->TapSlaveAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->TapSlaveAutoRefreshButton->setEnabled(false);

        tap_slave_timer->stop();

        ui->TapSlaveInstanceComboBox->setEnabled(true);
        ui->TapSlaveReadValuesButton->setEnabled(true);
        ui->TapSlaveWriteValuesButton->setEnabled(true);

        ui->TapSlaveAutoRefreshButton->setText("Start Refresh");
        ui->TapSlaveAutoRefreshButton->setEnabled(true);
    }
}


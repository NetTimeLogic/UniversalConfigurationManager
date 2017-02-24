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
}

Ucm_ClkClockTab::~Ucm_ClkClockTab()
{
    clk_clock_timer->stop();
    delete ui;
    delete clk_clock_timer;
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
    ui->ClkClockReadValuesButton->setEnabled(true);
    ui->ClkClockWriteValuesButton->setEnabled(true);
    ui->ClkClockInstanceComboBox->clear();

    return 0;
}

void Ucm_ClkClockTab::clk_clock_read_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
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
            ui->ClkClockInSyncValue->setText("NA");
            ui->ClkClockInSyncThresholdValue->setText("NA");
            ui->ClkClockSourceValue->setCurrentText("NA");
            ui->ClkClockEnableCheckBox->setChecked(false);
            ui->ClkClockVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
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
    if (0 ==  ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        for (int i = 0; i < 10; i++)
        {
            if(0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // seconds
                    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000014, temp_data))
                    {
                        ui->ClkClockSecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->ClkClockSecondsValue->setText("NA");
                    }

                    // nanoseconds
                    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
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
    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000004, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkClockInSyncValue->setText("false");
        }
        else
        {
            ui->ClkClockInSyncValue->setText("true");
        }
    }
    else
    {
        ui->ClkClockInSyncValue->setText("NA");
    }

    // in sync Threshold
    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000050, temp_data))
    {
        ui->ClkClockInSyncThresholdValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockInSyncThresholdValue->setText("NA");
    }

    // source
    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x00000008, temp_data))
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
            ui->ClkClockSourceValue->setCurrentText("REG");
            break;
        case 0x00000007:
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

    // version
    if (0 ==  ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
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
    unsigned int temp_data;
    unsigned int temp_addr;
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
            ui->ClkClockInSyncValue->setText("NA");
            ui->ClkClockInSyncThresholdValue->setText("NA");
            ui->ClkClockSourceValue->setCurrentText("NA");
            ui->ClkClockEnableCheckBox->setChecked(false);
            ui->ClkClockVersionValue->setText("NA");
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
    else if (temp_string == "REG")
    {
        temp_data = 0x00000006;
    }
    else if (temp_string == "EXT")
    {
        temp_data = 0x00000007;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + 0x00000008, temp_data))
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
            ui->ClkClockSourceValue->setCurrentText("REG");
            break;
        case 0x00000007:
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
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + 0x00000050, temp_data))
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
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + 0x00000024, temp_data))
    {
        ui->ClkClockSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockSecondsValue->setText("NA");
    }

    // nanoseconds
    temp_string = ui->ClkClockNanosecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 ==  ucm->com_lib.write_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->ClkClockNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkClockNanosecondsValue->setText("NA");
    }

    temp_data = 0x00000002; // set time
    if(true == ui->ClkClockEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 ==  ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkClockSecondsValue->setText("NA");
        ui->ClkClockNanosecondsValue->setText("NA");
    }
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

        ui->ClkClockReadValuesButton->setEnabled(false);
        ui->ClkClockWriteValuesButton->setEnabled(false);

        clk_clock_timer->start(1000);

        ui->ClkClockAutoRefreshButton->setText("Stop Refresh");
        ui->ClkClockAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->ClkClockAutoRefreshButton->setEnabled(false);

        clk_clock_timer->stop();

        ui->ClkClockReadValuesButton->setEnabled(true);
        ui->ClkClockWriteValuesButton->setEnabled(true);

        ui->ClkClockAutoRefreshButton->setText("Start Refresh");
        ui->ClkClockAutoRefreshButton->setEnabled(true);
    }
}


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

#include "Ucm_ClkTsTab.h"
#include "ui_Ucm_ClkTsTab.h"

Ucm_ClkTsTab::Ucm_ClkTsTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_ClkTsTab();
    ui->setupUi(this);

    clk_ts_timer = new QTimer(this);
    clk_ts_timer->stop();
    connect(ui->ClkTsReadValuesButton, SIGNAL(clicked()), this, SLOT(clk_ts_read_values_button_clicked()));
    connect(ui->ClkTsWriteValuesButton, SIGNAL(clicked()), this, SLOT(clk_ts_write_values_button_clicked()));
    connect(ui->ClkTsAutoRefreshButton, SIGNAL(clicked()), this, SLOT(clk_ts_auto_refresh_button_clicked()));
    connect(clk_ts_timer, SIGNAL(timeout()), this, SLOT(clk_ts_read_values_button_clicked()));
}

Ucm_ClkTsTab::~Ucm_ClkTsTab()
{
    clk_ts_timer->stop();
    delete ui;
    delete clk_ts_timer;
}

void Ucm_ClkTsTab::clk_ts_add_instance(unsigned int instance)
{
    ui->ClkTsInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_ClkTsTab::clk_ts_enable(void)
{
    return 0;
}

int Ucm_ClkTsTab::clk_ts_disable(void)
{
    clk_ts_timer->stop();
    ui->ClkTsAutoRefreshButton->setText("Start Refresh");
    ui->ClkTsReadValuesButton->setEnabled(true);
    ui->ClkTsWriteValuesButton->setEnabled(true);
    ui->ClkTsInstanceComboBox->clear();

    return 0;
}

void Ucm_ClkTsTab::clk_ts_read_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;
    unsigned int temp_data_width = 0;

    temp_string = ui->ClkTsInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_ClkSignalTimestamperCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->ClkTsInvertedCheckBox->setChecked(false);
            ui->ClkTsNewTimestampCheckBox->setChecked(false);
            ui->ClkTsSecondsValue->setText("NA");
            ui->ClkTsNanosecondsValue->setText("NA");
            ui->ClkTsTimestampNrValue->setText("NA");
            ui->ClkTsEnableCheckBox->setChecked(false);
            ui->ClkTsVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkTsEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->ClkTsEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->ClkTsEnableCheckBox->setChecked(false);
    }

    // polarity
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000008, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkTsInvertedCheckBox->setChecked(true);
        }
        else
        {
            ui->ClkTsInvertedCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->ClkTsInvertedCheckBox->setChecked(false);
    }

    // new timestamp (irq)
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkTsNewTimestampCheckBox->setChecked(false);
        }
        else
        {
            ui->ClkTsNewTimestampCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->ClkTsNewTimestampCheckBox->setChecked(false);
    }

    if (true == ui->ClkTsNewTimestampCheckBox->isChecked())
    {
        // timestamp nr
        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000020, temp_data))
        {
            ui->ClkTsTimestampNrValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkTsTimestampNrValue->setText("NA");
        }

        // seconds
        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000028, temp_data))
        {
            ui->ClkTsSecondsValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkTsSecondsValue->setText("NA");
        }

        // nanoseconds
        if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000024, temp_data))
        {
            ui->ClkTsNanosecondsValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkTsNanosecondsValue->setText("NA");
        }

        // data width
        if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000002C, temp_data))
        {
            temp_data_width = temp_data;
            ui->ClkTsDataWidthValue->setText(QString::number(temp_data));
        }
        else
        {
            ui->ClkTsDataWidthValue->setText("NA");
        }

        // data
        temp_string.clear();
        for (unsigned int i = 0; i < (temp_data_width/8); i++)
        {
            if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000030 + ((i/4)*4), temp_data))
            {
                if (i == 0)
                {
                    // nothing
                }
                else if ((i%4) == 0)
                {
                    temp_string.append("\n");
                }
                else
                {
                    temp_string.append(" ");
                }
                temp_data = (temp_data >> ((i%4)*8)) & 0x000000FF;
                temp_string.append(QString("%1").arg(temp_data, 2, 16, QLatin1Char('0')));
            }
            else
            {
                ui->ClkTsDataValue->setText("NA");
                break;
            }

            if (i == ((temp_data_width/8)-1))
            {
                ui->ClkTsDataValue->setText(temp_string);
            }
        }

        // clear irq
        temp_data = 0x00000001;
        if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000010, temp_data))
        {
            // nothing irq cleared
        }
        else
        {
            cout << "ERROR: " << "irq cleaning didn't work" << endl;
        }
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->ClkTsVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->ClkTsVersionValue->setText("NA");
    }

}

void Ucm_ClkTsTab::clk_ts_write_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->ClkTsInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_ClkSignalTimestamperCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->ClkTsInvertedCheckBox->setChecked(false);
            ui->ClkTsEnableCheckBox->setChecked(false);
            ui->ClkTsVersionValue->setText("NA");
            return;
        }
    }

    // polarity
    temp_data = 0x00000000; // nothing
    if(false == ui->ClkTsInvertedCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // no inversion
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000008, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkTsInvertedCheckBox->setChecked(false);
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->ClkTsEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    // irq mask
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000014, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkTsEnableCheckBox->setChecked(false);
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkTsEnableCheckBox->setChecked(false);
    }
}

void Ucm_ClkTsTab::clk_ts_read_values_button_clicked(void)
{
    clk_ts_read_values();
}

void Ucm_ClkTsTab::clk_ts_write_values_button_clicked(void)
{
    clk_ts_write_values();
    clk_ts_read_values();
}

void Ucm_ClkTsTab::clk_ts_auto_refresh_button_clicked(void)
{
    if (ui->ClkTsAutoRefreshButton->text() == "Start Refresh")
    {
        ui->ClkTsAutoRefreshButton->setEnabled(false);

        ui->ClkTsReadValuesButton->setEnabled(false);
        ui->ClkTsWriteValuesButton->setEnabled(false);

        clk_ts_timer->start(1000);

        ui->ClkTsAutoRefreshButton->setText("Stop Refresh");
        ui->ClkTsAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->ClkTsAutoRefreshButton->setEnabled(false);

        clk_ts_timer->stop();

        ui->ClkTsReadValuesButton->setEnabled(true);
        ui->ClkTsWriteValuesButton->setEnabled(true);

        ui->ClkTsAutoRefreshButton->setText("Start Refresh");
        ui->ClkTsAutoRefreshButton->setEnabled(true);
    }
}


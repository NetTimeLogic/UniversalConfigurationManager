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

#include "Ucm_TodMasterTab.h"
#include "ui_Ucm_TodMasterTab.h"

Ucm_TodMasterTab::Ucm_TodMasterTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_TodMasterTab();
    ui->setupUi(this);

    tod_master_timer = new QTimer(this);
    tod_master_timer->stop();
    connect(ui->TodMasterReadValuesButton, SIGNAL(clicked()), this, SLOT(tod_master_read_values_button_clicked()));
    connect(ui->TodMasterWriteValuesButton, SIGNAL(clicked()), this, SLOT(tod_master_write_values_button_clicked()));
    connect(ui->TodMasterAutoRefreshButton, SIGNAL(clicked()), this, SLOT(tod_master_auto_refresh_button_clicked()));
    connect(tod_master_timer, SIGNAL(timeout()), this, SLOT(tod_master_read_values_button_clicked()));
}

Ucm_TodMasterTab::~Ucm_TodMasterTab()
{
    tod_master_timer->stop();
    delete ui;
    delete tod_master_timer;
}

int Ucm_TodMasterTab::tod_master_resize(int height, int width)
{
    return 0;
}

void Ucm_TodMasterTab::tod_master_add_instance(unsigned int instance)
{
    ui->TodMasterInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_TodMasterTab::tod_master_enable(void)
{
    return 0;
}

int Ucm_TodMasterTab::tod_master_disable(void)
{
    tod_master_timer->stop();
    ui->TodMasterAutoRefreshButton->setText("Start Refresh");
    ui->TodMasterInstanceComboBox->setEnabled(true);
    ui->TodMasterReadValuesButton->setEnabled(true);
    ui->TodMasterWriteValuesButton->setEnabled(true);
    ui->TodMasterInstanceComboBox->clear();

    ui->TodMasterCorrectionValue->setText("NA");
    ui->TodMasterLocalSignCheckBox->setChecked(false);
    ui->TodMasterLocalHoursValue->setText("NA");
    ui->TodMasterLocalMinutesValue->setText("NA");
    ui->TodMasterBaudRateValue->setCurrentText("NA");
    ui->TodMasterEnableCheckBox->setChecked(false);
    ui->TodMasterVersionValue->setText("NA");

    return 0;
}

void Ucm_TodMasterTab::tod_master_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->TodMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TodMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TodMasterCorrectionValue->setText("NA");
            ui->TodMasterLocalSignCheckBox->setChecked(false);
            ui->TodMasterLocalHoursValue->setText("NA");
            ui->TodMasterLocalMinutesValue->setText("NA");
            ui->TodMasterBaudRateValue->setCurrentText("NA");
            ui->TodMasterEnableCheckBox->setChecked(false);
            ui->TodMasterVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TodMasterEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TodMasterEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TodMasterEnableCheckBox->setChecked(false);
    }


    // correction
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->TodMasterCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TodMasterCorrectionValue->setText("NA");
    }


    // local time
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000014, temp_data))
    {
        ui->TodMasterLocalMinutesValue->setText(QString::number((temp_data & 0x0000003F)));
        temp_data = temp_data >> 16;
        ui->TodMasterLocalHoursValue->setText(QString::number((temp_data & 0x0000000F)));
        temp_data = temp_data >> 15;
        if (temp_data == 0)
        {
            ui->TodMasterLocalSignCheckBox->setChecked(false);
        }
        else
        {
            ui->TodMasterLocalSignCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TodMasterLocalSignCheckBox->setChecked(false);
        ui->TodMasterLocalHoursValue->setText("NA");
        ui->TodMasterLocalMinutesValue->setText("NA");
    }

    // baud rate
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000020, temp_data))
    {
        switch (temp_data)
        {
        case 0:
            ui->TodMasterBaudRateValue->setCurrentText("1200");
            break;
        case 1:
            ui->TodMasterBaudRateValue->setCurrentText("2400");
            break;
        case 2:
            ui->TodMasterBaudRateValue->setCurrentText("4800");
            break;
        case 3:
            ui->TodMasterBaudRateValue->setCurrentText("9600");
            break;
        case 4:
            ui->TodMasterBaudRateValue->setCurrentText("19200");
            break;
        case 5:
            ui->TodMasterBaudRateValue->setCurrentText("38400");
            break;
        case 6:
            ui->TodMasterBaudRateValue->setCurrentText("57600");
            break;
        case 7:
            ui->TodMasterBaudRateValue->setCurrentText("115200");
            break;
        case 8:
            ui->TodMasterBaudRateValue->setCurrentText("230400");
            break;
        case 9:
            ui->TodMasterBaudRateValue->setCurrentText("460800");
            break;
        case 10:
            ui->TodMasterBaudRateValue->setCurrentText("921600");
            break;
        case 11:
            ui->TodMasterBaudRateValue->setCurrentText("1000000");
            break;
        case 12:
            ui->TodMasterBaudRateValue->setCurrentText("2000000");
            break;
        default:
            ui->TodMasterBaudRateValue->setCurrentText("NA");
            break;
        }
    }
    else
    {
        ui->TodMasterBaudRateValue->setCurrentText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->TodMasterVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->TodMasterVersionValue->setText("NA");
    }

}

void Ucm_TodMasterTab::tod_master_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->TodMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TodMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TodMasterCorrectionValue->setText("NA");
            ui->TodMasterLocalSignCheckBox->setChecked(false);
            ui->TodMasterLocalHoursValue->setText("NA");
            ui->TodMasterLocalMinutesValue->setText("NA");
            ui->TodMasterBaudRateValue->setCurrentText("NA");
            ui->TodMasterEnableCheckBox->setChecked(false);
            ui->TodMasterVersionValue->setText("NA");
            return;
        }
    }


    // correction
    temp_string = ui->TodMasterCorrectionValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->TodMasterCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TodMasterCorrectionValue->setText("NA");
    }

    // local time
    temp_string = ui->TodMasterLocalHoursValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else
    {
        temp_data = temp_data << 16;
        temp_string = ui->TodMasterLocalMinutesValue->text();
        temp_data |= temp_string.toUInt(nullptr, 10);
        if(true == ui->TodMasterLocalSignCheckBox->isChecked())
        {
            temp_data |= 0x80000008; // negative
        }
        if (temp_string == "NA")
        {
            //nothing
        }
        else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000014, temp_data))
        {
            ui->TodMasterLocalMinutesValue->setText(QString::number((temp_data & 0x0000003F)));
            temp_data = temp_data >> 16;
            ui->TodMasterLocalHoursValue->setText(QString::number((temp_data & 0x0000000F)));
            temp_data = temp_data >> 15;
            if (temp_data == 0)
            {
                ui->TodMasterLocalSignCheckBox->setChecked(false);
            }
            else
            {
                ui->TodMasterLocalSignCheckBox->setChecked(true);
            }
        }
        else
        {
            ui->TodMasterLocalSignCheckBox->setChecked(false);
            ui->TodMasterLocalHoursValue->setText("NA");
            ui->TodMasterLocalMinutesValue->setText("NA");
        }
    }

    // baud rate
    temp_string = ui->TodMasterBaudRateValue->currentText();
    if (temp_string == "1200")
    {
        temp_data = 0x00000000;
    }
    else if (temp_string == "2400")
    {
        temp_data = 0x0000001;
    }
    else if (temp_string == "4800")
    {
        temp_data = 0x00000002;
    }
    else if (temp_string == "9600")
    {
        temp_data = 0x00000003;
    }
    else if (temp_string == "19200")
    {
        temp_data = 0x00000004;
    }
    else if (temp_string == "38400")
    {
        temp_data = 0x00000005;
    }
    else if (temp_string == "57600")
    {
        temp_data = 0x00000006;
    }
    else if (temp_string == "115200")
    {
        temp_data = 0x00000007;
    }
    else if (temp_string == "230400")
    {
        temp_data = 0x00000008;
    }
    else if (temp_string == "460800")
    {
        temp_data = 0x00000009;
    }
    else if (temp_string == "921600")
    {
        temp_data = 0x000000A;
    }
    else if (temp_string == "1000000")
    {
        temp_data = 0x0000000B;
    }
    else if (temp_string == "2000000")
    {
        temp_data = 0x0000000C;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000020, temp_data))
    {
        switch (temp_data)
        {
        case 0:
            ui->TodMasterBaudRateValue->setCurrentText("1200");
            break;
        case 1:
            ui->TodMasterBaudRateValue->setCurrentText("2400");
            break;
        case 2:
            ui->TodMasterBaudRateValue->setCurrentText("4800");
            break;
        case 3:
            ui->TodMasterBaudRateValue->setCurrentText("9600");
            break;
        case 4:
            ui->TodMasterBaudRateValue->setCurrentText("19200");
            break;
        case 5:
            ui->TodMasterBaudRateValue->setCurrentText("38400");
            break;
        case 6:
            ui->TodMasterBaudRateValue->setCurrentText("57600");
            break;
        case 7:
            ui->TodMasterBaudRateValue->setCurrentText("115200");
            break;
        case 8:
            ui->TodMasterBaudRateValue->setCurrentText("230400");
            break;
        case 9:
            ui->TodMasterBaudRateValue->setCurrentText("460800");
            break;
        case 10:
            ui->TodMasterBaudRateValue->setCurrentText("921600");
            break;
        case 11:
            ui->TodMasterBaudRateValue->setCurrentText("1000000");
            break;
        case 12:
            ui->TodMasterBaudRateValue->setCurrentText("2000000");
            break;
        default:
            ui->TodMasterBaudRateValue->setCurrentText("NA");
            break;
        }
    }
    else
    {
        ui->TodMasterBaudRateValue->setCurrentText("NA");
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->TodMasterEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TodMasterEnableCheckBox->setChecked(false);
    }
}

void Ucm_TodMasterTab::tod_master_read_values_button_clicked(void)
{
    tod_master_read_values();
}

void Ucm_TodMasterTab::tod_master_write_values_button_clicked(void)
{
    tod_master_write_values();
    tod_master_read_values();
}

void Ucm_TodMasterTab::tod_master_auto_refresh_button_clicked(void)
{
    if (ui->TodMasterAutoRefreshButton->text() == "Start Refresh")
    {
        ui->TodMasterAutoRefreshButton->setEnabled(false);

        ui->TodMasterInstanceComboBox->setEnabled(false);
        ui->TodMasterReadValuesButton->setEnabled(false);
        ui->TodMasterWriteValuesButton->setEnabled(false);

        tod_master_timer->start(1000);

        ui->TodMasterAutoRefreshButton->setText("Stop Refresh");
        ui->TodMasterAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->TodMasterAutoRefreshButton->setEnabled(false);

        tod_master_timer->stop();

        ui->TodMasterInstanceComboBox->setEnabled(true);
        ui->TodMasterReadValuesButton->setEnabled(true);
        ui->TodMasterWriteValuesButton->setEnabled(true);

        ui->TodMasterAutoRefreshButton->setText("Start Refresh");
        ui->TodMasterAutoRefreshButton->setEnabled(true);
    }
}


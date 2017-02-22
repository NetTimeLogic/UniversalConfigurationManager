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

#include "Ucm_TodSlaveTab.h"
#include "ui_Ucm_TodSlaveTab.h"

Ucm_TodSlaveTab::Ucm_TodSlaveTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_TodSlaveTab();
    ui->setupUi(this);

    tod_slave_timer = new QTimer(this);
    tod_slave_timer->stop();
    connect(ui->TodSlaveReadValuesButton, SIGNAL(clicked()), this, SLOT(tod_slave_read_values_button_clicked()));
    connect(ui->TodSlaveWriteValuesButton, SIGNAL(clicked()), this, SLOT(tod_slave_write_values_button_clicked()));
    connect(ui->TodSlaveAutoRefreshButton, SIGNAL(clicked()), this, SLOT(tod_slave_auto_refresh_button_clicked()));
    connect(tod_slave_timer, SIGNAL(timeout()), this, SLOT(tod_slave_read_values_button_clicked()));
}

Ucm_TodSlaveTab::~Ucm_TodSlaveTab()
{
    tod_slave_timer->stop();
    delete ui;
    delete tod_slave_timer;
}

void Ucm_TodSlaveTab::tod_slave_add_instance(unsigned int instance)
{
    ui->TodSlaveInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_TodSlaveTab::tod_slave_enable(void)
{
    return 0;
}

int Ucm_TodSlaveTab::tod_slave_disable(void)
{
    tod_slave_timer->stop();
    ui->TodSlaveAutoRefreshButton->setText("Start Refresh");
    ui->TodSlaveReadValuesButton->setEnabled(true);
    ui->TodSlaveWriteValuesButton->setEnabled(true);
    ui->TodSlaveInstanceComboBox->clear();

    return 0;
}

void Ucm_TodSlaveTab::tod_slave_read_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->TodSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TodSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TodSlaveCorrectionValue->setText("NA");
            ui->TodSlaveBaudRateValue->setCurrentText("NA");
            ui->TodSlaveEnableCheckBox->setChecked(false);
            ui->TodSlaveVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TodSlaveEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TodSlaveEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TodSlaveEnableCheckBox->setChecked(false);
    }


    // correction
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->TodSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TodSlaveCorrectionValue->setText("NA");
    }


    // baud rate
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000020, temp_data))
    {
        switch (temp_data)
        {
        case 0:
            ui->TodSlaveBaudRateValue->setCurrentText("1200");
            break;
        case 1:
            ui->TodSlaveBaudRateValue->setCurrentText("2400");
            break;
        case 2:
            ui->TodSlaveBaudRateValue->setCurrentText("4800");
            break;
        case 3:
            ui->TodSlaveBaudRateValue->setCurrentText("9600");
            break;
        case 4:
            ui->TodSlaveBaudRateValue->setCurrentText("19200");
            break;
        case 5:
            ui->TodSlaveBaudRateValue->setCurrentText("38400");
            break;
        case 6:
            ui->TodSlaveBaudRateValue->setCurrentText("57600");
            break;
        case 7:
            ui->TodSlaveBaudRateValue->setCurrentText("115200");
            break;
        case 8:
            ui->TodSlaveBaudRateValue->setCurrentText("230400");
            break;
        case 9:
            ui->TodSlaveBaudRateValue->setCurrentText("460800");
            break;
        case 10:
            ui->TodSlaveBaudRateValue->setCurrentText("921600");
            break;
        case 11:
            ui->TodSlaveBaudRateValue->setCurrentText("1000000");
            break;
        case 12:
            ui->TodSlaveBaudRateValue->setCurrentText("2000000");
            break;
        default:
            ui->TodSlaveBaudRateValue->setCurrentText("NA");
            break;
        }
    }
    else
    {
        ui->TodSlaveBaudRateValue->setCurrentText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->TodSlaveVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->TodSlaveVersionValue->setText("NA");
    }

}

void Ucm_TodSlaveTab::tod_slave_write_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->TodSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TodSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TodSlaveCorrectionValue->setText("NA");
            ui->TodSlaveBaudRateValue->setCurrentText("NA");
            ui->TodSlaveEnableCheckBox->setChecked(false);
            ui->TodSlaveVersionValue->setText("NA");
            return;
        }
    }


    // correction
    temp_string = ui->TodSlaveCorrectionValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->TodSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TodSlaveCorrectionValue->setText("NA");
    }

    // baud rate
    temp_string = ui->TodSlaveBaudRateValue->currentText();
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
            ui->TodSlaveBaudRateValue->setCurrentText("1200");
            break;
        case 1:
            ui->TodSlaveBaudRateValue->setCurrentText("2400");
            break;
        case 2:
            ui->TodSlaveBaudRateValue->setCurrentText("4800");
            break;
        case 3:
            ui->TodSlaveBaudRateValue->setCurrentText("9600");
            break;
        case 4:
            ui->TodSlaveBaudRateValue->setCurrentText("19200");
            break;
        case 5:
            ui->TodSlaveBaudRateValue->setCurrentText("38400");
            break;
        case 6:
            ui->TodSlaveBaudRateValue->setCurrentText("57600");
            break;
        case 7:
            ui->TodSlaveBaudRateValue->setCurrentText("115200");
            break;
        case 8:
            ui->TodSlaveBaudRateValue->setCurrentText("230400");
            break;
        case 9:
            ui->TodSlaveBaudRateValue->setCurrentText("460800");
            break;
        case 10:
            ui->TodSlaveBaudRateValue->setCurrentText("921600");
            break;
        case 11:
            ui->TodSlaveBaudRateValue->setCurrentText("1000000");
            break;
        case 12:
            ui->TodSlaveBaudRateValue->setCurrentText("2000000");
            break;
        default:
            ui->TodSlaveBaudRateValue->setCurrentText("NA");
            break;
        }
    }
    else
    {
        ui->TodSlaveBaudRateValue->setCurrentText("NA");
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->TodSlaveEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TodSlaveEnableCheckBox->setChecked(false);
    }
}

void Ucm_TodSlaveTab::tod_slave_read_values_button_clicked(void)
{
    tod_slave_read_values();
}

void Ucm_TodSlaveTab::tod_slave_write_values_button_clicked(void)
{
    tod_slave_write_values();
    tod_slave_read_values();
}

void Ucm_TodSlaveTab::tod_slave_auto_refresh_button_clicked(void)
{
    if (ui->TodSlaveAutoRefreshButton->text() == "Start Refresh")
    {
        ui->TodSlaveAutoRefreshButton->setEnabled(false);

        ui->TodSlaveReadValuesButton->setEnabled(false);
        ui->TodSlaveWriteValuesButton->setEnabled(false);

        tod_slave_timer->start(1000);

        ui->TodSlaveAutoRefreshButton->setText("Stop Refresh");
        ui->TodSlaveAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->TodSlaveAutoRefreshButton->setEnabled(false);

        tod_slave_timer->stop();

        ui->TodSlaveReadValuesButton->setEnabled(true);
        ui->TodSlaveWriteValuesButton->setEnabled(true);

        ui->TodSlaveAutoRefreshButton->setText("Start Refresh");
        ui->TodSlaveAutoRefreshButton->setEnabled(true);
    }
}


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

#include "Ucm_DcfSlaveTab.h"
#include "ui_Ucm_DcfSlaveTab.h"

Ucm_DcfSlaveTab::Ucm_DcfSlaveTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_DcfSlaveTab();
    ui->setupUi(this);

    dcf_slave_timer = new QTimer(this);
    dcf_slave_timer->stop();
    connect(ui->DcfSlaveReadValuesButton, SIGNAL(clicked()), this, SLOT(dcf_slave_read_values_button_clicked()));
    connect(ui->DcfSlaveWriteValuesButton, SIGNAL(clicked()), this, SLOT(dcf_slave_write_values_button_clicked()));
    connect(ui->DcfSlaveAutoRefreshButton, SIGNAL(clicked()), this, SLOT(dcf_slave_auto_refresh_button_clicked()));
    connect(dcf_slave_timer, SIGNAL(timeout()), this, SLOT(dcf_slave_read_values_button_clicked()));
}

Ucm_DcfSlaveTab::~Ucm_DcfSlaveTab()
{
    dcf_slave_timer->stop();
    delete ui;
    delete dcf_slave_timer;
}

int Ucm_DcfSlaveTab::dcf_slave_resize(int height, int width)
{
    return 0;
}

void Ucm_DcfSlaveTab::dcf_slave_add_instance(unsigned int instance)
{
    ui->DcfSlaveInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_DcfSlaveTab::dcf_slave_enable(void)
{
    return 0;
}

int Ucm_DcfSlaveTab::dcf_slave_disable(void)
{
    dcf_slave_timer->stop();
    ui->DcfSlaveAutoRefreshButton->setText("Start Refresh");
    ui->DcfSlaveInstanceComboBox->setEnabled(true);
    ui->DcfSlaveReadValuesButton->setEnabled(true);
    ui->DcfSlaveWriteValuesButton->setEnabled(true);
    ui->DcfSlaveInstanceComboBox->clear();

    ui->DcfSlaveBitPositionValue->setText("NA");
    ui->DcfSlaveCorrectionValue->setText("NA");
    ui->DcfSlaveAirDelayValue->setText("NA");
    ui->DcfSlaveEnableCheckBox->setChecked(false);
    ui->DcfSlaveVersionValue->setText("NA");

    return 0;
}

void Ucm_DcfSlaveTab::dcf_slave_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->DcfSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_DcfSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->DcfSlaveBitPositionValue->setText("NA");
            ui->DcfSlaveCorrectionValue->setText("NA");
            ui->DcfSlaveAirDelayValue->setText("NA");
            ui->DcfSlaveEnableCheckBox->setChecked(false);
            ui->DcfSlaveVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfSlave_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->DcfSlaveEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->DcfSlaveEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->DcfSlaveEnableCheckBox->setChecked(false);
    }

    // bit position
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfSlave_BitPositionReg, temp_data))
    {
        ui->DcfSlaveBitPositionValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->DcfSlaveBitPositionValue->setText("NA");
    }

    // correction
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfSlave_CorrectionReg, temp_data))
    {
        ui->DcfSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->DcfSlaveCorrectionValue->setText("NA");
    }

    // air delay
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfSlave_AirDelayReg, temp_data))
    {
        ui->DcfSlaveAirDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->DcfSlaveAirDelayValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfSlave_VersionReg, temp_data))
    {
        ui->DcfSlaveVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->DcfSlaveVersionValue->setText("NA");
    }
}

void Ucm_DcfSlaveTab::dcf_slave_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->DcfSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_DcfSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->DcfSlaveBitPositionValue->setText("NA");
            ui->DcfSlaveCorrectionValue->setText("NA");
            ui->DcfSlaveAirDelayValue->setText("NA");
            ui->DcfSlaveEnableCheckBox->setChecked(false);
            ui->DcfSlaveVersionValue->setText("NA");
            return;
        }
    }


    // correction
    temp_string = ui->DcfSlaveCorrectionValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_DcfSlave_CorrectionReg, temp_data))
    {
        ui->DcfSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->DcfSlaveCorrectionValue->setText("NA");
    }

    // air delay
    temp_string = ui->DcfSlaveAirDelayValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_DcfSlave_AirDelayReg, temp_data))
    {
        ui->DcfSlaveAirDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->DcfSlaveAirDelayValue->setText("NA");
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->DcfSlaveEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_DcfSlave_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->DcfSlaveEnableCheckBox->setChecked(false);
    }
}

void Ucm_DcfSlaveTab::dcf_slave_read_values_button_clicked(void)
{
    dcf_slave_read_values();
}

void Ucm_DcfSlaveTab::dcf_slave_write_values_button_clicked(void)
{
    dcf_slave_write_values();
    dcf_slave_read_values();
}

void Ucm_DcfSlaveTab::dcf_slave_auto_refresh_button_clicked(void)
{
    if (ui->DcfSlaveAutoRefreshButton->text() == "Start Refresh")
    {
        ui->DcfSlaveAutoRefreshButton->setEnabled(false);

        ui->DcfSlaveInstanceComboBox->setEnabled(false);
        ui->DcfSlaveReadValuesButton->setEnabled(false);
        ui->DcfSlaveWriteValuesButton->setEnabled(false);

        dcf_slave_timer->start(1000);

        ui->DcfSlaveAutoRefreshButton->setText("Stop Refresh");
        ui->DcfSlaveAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->DcfSlaveAutoRefreshButton->setEnabled(false);

        dcf_slave_timer->stop();

        ui->DcfSlaveInstanceComboBox->setEnabled(true);
        ui->DcfSlaveReadValuesButton->setEnabled(true);
        ui->DcfSlaveWriteValuesButton->setEnabled(true);

        ui->DcfSlaveAutoRefreshButton->setText("Start Refresh");
        ui->DcfSlaveAutoRefreshButton->setEnabled(true);
    }
}


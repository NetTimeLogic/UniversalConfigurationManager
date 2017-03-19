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

#include "Ucm_PpsSlaveTab.h"
#include "ui_Ucm_PpsSlaveTab.h"

Ucm_PpsSlaveTab::Ucm_PpsSlaveTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_PpsSlaveTab();
    ui->setupUi(this);

    pps_slave_timer = new QTimer(this);
    pps_slave_timer->stop();
    connect(ui->PpsSlaveReadValuesButton, SIGNAL(clicked()), this, SLOT(pps_slave_read_values_button_clicked()));
    connect(ui->PpsSlaveWriteValuesButton, SIGNAL(clicked()), this, SLOT(pps_slave_write_values_button_clicked()));
    connect(ui->PpsSlaveAutoRefreshButton, SIGNAL(clicked()), this, SLOT(pps_slave_auto_refresh_button_clicked()));
    connect(pps_slave_timer, SIGNAL(timeout()), this, SLOT(pps_slave_read_values_button_clicked()));
}

Ucm_PpsSlaveTab::~Ucm_PpsSlaveTab()
{
    pps_slave_timer->stop();
    delete ui;
    delete pps_slave_timer;
}

void Ucm_PpsSlaveTab::pps_slave_add_instance(unsigned int instance)
{
    ui->PpsSlaveInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_PpsSlaveTab::pps_slave_enable(void)
{
    return 0;
}

int Ucm_PpsSlaveTab::pps_slave_disable(void)
{
    pps_slave_timer->stop();
    ui->PpsSlaveAutoRefreshButton->setText("Start Refresh");
    ui->PpsSlaveReadValuesButton->setEnabled(true);
    ui->PpsSlaveWriteValuesButton->setEnabled(true);
    ui->PpsSlaveInstanceComboBox->clear();

    ui->PpsSlavePulseWidthValue->setText("NA");
    ui->PpsSlaveCableDelayValue->setText("NA");
    ui->PpsSlaveInvertedCheckBox->setChecked(false);
    ui->PpsSlaveEnableCheckBox->setChecked(false);
    ui->PpsSlaveVersionValue->setText("NA");

    return 0;
}

void Ucm_PpsSlaveTab::pps_slave_read_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->PpsSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PpsSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PpsSlavePulseWidthValue->setText("NA");
            ui->PpsSlaveCableDelayValue->setText("NA");
            ui->PpsSlaveInvertedCheckBox->setChecked(false);
            ui->PpsSlaveEnableCheckBox->setChecked(false);
            ui->PpsSlaveVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PpsSlaveEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PpsSlaveEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->PpsSlaveEnableCheckBox->setChecked(false);
    }


    // polarity
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000008, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PpsSlaveInvertedCheckBox->setChecked(true);
        }
        else
        {
            ui->PpsSlaveInvertedCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PpsSlaveInvertedCheckBox->setChecked(false);
    }

    // pulse width
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->PpsSlavePulseWidthValue->setText(QString::number((temp_data)));
    }
    else
    {
        ui->PpsSlavePulseWidthValue->setText("NA");
    }


    // cable delay
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->PpsSlaveCableDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->PpsSlaveCableDelayValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->PpsSlaveVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->PpsSlaveVersionValue->setText("NA");
    }
}

void Ucm_PpsSlaveTab::pps_slave_write_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->PpsSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PpsSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PpsSlavePulseWidthValue->setText("NA");
            ui->PpsSlaveCableDelayValue->setText("NA");
            ui->PpsSlaveInvertedCheckBox->setChecked(false);
            ui->PpsSlaveEnableCheckBox->setChecked(false);
            ui->PpsSlaveVersionValue->setText("NA");
            return;
        }
    }


    // cable delay
    temp_string = ui->PpsSlaveCableDelayValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->PpsSlaveCableDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->PpsSlaveCableDelayValue->setText("NA");
    }

    // polarity
    temp_data = 0x00000000; // nothing
    if(false == ui->PpsSlaveInvertedCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // no inversion
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000008, temp_data))
    {
        // nothing
    }
    else
    {
        ui->PpsSlaveInvertedCheckBox->setChecked(false);
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->PpsSlaveEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->PpsSlaveEnableCheckBox->setChecked(false);
    }
}

void Ucm_PpsSlaveTab::pps_slave_read_values_button_clicked(void)
{
    pps_slave_read_values();
}

void Ucm_PpsSlaveTab::pps_slave_write_values_button_clicked(void)
{
    pps_slave_write_values();
    pps_slave_read_values();
}

void Ucm_PpsSlaveTab::pps_slave_auto_refresh_button_clicked(void)
{
    if (ui->PpsSlaveAutoRefreshButton->text() == "Start Refresh")
    {
        ui->PpsSlaveAutoRefreshButton->setEnabled(false);

        ui->PpsSlaveReadValuesButton->setEnabled(false);
        ui->PpsSlaveWriteValuesButton->setEnabled(false);

        pps_slave_timer->start(1000);

        ui->PpsSlaveAutoRefreshButton->setText("Stop Refresh");
        ui->PpsSlaveAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->PpsSlaveAutoRefreshButton->setEnabled(false);

        pps_slave_timer->stop();

        ui->PpsSlaveReadValuesButton->setEnabled(true);
        ui->PpsSlaveWriteValuesButton->setEnabled(true);

        ui->PpsSlaveAutoRefreshButton->setText("Start Refresh");
        ui->PpsSlaveAutoRefreshButton->setEnabled(true);
    }
}


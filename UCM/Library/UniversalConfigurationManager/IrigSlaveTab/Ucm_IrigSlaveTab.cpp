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

#include "Ucm_IrigSlaveTab.h"
#include "ui_Ucm_IrigSlaveTab.h"

Ucm_IrigSlaveTab::Ucm_IrigSlaveTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_IrigSlaveTab();
    ui->setupUi(this);

    irig_slave_timer = new QTimer(this);
    irig_slave_timer->stop();
    connect(ui->IrigSlaveReadValuesButton, SIGNAL(clicked()), this, SLOT(irig_slave_read_values_button_clicked()));
    connect(ui->IrigSlaveWriteValuesButton, SIGNAL(clicked()), this, SLOT(irig_slave_write_values_button_clicked()));
    connect(ui->IrigSlaveAutoRefreshButton, SIGNAL(clicked()), this, SLOT(irig_slave_auto_refresh_button_clicked()));
    connect(irig_slave_timer, SIGNAL(timeout()), this, SLOT(irig_slave_read_values_button_clicked()));
}

Ucm_IrigSlaveTab::~Ucm_IrigSlaveTab()
{
    irig_slave_timer->stop();
    delete ui;
    delete irig_slave_timer;
}

void Ucm_IrigSlaveTab::irig_slave_add_instance(unsigned int instance)
{
    ui->IrigSlaveInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_IrigSlaveTab::irig_slave_enable(void)
{
    return 0;
}

int Ucm_IrigSlaveTab::irig_slave_disable(void)
{
    irig_slave_timer->stop();
    ui->IrigSlaveAutoRefreshButton->setText("Start Refresh");
    ui->IrigSlaveReadValuesButton->setEnabled(true);
    ui->IrigSlaveWriteValuesButton->setEnabled(true);
    ui->IrigSlaveInstanceComboBox->clear();

    return 0;
}

void Ucm_IrigSlaveTab::irig_slave_read_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->IrigSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_IrigSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->IrigSlaveCorrectionValue->setText("NA");
            ui->IrigSlaveCableDelayValue->setText("NA");
            ui->IrigSlaveEnableCheckBox->setChecked(false);
            ui->IrigSlaveVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->IrigSlaveEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->IrigSlaveEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->IrigSlaveEnableCheckBox->setChecked(false);
    }

    // correction
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->IrigSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IrigSlaveCorrectionValue->setText("NA");
    }

    // cable delay
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->IrigSlaveCableDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->IrigSlaveCableDelayValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->IrigSlaveVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->IrigSlaveVersionValue->setText("NA");
    }
}

void Ucm_IrigSlaveTab::irig_slave_write_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->IrigSlaveInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_IrigSlaveCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->IrigSlaveCorrectionValue->setText("NA");
            ui->IrigSlaveCableDelayValue->setText("NA");
            ui->IrigSlaveEnableCheckBox->setChecked(false);
            ui->IrigSlaveVersionValue->setText("NA");
            return;
        }
    }


    // correction
    temp_string = ui->IrigSlaveCorrectionValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->IrigSlaveCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IrigSlaveCorrectionValue->setText("NA");
    }

    // cable delay
    temp_string = ui->IrigSlaveCableDelayValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->IrigSlaveCableDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->IrigSlaveCableDelayValue->setText("NA");
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->IrigSlaveEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->IrigSlaveEnableCheckBox->setChecked(false);
    }
}

void Ucm_IrigSlaveTab::irig_slave_read_values_button_clicked(void)
{
    irig_slave_read_values();
}

void Ucm_IrigSlaveTab::irig_slave_write_values_button_clicked(void)
{
    irig_slave_write_values();
    irig_slave_read_values();
}

void Ucm_IrigSlaveTab::irig_slave_auto_refresh_button_clicked(void)
{
    if (ui->IrigSlaveAutoRefreshButton->text() == "Start Refresh")
    {
        ui->IrigSlaveAutoRefreshButton->setEnabled(false);

        ui->IrigSlaveReadValuesButton->setEnabled(false);
        ui->IrigSlaveWriteValuesButton->setEnabled(false);

        irig_slave_timer->start(1000);

        ui->IrigSlaveAutoRefreshButton->setText("Stop Refresh");
        ui->IrigSlaveAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->IrigSlaveAutoRefreshButton->setEnabled(false);

        irig_slave_timer->stop();

        ui->IrigSlaveReadValuesButton->setEnabled(true);
        ui->IrigSlaveWriteValuesButton->setEnabled(true);

        ui->IrigSlaveAutoRefreshButton->setText("Start Refresh");
        ui->IrigSlaveAutoRefreshButton->setEnabled(true);
    }
}


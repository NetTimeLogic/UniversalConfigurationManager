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

#include "Ucm_PpsMasterTab.h"
#include "ui_Ucm_PpsMasterTab.h"

Ucm_PpsMasterTab::Ucm_PpsMasterTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_PpsMasterTab();
    ui->setupUi(this);

    pps_master_timer = new QTimer(this);
    pps_master_timer->stop();
    connect(ui->PpsMasterReadValuesButton, SIGNAL(clicked()), this, SLOT(pps_master_read_values_button_clicked()));
    connect(ui->PpsMasterWriteValuesButton, SIGNAL(clicked()), this, SLOT(pps_master_write_values_button_clicked()));
    connect(ui->PpsMasterAutoRefreshButton, SIGNAL(clicked()), this, SLOT(pps_master_auto_refresh_button_clicked()));
    connect(pps_master_timer, SIGNAL(timeout()), this, SLOT(pps_master_read_values_button_clicked()));
}

Ucm_PpsMasterTab::~Ucm_PpsMasterTab()
{
    pps_master_timer->stop();
    delete ui;
    delete pps_master_timer;
}

int Ucm_PpsMasterTab::pps_master_resize(int height, int width)
{
    return 0;
}

void Ucm_PpsMasterTab::pps_master_add_instance(unsigned int instance)
{
    ui->PpsMasterInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_PpsMasterTab::pps_master_enable(void)
{
    return 0;
}

int Ucm_PpsMasterTab::pps_master_disable(void)
{
    pps_master_timer->stop();
    ui->PpsMasterAutoRefreshButton->setText("Start Refresh");
    ui->PpsMasterInstanceComboBox->setEnabled(true);
    ui->PpsMasterReadValuesButton->setEnabled(true);
    ui->PpsMasterWriteValuesButton->setEnabled(true);
    ui->PpsMasterInstanceComboBox->clear();

    ui->PpsMasterPulseWidthValue->setText("NA");
    ui->PpsMasterInvertedCheckBox->setChecked(false);
    ui->PpsMasterEnableCheckBox->setChecked(false);
    ui->PpsMasterVersionValue->setText("NA");

    return 0;
}

void Ucm_PpsMasterTab::pps_master_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->PpsMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PpsMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PpsMasterPulseWidthValue->setText("NA");
            ui->PpsMasterInvertedCheckBox->setChecked(false);
            ui->PpsMasterEnableCheckBox->setChecked(false);
            ui->PpsMasterVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PpsMasterEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->PpsMasterEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->PpsMasterEnableCheckBox->setChecked(false);
    }


    // polarity
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000008, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->PpsMasterInvertedCheckBox->setChecked(true);
        }
        else
        {
            ui->PpsMasterInvertedCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->PpsMasterInvertedCheckBox->setChecked(false);
    }

    // pulse width
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->PpsMasterPulseWidthValue->setText(QString::number((temp_data)));
    }
    else
    {
        ui->PpsMasterPulseWidthValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->PpsMasterVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->PpsMasterVersionValue->setText("NA");
    }
}

void Ucm_PpsMasterTab::pps_master_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->PpsMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PpsMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PpsMasterPulseWidthValue->setText("NA");
            ui->PpsMasterInvertedCheckBox->setChecked(false);
            ui->PpsMasterEnableCheckBox->setChecked(false);
            ui->PpsMasterVersionValue->setText("NA");
            return;
        }
    }


    // pulse width
    temp_string = ui->PpsMasterPulseWidthValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->PpsMasterPulseWidthValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->PpsMasterPulseWidthValue->setText("NA");
    }

    // polarity
    temp_data = 0x00000000; // nothing
    if(false == ui->PpsMasterInvertedCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // no inversion
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000008, temp_data))
    {
        // nothing
    }
    else
    {
        ui->PpsMasterInvertedCheckBox->setChecked(false);
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->PpsMasterEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->PpsMasterEnableCheckBox->setChecked(false);
    }
}

void Ucm_PpsMasterTab::pps_master_read_values_button_clicked(void)
{
    pps_master_read_values();
}

void Ucm_PpsMasterTab::pps_master_write_values_button_clicked(void)
{
    pps_master_write_values();
    pps_master_read_values();
}

void Ucm_PpsMasterTab::pps_master_auto_refresh_button_clicked(void)
{
    if (ui->PpsMasterAutoRefreshButton->text() == "Start Refresh")
    {
        ui->PpsMasterAutoRefreshButton->setEnabled(false);

        ui->PpsMasterInstanceComboBox->setEnabled(false);
        ui->PpsMasterReadValuesButton->setEnabled(false);
        ui->PpsMasterWriteValuesButton->setEnabled(false);

        pps_master_timer->start(1000);

        ui->PpsMasterAutoRefreshButton->setText("Stop Refresh");
        ui->PpsMasterAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->PpsMasterAutoRefreshButton->setEnabled(false);

        pps_master_timer->stop();

        ui->PpsMasterInstanceComboBox->setEnabled(true);
        ui->PpsMasterReadValuesButton->setEnabled(true);
        ui->PpsMasterWriteValuesButton->setEnabled(true);

        ui->PpsMasterAutoRefreshButton->setText("Start Refresh");
        ui->PpsMasterAutoRefreshButton->setEnabled(true);
    }
}


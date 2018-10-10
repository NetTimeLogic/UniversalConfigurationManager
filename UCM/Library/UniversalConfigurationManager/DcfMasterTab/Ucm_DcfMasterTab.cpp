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

#include <Ucm_DcfMasterTab.h>
#include <ui_Ucm_DcfMasterTab.h>

Ucm_DcfMasterTab::Ucm_DcfMasterTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_DcfMasterTab();
    ui->setupUi(this);

    dcf_master_timer = new QTimer(this);
    dcf_master_timer->stop();
    connect(ui->DcfMasterReadValuesButton, SIGNAL(clicked()), this, SLOT(dcf_master_read_values_button_clicked()));
    connect(ui->DcfMasterWriteValuesButton, SIGNAL(clicked()), this, SLOT(dcf_master_write_values_button_clicked()));
    connect(ui->DcfMasterAutoRefreshButton, SIGNAL(clicked()), this, SLOT(dcf_master_auto_refresh_button_clicked()));
    connect(dcf_master_timer, SIGNAL(timeout()), this, SLOT(dcf_master_read_values_button_clicked()));
}

Ucm_DcfMasterTab::~Ucm_DcfMasterTab()
{
    dcf_master_timer->stop();
    delete ui;
    delete dcf_master_timer;
}

int Ucm_DcfMasterTab::dcf_master_resize(int height, int width)
{
    return 0;
}

void Ucm_DcfMasterTab::dcf_master_add_instance(unsigned int instance)
{
    ui->DcfMasterInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_DcfMasterTab::dcf_master_enable(void)
{
    return 0;
}

int Ucm_DcfMasterTab::dcf_master_disable(void)
{
    dcf_master_timer->stop();
    ui->DcfMasterAutoRefreshButton->setText("Start Refresh");
    ui->DcfMasterInstanceComboBox->setEnabled(true);
    ui->DcfMasterReadValuesButton->setEnabled(true);
    ui->DcfMasterWriteValuesButton->setEnabled(true);
    ui->DcfMasterInstanceComboBox->clear();

    ui->DcfMasterCorrectionValue->setText("NA");
    ui->DcfMasterEnableCheckBox->setChecked(false);
    ui->DcfMasterVersionValue->setText("NA");

    return 0;
}

void Ucm_DcfMasterTab::dcf_master_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->DcfMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_DcfMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->DcfMasterCorrectionValue->setText("NA");
            ui->DcfMasterEnableCheckBox->setChecked(false);
            ui->DcfMasterVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfMaster_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->DcfMasterEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->DcfMasterEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->DcfMasterEnableCheckBox->setChecked(false);
    }


    // correction
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfMaster_CorrectionReg, temp_data))
    {
        ui->DcfMasterCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->DcfMasterCorrectionValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_DcfMaster_VersionReg, temp_data))
    {
        ui->DcfMasterVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->DcfMasterVersionValue->setText("NA");
    }
}

void Ucm_DcfMasterTab::dcf_master_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->DcfMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_DcfMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->DcfMasterCorrectionValue->setText("NA");
            ui->DcfMasterEnableCheckBox->setChecked(false);
            ui->DcfMasterVersionValue->setText("NA");
            return;
        }
    }


    // correction
    temp_string = ui->DcfMasterCorrectionValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_DcfMaster_CorrectionReg, temp_data))
    {
        ui->DcfMasterCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->DcfMasterCorrectionValue->setText("NA");
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->DcfMasterEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_DcfMaster_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->DcfMasterEnableCheckBox->setChecked(false);
    }
}

void Ucm_DcfMasterTab::dcf_master_read_values_button_clicked(void)
{
    dcf_master_read_values();
}

void Ucm_DcfMasterTab::dcf_master_write_values_button_clicked(void)
{
    dcf_master_write_values();
    dcf_master_read_values();
}

void Ucm_DcfMasterTab::dcf_master_auto_refresh_button_clicked(void)
{
    if (ui->DcfMasterAutoRefreshButton->text() == "Start Refresh")
    {
        ui->DcfMasterAutoRefreshButton->setEnabled(false);

        ui->DcfMasterInstanceComboBox->setEnabled(false);
        ui->DcfMasterReadValuesButton->setEnabled(false);
        ui->DcfMasterWriteValuesButton->setEnabled(false);

        dcf_master_timer->start(1000);

        ui->DcfMasterAutoRefreshButton->setText("Stop Refresh");
        ui->DcfMasterAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->DcfMasterAutoRefreshButton->setEnabled(false);

        dcf_master_timer->stop();

        ui->DcfMasterInstanceComboBox->setEnabled(true);
        ui->DcfMasterReadValuesButton->setEnabled(true);
        ui->DcfMasterWriteValuesButton->setEnabled(true);

        ui->DcfMasterAutoRefreshButton->setText("Start Refresh");
        ui->DcfMasterAutoRefreshButton->setEnabled(true);
    }
}


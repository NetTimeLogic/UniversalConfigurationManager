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

#include "Ucm_IrigMasterTab.h"
#include "ui_Ucm_IrigMasterTab.h"

Ucm_IrigMasterTab::Ucm_IrigMasterTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_IrigMasterTab();
    ui->setupUi(this);

    irig_master_timer = new QTimer(this);
    irig_master_timer->stop();
    connect(ui->IrigMasterReadValuesButton, SIGNAL(clicked()), this, SLOT(irig_master_read_values_button_clicked()));
    connect(ui->IrigMasterWriteValuesButton, SIGNAL(clicked()), this, SLOT(irig_master_write_values_button_clicked()));
    connect(ui->IrigMasterAutoRefreshButton, SIGNAL(clicked()), this, SLOT(irig_master_auto_refresh_button_clicked()));
    connect(irig_master_timer, SIGNAL(timeout()), this, SLOT(irig_master_read_values_button_clicked()));
}

Ucm_IrigMasterTab::~Ucm_IrigMasterTab()
{
    irig_master_timer->stop();
    delete ui;
    delete irig_master_timer;
}

int Ucm_IrigMasterTab::irig_master_resize(int height, int width)
{
    return 0;
}

void Ucm_IrigMasterTab::irig_master_add_instance(unsigned int instance)
{
    ui->IrigMasterInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_IrigMasterTab::irig_master_enable(void)
{
    return 0;
}

int Ucm_IrigMasterTab::irig_master_disable(void)
{
    irig_master_timer->stop();
    ui->IrigMasterAutoRefreshButton->setText("Start Refresh");
    ui->IrigMasterInstanceComboBox->setEnabled(true);
    ui->IrigMasterReadValuesButton->setEnabled(true);
    ui->IrigMasterWriteValuesButton->setEnabled(true);
    ui->IrigMasterInstanceComboBox->clear();

    ui->IrigMasterCorrectionValue->setText("NA");
    ui->IrigMasterModeComboBox->setCurrentText("NA");
    ui->IrigMasterControlBitsValue->setText("NA");
    ui->IrigMasterEnableCheckBox->setChecked(false);
    ui->IrigMasterVersionValue->setText("NA");

    return 0;
}

void Ucm_IrigMasterTab::irig_master_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->IrigMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_IrigMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->IrigMasterCorrectionValue->setText("NA");
            ui->IrigMasterModeComboBox->setCurrentText("NA");
            ui->IrigMasterControlBitsValue->setText("NA");
            ui->IrigMasterEnableCheckBox->setChecked(false);
            ui->IrigMasterVersionValue->setText("NA");
            return;
        }
    }

    // enabled & mode
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IrigMaster_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->IrigMasterEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->IrigMasterEnableCheckBox->setChecked(true);
        }

        temp_data = ((temp_data >> 16) & 0x00000007);

        switch (temp_data)
        {
            case 0:
                ui->IrigMasterModeComboBox->setCurrentText("B000");
                break;
            case 1:
                ui->IrigMasterModeComboBox->setCurrentText("B001");
                break;
            case 2:
                ui->IrigMasterModeComboBox->setCurrentText("B002");
                break;
            case 3:
                ui->IrigMasterModeComboBox->setCurrentText("B003");
                break;
            case 4:
                ui->IrigMasterModeComboBox->setCurrentText("B004");
                break;
            case 5:
                ui->IrigMasterModeComboBox->setCurrentText("B005");
                break;
            case 6:
                ui->IrigMasterModeComboBox->setCurrentText("B006");
                break;
            case 7:
                ui->IrigMasterModeComboBox->setCurrentText("B007");
                break;
            default:
                ui->IrigMasterModeComboBox->setCurrentText("NA");
                break;
        }

    }
    else
    {
        ui->IrigMasterModeComboBox->setCurrentText("NA");
        ui->IrigMasterEnableCheckBox->setChecked(false);
    }


    // control bits
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IrigMaster_ControlBitsReg, temp_data))
    {
        ui->IrigMasterControlBitsValue->setText(QString("0x%1").arg(temp_data, 7, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IrigMasterControlBitsValue->setText("NA");
    }

    // correction
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IrigMaster_CorrectionReg, temp_data))
    {
        ui->IrigMasterCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IrigMasterCorrectionValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IrigMaster_VersionReg, temp_data))
    {
        ui->IrigMasterVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->IrigMasterVersionValue->setText("NA");
    }
}

void Ucm_IrigMasterTab::irig_master_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->IrigMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_IrigMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->IrigMasterCorrectionValue->setText("NA");
            ui->IrigMasterModeComboBox->setCurrentText("NA");
            ui->IrigMasterControlBitsValue->setText("NA");
            ui->IrigMasterEnableCheckBox->setChecked(false);
            ui->IrigMasterVersionValue->setText("NA");
            return;
        }
    }


    // correction
    temp_string = ui->IrigMasterCorrectionValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_IrigMaster_CorrectionReg, temp_data))
    {
        ui->IrigMasterCorrectionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IrigMasterCorrectionValue->setText("NA");
    }

    // control bits
    temp_string = ui->IrigMasterControlBitsValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_IrigMaster_ControlBitsReg, temp_data))
    {
        ui->IrigMasterControlBitsValue->setText(QString("0x%1").arg(temp_data, 7, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IrigMasterControlBitsValue->setText("NA");
    }

    // mode and enable
    temp_string = ui->IrigMasterModeComboBox->currentText();
    if (temp_string == "B000")
    {
        temp_data = 0x00000000;
    }
    else if (temp_string == "B001")
    {
        temp_data = 0x00010000;
    }
    else if (temp_string == "B002")
    {
        temp_data = 0x00020000;
    }
    else if (temp_string == "B003")
    {
        temp_data = 0x00030000;
    }
    else if (temp_string == "B004")
    {
        temp_data = 0x00040000;
    }
    else if (temp_string == "B005")
    {
        temp_data = 0x00050000;
    }
    else if (temp_string == "B006")
    {
        temp_data = 0x00060000;
    }
    else if (temp_string == "B007")
    {
        temp_data = 0x00070000;
    }
    else
    {
        temp_data = 0x00070000; // default to B007
    }

    if(true == ui->IrigMasterEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_IrigMaster_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->IrigMasterEnableCheckBox->setChecked(false);
    }
}

void Ucm_IrigMasterTab::irig_master_read_values_button_clicked(void)
{
    irig_master_read_values();
}

void Ucm_IrigMasterTab::irig_master_write_values_button_clicked(void)
{
    irig_master_write_values();
    irig_master_read_values();
}

void Ucm_IrigMasterTab::irig_master_auto_refresh_button_clicked(void)
{
    if (ui->IrigMasterAutoRefreshButton->text() == "Start Refresh")
    {
        ui->IrigMasterAutoRefreshButton->setEnabled(false);

        ui->IrigMasterInstanceComboBox->setEnabled(false);
        ui->IrigMasterReadValuesButton->setEnabled(false);
        ui->IrigMasterWriteValuesButton->setEnabled(false);

        irig_master_timer->start(1000);

        ui->IrigMasterAutoRefreshButton->setText("Stop Refresh");
        ui->IrigMasterAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->IrigMasterAutoRefreshButton->setEnabled(false);

        irig_master_timer->stop();

        ui->IrigMasterInstanceComboBox->setEnabled(true);
        ui->IrigMasterReadValuesButton->setEnabled(true);
        ui->IrigMasterWriteValuesButton->setEnabled(true);

        ui->IrigMasterAutoRefreshButton->setText("Start Refresh");
        ui->IrigMasterAutoRefreshButton->setEnabled(true);
    }
}


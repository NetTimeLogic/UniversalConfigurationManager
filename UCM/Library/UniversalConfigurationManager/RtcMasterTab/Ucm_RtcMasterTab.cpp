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

#include "Ucm_RtcMasterTab.h"
#include "ui_Ucm_RtcMasterTab.h"

Ucm_RtcMasterTab::Ucm_RtcMasterTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_RtcMasterTab();
    ui->setupUi(this);

    rtc_master_timer = new QTimer(this);
    rtc_master_timer->stop();
    connect(ui->RtcMasterReadValuesButton, SIGNAL(clicked()), this, SLOT(rtc_master_read_values_button_clicked()));
    connect(ui->RtcMasterWriteValuesButton, SIGNAL(clicked()), this, SLOT(rtc_master_write_values_button_clicked()));
    connect(ui->RtcMasterAutoRefreshButton, SIGNAL(clicked()), this, SLOT(rtc_master_auto_refresh_button_clicked()));
    connect(rtc_master_timer, SIGNAL(timeout()), this, SLOT(rtc_master_read_values_button_clicked()));
}

Ucm_RtcMasterTab::~Ucm_RtcMasterTab()
{
    rtc_master_timer->stop();
    delete ui;
    delete rtc_master_timer;
}

int Ucm_RtcMasterTab::rtc_master_resize(int height, int width)
{
    return 0;
}

void Ucm_RtcMasterTab::rtc_master_add_instance(unsigned int instance)
{
    ui->RtcMasterInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_RtcMasterTab::rtc_master_enable(void)
{
    return 0;
}

int Ucm_RtcMasterTab::rtc_master_disable(void)
{
    rtc_master_timer->stop();
    ui->RtcMasterAutoRefreshButton->setText("Start Refresh");
    ui->RtcMasterInstanceComboBox->setEnabled(true);
    ui->RtcMasterReadValuesButton->setEnabled(true);
    ui->RtcMasterWriteValuesButton->setEnabled(true);
    ui->RtcMasterInstanceComboBox->clear();

    ui->RtcMasterSecondsValue->setText("NA");
    ui->RtcMasterNanosecondsValue->setText("NA");
    ui->RtcMasterInvertedCheckBox->setChecked(false);
    ui->RtcMasterEnableCheckBox->setChecked(false);
    ui->RtcMasterVersionValue->setText("NA");

    return 0;
}

void Ucm_RtcMasterTab::rtc_master_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->RtcMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RtcMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->RtcMasterSecondsValue->setText("NA");
            ui->RtcMasterNanosecondsValue->setText("NA");
            ui->RtcMasterInvertedCheckBox->setChecked(false);
            ui->RtcMasterEnableCheckBox->setChecked(false);
            ui->RtcMasterVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RtcMaster_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->RtcMasterEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->RtcMasterEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->RtcMasterEnableCheckBox->setChecked(false);
    }

    temp_data = 0x40000000;
    if(true == ui->RtcMasterEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001;
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RtcMaster_ControlReg, temp_data))
    {
        for (int i = 0; i < 100; i++)
        {
            if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_RtcMaster_ControlReg, temp_data))
            {
                if ((temp_data & 0x80000000) != 0)
                {
                    // seconds
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RtcMaster_TimeReadValueHReg, temp_data))
                    {
                        ui->RtcMasterSecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->RtcMasterSecondsValue->setText("NA");
                    }

                    // nanoseconds
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RtcMaster_TimeReadValueLReg, temp_data))
                    {
                        ui->RtcMasterNanosecondsValue->setText(QString::number(temp_data));
                    }
                    else
                    {
                        ui->RtcMasterNanosecondsValue->setText("NA");
                    }

                    break;
                }
                else if (i == 9)
                {
                    cout << "ERROR: " << "read did not complete" << endl;
                    ui->RtcMasterSecondsValue->setText("NA");
                    ui->RtcMasterNanosecondsValue->setText("NA");
                }

            }
            else
            {
                ui->RtcMasterSecondsValue->setText("NA");
                ui->RtcMasterNanosecondsValue->setText("NA");
            }
        }
    }
    else
    {
        ui->RtcMasterSecondsValue->setText("NA");
        ui->RtcMasterNanosecondsValue->setText("NA");
    }


    // polarity
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RtcMaster_PolarityReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->RtcMasterInvertedCheckBox->setChecked(true);
        }
        else
        {
            ui->RtcMasterInvertedCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RtcMasterInvertedCheckBox->setChecked(false);
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RtcMaster_VersionReg, temp_data))
    {
        ui->RtcMasterVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->RtcMasterVersionValue->setText("NA");
    }
}

void Ucm_RtcMasterTab::rtc_master_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->RtcMasterInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RtcMasterCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->RtcMasterSecondsValue->setText("NA");
            ui->RtcMasterNanosecondsValue->setText("NA");
            ui->RtcMasterInvertedCheckBox->setChecked(false);
            ui->RtcMasterEnableCheckBox->setChecked(false);
            ui->RtcMasterVersionValue->setText("NA");
            return;
        }
    }

    // seconds
    temp_string = ui->RtcMasterSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RtcMaster_TimeWriteValueHReg, temp_data))
    {
        ui->RtcMasterSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RtcMasterSecondsValue->setText("NA");
    }

    // nanoseconds
    temp_string = ui->RtcMasterNanosecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RtcMaster_TimeWriteValueLReg, temp_data))
    {
        ui->RtcMasterNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RtcMasterNanosecondsValue->setText("NA");
    }

    // polarity
    temp_data = 0x00000000; // nothing
    if(false == ui->RtcMasterInvertedCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // no inversion
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RtcMaster_PolarityReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->RtcMasterInvertedCheckBox->setChecked(false);
    }

    temp_data = 0x00000002; // set time
    if(true == ui->RtcMasterEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RtcMaster_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->RtcMasterSecondsValue->setText("NA");
        ui->RtcMasterNanosecondsValue->setText("NA");
    }
}

void Ucm_RtcMasterTab::rtc_master_read_values_button_clicked(void)
{
    rtc_master_read_values();
}

void Ucm_RtcMasterTab::rtc_master_write_values_button_clicked(void)
{
    rtc_master_write_values();
    rtc_master_read_values();
}

void Ucm_RtcMasterTab::rtc_master_auto_refresh_button_clicked(void)
{
    if (ui->RtcMasterAutoRefreshButton->text() == "Start Refresh")
    {
        ui->RtcMasterAutoRefreshButton->setEnabled(false);

        ui->RtcMasterInstanceComboBox->setEnabled(false);
        ui->RtcMasterReadValuesButton->setEnabled(false);
        ui->RtcMasterWriteValuesButton->setEnabled(false);

        rtc_master_timer->start(1000);

        ui->RtcMasterAutoRefreshButton->setText("Stop Refresh");
        ui->RtcMasterAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->RtcMasterAutoRefreshButton->setEnabled(false);

        rtc_master_timer->stop();

        ui->RtcMasterInstanceComboBox->setEnabled(true);
        ui->RtcMasterReadValuesButton->setEnabled(true);
        ui->RtcMasterWriteValuesButton->setEnabled(true);

        ui->RtcMasterAutoRefreshButton->setText("Start Refresh");
        ui->RtcMasterAutoRefreshButton->setEnabled(true);
    }
}


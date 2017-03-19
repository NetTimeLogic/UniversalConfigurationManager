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

#include "Ucm_ClkSgTab.h"
#include "ui_Ucm_ClkSgTab.h"

Ucm_ClkSgTab::Ucm_ClkSgTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_ClkSgTab();
    ui->setupUi(this);

    clk_sg_timer = new QTimer(this);
    clk_sg_timer->stop();
    connect(ui->ClkSgReadValuesButton, SIGNAL(clicked()), this, SLOT(clk_sg_read_values_button_clicked()));
    connect(ui->ClkSgWriteValuesButton, SIGNAL(clicked()), this, SLOT(clk_sg_write_values_button_clicked()));
    connect(ui->ClkSgAutoRefreshButton, SIGNAL(clicked()), this, SLOT(clk_sg_auto_refresh_button_clicked()));
    connect(clk_sg_timer, SIGNAL(timeout()), this, SLOT(clk_sg_read_values_button_clicked()));
}

Ucm_ClkSgTab::~Ucm_ClkSgTab()
{
    clk_sg_timer->stop();
    delete ui;
    delete clk_sg_timer;
}

void Ucm_ClkSgTab::clk_sg_add_instance(unsigned int instance)
{
    ui->ClkSgInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_ClkSgTab::clk_sg_enable(void)
{
    return 0;
}

int Ucm_ClkSgTab::clk_sg_disable(void)
{
    clk_sg_timer->stop();
    ui->ClkSgAutoRefreshButton->setText("Start Refresh");
    ui->ClkSgReadValuesButton->setEnabled(true);
    ui->ClkSgWriteValuesButton->setEnabled(true);
    ui->ClkSgInstanceComboBox->clear();

    ui->ClkSgInvertedCheckBox->setChecked(false);
    ui->ClkSgStartSecondsValue->setText("NA");
    ui->ClkSgStartNanosecondsValue->setText("NA");
    ui->ClkSgPulseSecondsValue->setText("NA");
    ui->ClkSgPulseNanosecondsValue->setText("NA");
    ui->ClkSgPeriodSecondsValue->setText("NA");
    ui->ClkSgPeriodNanosecondsValue->setText("NA");
    ui->ClkSgRepeatCountValue->setText("NA");
    ui->ClkSgEnableCheckBox->setChecked(false);
    ui->ClkSgVersionValue->setText("NA");

    return 0;
}

void Ucm_ClkSgTab::clk_sg_read_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->ClkSgInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_ClkSignalGeneratorCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->ClkSgInvertedCheckBox->setChecked(false);
            ui->ClkSgStartSecondsValue->setText("NA");
            ui->ClkSgStartNanosecondsValue->setText("NA");
            ui->ClkSgPulseSecondsValue->setText("NA");
            ui->ClkSgPulseNanosecondsValue->setText("NA");
            ui->ClkSgPeriodSecondsValue->setText("NA");
            ui->ClkSgPeriodNanosecondsValue->setText("NA");
            ui->ClkSgRepeatCountValue->setText("NA");
            ui->ClkSgEnableCheckBox->setChecked(false);
            ui->ClkSgVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000000, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkSgEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->ClkSgEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->ClkSgEnableCheckBox->setChecked(false);
    }

    // polarity
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000008, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->ClkSgInvertedCheckBox->setChecked(true);
        }
        else
        {
            ui->ClkSgInvertedCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->ClkSgInvertedCheckBox->setChecked(false);
    }

    // start seconds
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000014, temp_data))
    {
        ui->ClkSgStartSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgStartSecondsValue->setText("NA");
    }

    // start nanoseconds
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->ClkSgStartNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgStartNanosecondsValue->setText("NA");
    }

    // pulse seconds
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000001C, temp_data))
    {
        ui->ClkSgPulseSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPulseSecondsValue->setText("NA");
    }

    // pulse nanoseconds
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000018, temp_data))
    {
        ui->ClkSgPulseNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPulseNanosecondsValue->setText("NA");
    }

    // pulse seconds
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000024, temp_data))
    {
        ui->ClkSgPeriodSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPeriodSecondsValue->setText("NA");
    }

    // period nanoseconds
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->ClkSgPeriodNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPeriodNanosecondsValue->setText("NA");
    }

    // repeat count
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x00000028, temp_data))
    {
        ui->ClkSgRepeatCountValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgRepeatCountValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + 0x0000000C, temp_data))
    {
        ui->ClkSgVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->ClkSgVersionValue->setText("NA");
    }
}

void Ucm_ClkSgTab::clk_sg_write_values(void)
{
    unsigned int temp_data;
    unsigned int temp_addr;
    QString temp_string;

    temp_string = ui->ClkSgInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_ClkSignalGeneratorCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->ClkSgInvertedCheckBox->setChecked(false);
            ui->ClkSgStartSecondsValue->setText("NA");
            ui->ClkSgStartNanosecondsValue->setText("NA");
            ui->ClkSgPulseSecondsValue->setText("NA");
            ui->ClkSgPulseNanosecondsValue->setText("NA");
            ui->ClkSgPeriodSecondsValue->setText("NA");
            ui->ClkSgPeriodNanosecondsValue->setText("NA");
            ui->ClkSgRepeatCountValue->setText("NA");
            ui->ClkSgEnableCheckBox->setChecked(false);
            ui->ClkSgVersionValue->setText("NA");
            return;
            return;
        }
    }

    // polarity
    temp_data = 0x00000000; // nothing
    if(false == ui->ClkSgInvertedCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // no inversion
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000008, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkSgInvertedCheckBox->setChecked(false);
    }


    // start seconds
    temp_string = ui->ClkSgStartSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000014, temp_data))
    {
        ui->ClkSgStartSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgStartSecondsValue->setText("NA");
    }

    // start nanoseconds
    temp_string = ui->ClkSgStartNanosecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000010, temp_data))
    {
        ui->ClkSgStartNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgStartNanosecondsValue->setText("NA");
    }

    // pulse seconds
    temp_string = ui->ClkSgPulseSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x0000001C, temp_data))
    {
        ui->ClkSgPulseSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPulseSecondsValue->setText("NA");
    }

    // pulse nanoseconds
    temp_string = ui->ClkSgPulseNanosecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000018, temp_data))
    {
        ui->ClkSgPulseNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPulseNanosecondsValue->setText("NA");
    }

    // period seconds
    temp_string = ui->ClkSgPeriodSecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000024, temp_data))
    {
        ui->ClkSgPeriodSecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPeriodSecondsValue->setText("NA");
    }

    // period nanoseconds
    temp_string = ui->ClkSgPeriodNanosecondsValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000020, temp_data))
    {
        ui->ClkSgPeriodNanosecondsValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgPeriodNanosecondsValue->setText("NA");
    }

    // repeat count
    temp_string = ui->ClkSgRepeatCountValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000028, temp_data))
    {
        ui->ClkSgRepeatCountValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->ClkSgRepeatCountValue->setText("NA");
    }

    temp_data = 0x00000002; // set time
    if(true == ui->ClkSgEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + 0x00000000, temp_data))
    {
        // nothing
    }
    else
    {
        ui->ClkSgEnableCheckBox->setChecked(false);
    }
}

void Ucm_ClkSgTab::clk_sg_read_values_button_clicked(void)
{
    clk_sg_read_values();
}

void Ucm_ClkSgTab::clk_sg_write_values_button_clicked(void)
{
    clk_sg_write_values();
    clk_sg_read_values();
}

void Ucm_ClkSgTab::clk_sg_auto_refresh_button_clicked(void)
{
    if (ui->ClkSgAutoRefreshButton->text() == "Start Refresh")
    {
        ui->ClkSgAutoRefreshButton->setEnabled(false);

        ui->ClkSgReadValuesButton->setEnabled(false);
        ui->ClkSgWriteValuesButton->setEnabled(false);

        clk_sg_timer->start(1000);

        ui->ClkSgAutoRefreshButton->setText("Stop Refresh");
        ui->ClkSgAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->ClkSgAutoRefreshButton->setEnabled(false);

        clk_sg_timer->stop();

        ui->ClkSgReadValuesButton->setEnabled(true);
        ui->ClkSgWriteValuesButton->setEnabled(true);

        ui->ClkSgAutoRefreshButton->setText("Start Refresh");
        ui->ClkSgAutoRefreshButton->setEnabled(true);
    }
}


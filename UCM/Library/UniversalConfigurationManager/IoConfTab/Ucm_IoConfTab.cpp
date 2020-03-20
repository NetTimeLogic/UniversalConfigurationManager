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

#include <Ucm_IoConfTab.h>
#include <ui_Ucm_IoConfTab.h>

Ucm_IoConfTab::Ucm_IoConfTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_IoConfTab();
    ui->setupUi(this);

    io_conf_timer = new QTimer(this);
    io_conf_timer->stop();
    connect(ui->IoConfReadValuesButton, SIGNAL(clicked()), this, SLOT(io_conf_read_values_button_clicked()));
    connect(ui->IoConfWriteValuesButton, SIGNAL(clicked()), this, SLOT(io_conf_write_values_button_clicked()));
    connect(ui->IoConfAutoRefreshButton, SIGNAL(clicked()), this, SLOT(io_conf_auto_refresh_button_clicked()));
    connect(io_conf_timer, SIGNAL(timeout()), this, SLOT(io_conf_read_values_button_clicked()));
}

Ucm_IoConfTab::~Ucm_IoConfTab()
{
    io_conf_timer->stop();
    delete ui;
    delete io_conf_timer;
}

int Ucm_IoConfTab::io_conf_resize(int height, int width)
{
    return 0;
}

void Ucm_IoConfTab::io_conf_add_instance(unsigned int instance)
{
    ui->IoConfInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_IoConfTab::io_conf_enable(void)
{
    return 0;
}

int Ucm_IoConfTab::io_conf_disable(void)
{
    io_conf_timer->stop();
    ui->IoConfAutoRefreshButton->setText("Start Refresh");
    ui->IoConfInstanceComboBox->setEnabled(true);
    ui->IoConfReadValuesButton->setEnabled(true);
    ui->IoConfWriteValuesButton->setEnabled(true);
    ui->IoConfInstanceComboBox->clear();

    ui->IoConfOutputDataValue->setText("NA");
    ui->IoConfOutputEnableValue->setText("NA");
    ui->IoConfInputDataValue->setText("NA");
    ui->IoConfVersionValue->setText("NA");

    return 0;
}

void Ucm_IoConfTab::io_conf_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->IoConfInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_IoConfigurationCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->IoConfOutputDataValue->setText("NA");
            ui->IoConfOutputEnableValue->setText("NA");
            ui->IoConfInputDataValue->setText("NA");
            ui->IoConfVersionValue->setText("NA");
            return;
        }
    }

    // output data
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IoConf_OutputDataReg, temp_data))
    {
        ui->IoConfOutputDataValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IoConfOutputDataValue->setText("NA");
    }

    // output enable
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IoConf_OutputEnableReg, temp_data))
    {
        ui->IoConfOutputEnableValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IoConfOutputEnableValue->setText("NA");
    }

    // input data
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_IoConf_InputDataReg, temp_data))
    {
        ui->IoConfInputDataValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->IoConfInputDataValue->setText("NA");
    }
}

void Ucm_IoConfTab::io_conf_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->IoConfInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_IoConfigurationCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->IoConfOutputDataValue->setText("NA");
            ui->IoConfOutputEnableValue->setText("NA");
            ui->IoConfInputDataValue->setText("NA");
            ui->IoConfVersionValue->setText("NA");
            return;
        }
    }


    // output data
    temp_string = ui->IoConfOutputDataValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_IoConf_OutputDataReg, temp_data))
    {
        //nothing
    }
    else
    {
        ui->IoConfOutputDataValue->setText("NA");
    }

    // output enable
    temp_string = ui->IoConfOutputEnableValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_IoConf_OutputEnableReg, temp_data))
    {
        //nothing
    }
    else
    {
        ui->IoConfOutputEnableValue->setText("NA");
    }
}

void Ucm_IoConfTab::io_conf_read_values_button_clicked(void)
{
    io_conf_read_values();
}

void Ucm_IoConfTab::io_conf_write_values_button_clicked(void)
{
    io_conf_write_values();
    io_conf_read_values();
}

void Ucm_IoConfTab::io_conf_auto_refresh_button_clicked(void)
{
    if (ui->IoConfAutoRefreshButton->text() == "Start Refresh")
    {
        ui->IoConfAutoRefreshButton->setEnabled(false);

        ui->IoConfInstanceComboBox->setEnabled(false);
        ui->IoConfReadValuesButton->setEnabled(false);
        ui->IoConfWriteValuesButton->setEnabled(false);

        io_conf_timer->start(1000);

        ui->IoConfAutoRefreshButton->setText("Stop Refresh");
        ui->IoConfAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->IoConfAutoRefreshButton->setEnabled(false);

        io_conf_timer->stop();

        ui->IoConfInstanceComboBox->setEnabled(true);
        ui->IoConfReadValuesButton->setEnabled(true);
        ui->IoConfWriteValuesButton->setEnabled(true);

        ui->IoConfAutoRefreshButton->setText("Start Refresh");
        ui->IoConfAutoRefreshButton->setEnabled(true);
    }
}


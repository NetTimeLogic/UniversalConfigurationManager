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

#include <Ucm_I2cConfTab.h>
#include <ui_Ucm_I2cConfTab.h>

Ucm_I2cConfTab::Ucm_I2cConfTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_I2cConfTab();
    ui->setupUi(this);

    i2c_conf_timer = new QTimer(this);
    i2c_conf_timer->stop();
    connect(ui->I2cConfReadValuesButton, SIGNAL(clicked()), this, SLOT(i2c_conf_read_values_button_clicked()));
    connect(ui->I2cConfWriteValuesButton, SIGNAL(clicked()), this, SLOT(i2c_conf_write_values_button_clicked()));
    connect(ui->I2cConfAutoRefreshButton, SIGNAL(clicked()), this, SLOT(i2c_conf_auto_refresh_button_clicked()));
    connect(i2c_conf_timer, SIGNAL(timeout()), this, SLOT(i2c_conf_read_values_button_clicked()));
}

Ucm_I2cConfTab::~Ucm_I2cConfTab()
{
    i2c_conf_timer->stop();
    delete ui;
    delete i2c_conf_timer;
}

int Ucm_I2cConfTab::i2c_conf_resize(int height, int width)
{
    return 0;
}

void Ucm_I2cConfTab::i2c_conf_add_instance(unsigned int instance)
{
    ui->I2cConfInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_I2cConfTab::i2c_conf_enable(void)
{
    return 0;
}

int Ucm_I2cConfTab::i2c_conf_disable(void)
{
    i2c_conf_timer->stop();
    ui->I2cConfAutoRefreshButton->setText("Start Refresh");
    ui->I2cConfInstanceComboBox->setEnabled(true);
    ui->I2cConfReadValuesButton->setEnabled(true);
    ui->I2cConfWriteValuesButton->setEnabled(true);
    ui->I2cConfInstanceComboBox->clear();

    ui->I2cConfChipAddrValue->setText("NA");
    ui->I2cConfRegAddrValue->setText("NA");
    ui->I2cConfRegDataValue->setText("NA");
    ui->I2cConfVersionValue->setText("NA");

    return 0;
}

void Ucm_I2cConfTab::i2c_conf_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->I2cConfInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_I2cConfigurationCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->I2cConfChipAddrValue->setText("NA");
            ui->I2cConfRegAddrValue->setText("NA");
            ui->I2cConfRegDataValue->setText("NA");
            ui->I2cConfVersionValue->setText("NA");
            return;
        }
    }

    // i2c addr
    temp_string = ui->I2cConfChipAddrValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        ui->I2cConfRegDataValue->setText("NA");
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ChipAddrReg, temp_data))
    {
        ui->I2cConfChipAddrValue->setText(QString::number(temp_data));
        
        // reg addr
        temp_string = ui->I2cConfRegAddrValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            ui->I2cConfRegDataValue->setText("NA");
        }
        else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_RegAddrReg, temp_data))
        {
            ui->I2cConfRegAddrValue->setText(QString::number(temp_data));
            
            // control read and wait
            temp_data = 0x00000002; // read
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data))
            {
                for (int i = 0; i < 10000; i++) // give it enough time
                {
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data))
                    {
                        if ((temp_data & 0x20000002) == 0x20000000)
                        {
                            // read done
                            if ((temp_data & 0x40000000) == 0x40000000)
                            {
                                cout << "ERROR: " << "read error" << endl;
                                temp_data = 0x00000000; // read clear
                                ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data);
                                ui->I2cConfRegDataValue->setText("NA");
                            }
                            else if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_I2cConf_DataReg, temp_data)) // read data
                            {
                                ui->I2cConfRegDataValue->setText(QString("0x%1").arg(temp_data, 2, 16, QLatin1Char('0')));
                            }
                            else
                            {
                                ui->I2cConfRegDataValue->setText("NA");
                            }
                            // read done
                            break;
                        }
                        else if (i == 9999)// timed out
                        {
                            cout << "ERROR: " << "read did not complete" << endl;
                            temp_data = 0x00000000; // read clear
                            ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data);
                            ui->I2cConfRegDataValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->I2cConfRegDataValue->setText("NA");
                    }    
                }
            }
            else
            {
                ui->I2cConfRegDataValue->setText("NA");
            }    
        }
        else
        {
            ui->I2cConfRegAddrValue->setText("NA");
        }
    }
    else
    {
        ui->I2cConfChipAddrValue->setText("NA");
    }
}

void Ucm_I2cConfTab::i2c_conf_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->I2cConfInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_I2cConfigurationCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->I2cConfChipAddrValue->setText("NA");
            ui->I2cConfRegAddrValue->setText("NA");
            ui->I2cConfRegDataValue->setText("NA");
            ui->I2cConfVersionValue->setText("NA");
            return;
            return;
        }
    }

    // i2c addr
    temp_string = ui->I2cConfChipAddrValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ChipAddrReg, temp_data))
    {
        ui->I2cConfChipAddrValue->setText(QString::number(temp_data));
        
        // reg addr
        temp_string = ui->I2cConfRegAddrValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            //nothing
        }
        else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_RegAddrReg, temp_data))
        {
            ui->I2cConfRegAddrValue->setText(QString::number(temp_data));
            
            // write data
            temp_string = ui->I2cConfRegDataValue->text();
            temp_data = temp_string.toUInt(nullptr, 16);
            if (temp_string == "NA")
            {
                //nothing
            }
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_DataReg, temp_data))
            {
                ui->I2cConfRegDataValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                
                // control write and wait
                temp_data = 0x00000001; // write
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data))
                {
                    for (int i = 0; i < 10000; i++) // give it enough time
                    {
                        if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data))
                        {
                            if ((temp_data & 0x10000001) == 0x10000000)
                            {
                                // write done
                                if ((temp_data & 0x40000000) == 0x40000000)
                                {
                                    cout << "ERROR: " << "write error" << endl;
                                    temp_data = 0x00000000; // write clear
                                    ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data);
                                }
                                break;
                            }
                            else if (i == 9999) // timed out
                            {
                                cout << "ERROR: " << "write did not complete" << endl;
                                temp_data = 0x00000000; // write clear
                                ucm->com_lib.write_reg(temp_addr + Ucm_I2cConf_ControlReg, temp_data);
                            }
                        }
                        else
                        {
                            ui->I2cConfRegDataValue->setText("NA");
                        }    
                    }
                }
                else
                {
                    ui->I2cConfRegDataValue->setText("NA");
                }    
            }
            else
            {
                ui->I2cConfRegDataValue->setText("NA");
            }
        }
        else
        {
            ui->I2cConfRegAddrValue->setText("NA");
        }
    }
    else
    {
        ui->I2cConfChipAddrValue->setText("NA");
    }
}

void Ucm_I2cConfTab::i2c_conf_read_values_button_clicked(void)
{
    i2c_conf_read_values();
}

void Ucm_I2cConfTab::i2c_conf_write_values_button_clicked(void)
{
    i2c_conf_write_values();
    i2c_conf_read_values();
}

void Ucm_I2cConfTab::i2c_conf_auto_refresh_button_clicked(void)
{
    if (ui->I2cConfAutoRefreshButton->text() == "Start Refresh")
    {
        ui->I2cConfAutoRefreshButton->setEnabled(false);

        ui->I2cConfInstanceComboBox->setEnabled(false);
        ui->I2cConfReadValuesButton->setEnabled(false);
        ui->I2cConfWriteValuesButton->setEnabled(false);

        i2c_conf_timer->start(1000);

        ui->I2cConfAutoRefreshButton->setText("Stop Refresh");
        ui->I2cConfAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->I2cConfAutoRefreshButton->setEnabled(false);

        i2c_conf_timer->stop();

        ui->I2cConfInstanceComboBox->setEnabled(true);
        ui->I2cConfReadValuesButton->setEnabled(true);
        ui->I2cConfWriteValuesButton->setEnabled(true);

        ui->I2cConfAutoRefreshButton->setText("Start Refresh");
        ui->I2cConfAutoRefreshButton->setEnabled(true);
    }
}


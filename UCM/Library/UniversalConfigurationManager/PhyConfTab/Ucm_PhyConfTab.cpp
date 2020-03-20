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

#include <Ucm_PhyConfTab.h>
#include <ui_Ucm_PhyConfTab.h>

Ucm_PhyConfTab::Ucm_PhyConfTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_PhyConfTab();
    ui->setupUi(this);

    phy_conf_timer = new QTimer(this);
    phy_conf_timer->stop();
    connect(ui->PhyConfReadValuesButton, SIGNAL(clicked()), this, SLOT(phy_conf_read_values_button_clicked()));
    connect(ui->PhyConfWriteValuesButton, SIGNAL(clicked()), this, SLOT(phy_conf_write_values_button_clicked()));
    connect(ui->PhyConfAutoRefreshButton, SIGNAL(clicked()), this, SLOT(phy_conf_auto_refresh_button_clicked()));
    connect(phy_conf_timer, SIGNAL(timeout()), this, SLOT(phy_conf_read_values_button_clicked()));
}

Ucm_PhyConfTab::~Ucm_PhyConfTab()
{
    phy_conf_timer->stop();
    delete ui;
    delete phy_conf_timer;
}

int Ucm_PhyConfTab::phy_conf_resize(int height, int width)
{
    return 0;
}

void Ucm_PhyConfTab::phy_conf_add_instance(unsigned int instance)
{
    ui->PhyConfInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_PhyConfTab::phy_conf_enable(void)
{
    return 0;
}

int Ucm_PhyConfTab::phy_conf_disable(void)
{
    phy_conf_timer->stop();
    ui->PhyConfAutoRefreshButton->setText("Start Refresh");
    ui->PhyConfInstanceComboBox->setEnabled(true);
    ui->PhyConfReadValuesButton->setEnabled(true);
    ui->PhyConfWriteValuesButton->setEnabled(true);
    ui->PhyConfInstanceComboBox->clear();

    ui->PhyConfPhyAddrValue->setText("NA");
    ui->PhyConfRegAddrValue->setText("NA");
    ui->PhyConfRegDataValue->setText("NA");
    ui->PhyConfVersionValue->setText("NA");

    return 0;
}

void Ucm_PhyConfTab::phy_conf_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->PhyConfInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PhyConfigurationCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PhyConfPhyAddrValue->setText("NA");
            ui->PhyConfRegAddrValue->setText("NA");
            ui->PhyConfRegDataValue->setText("NA");
            ui->PhyConfVersionValue->setText("NA");
            return;
        }
    }

    // phy addr
    temp_string = ui->PhyConfPhyAddrValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        ui->PhyConfRegDataValue->setText("NA");
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_PhyAddrReg, temp_data))
    {
        ui->PhyConfPhyAddrValue->setText(QString::number(temp_data));
        
        // reg addr
        temp_string = ui->PhyConfRegAddrValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            ui->PhyConfRegDataValue->setText("NA");
        }
        else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_RegAddrReg, temp_data))
        {
            ui->PhyConfRegAddrValue->setText(QString::number(temp_data));
            
            // control read and wait
            temp_data = 0x00000002; // read
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_ControlReg, temp_data))
            {
                for (int i = 0; i < 10; i++)
                {
                    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PhyConf_ControlReg, temp_data))
                    {
                        if ((temp_data & 0x20000002) == 0x20000000)
                        {
                            // read data
                            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_PhyConf_DataReg, temp_data))
                            {
                                ui->PhyConfRegDataValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                            }
                            else
                            {
                                ui->PhyConfRegDataValue->setText("NA");
                            }
                            // read done
                            break;
                        }
                        else if (i == 9)
                        {
                            cout << "ERROR: " << "read did not complete" << endl;
                            temp_data = 0x00000000; // read clear
                            ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_ControlReg, temp_data);
                            ui->PhyConfRegDataValue->setText("NA");
                        }
                    }
                    else
                    {
                        ui->PhyConfRegDataValue->setText("NA");
                    }    
                }
            }
            else
            {
                ui->PhyConfRegDataValue->setText("NA");
            }    
        }
        else
        {
            ui->PhyConfRegAddrValue->setText("NA");
        }
    }
    else
    {
        ui->PhyConfPhyAddrValue->setText("NA");
    }
}

void Ucm_PhyConfTab::phy_conf_write_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->PhyConfInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_PhyConfigurationCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->PhyConfPhyAddrValue->setText("NA");
            ui->PhyConfRegAddrValue->setText("NA");
            ui->PhyConfRegDataValue->setText("NA");
            ui->PhyConfVersionValue->setText("NA");
            return;
            return;
        }
    }

    // phy addr
    temp_string = ui->PhyConfPhyAddrValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_PhyAddrReg, temp_data))
    {
        ui->PhyConfPhyAddrValue->setText(QString::number(temp_data));
        
        // reg addr
        temp_string = ui->PhyConfRegAddrValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        if (temp_string == "NA")
        {
            //nothing
        }
        else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_RegAddrReg, temp_data))
        {
            ui->PhyConfRegAddrValue->setText(QString::number(temp_data));
            
            // write data
            temp_string = ui->PhyConfRegDataValue->text();
            temp_data = temp_string.toUInt(nullptr, 16);
            if (temp_string == "NA")
            {
                //nothing
            }
            else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_DataReg, temp_data))
            {
                ui->PhyConfRegDataValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
                
                // control write and wait
                temp_data = 0x00000001; // write
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_ControlReg, temp_data))
                {
                    for (int i = 0; i < 10; i++)
                    {
                        if(0 == ucm->com_lib.read_reg(temp_addr + Ucm_PhyConf_ControlReg, temp_data))
                        {
                            if ((temp_data & 0x10000001) == 0x10000000)
                            {
                                // write done
                                break;
                            }
                            else if (i == 9)
                            {
                                cout << "ERROR: " << "write did not complete" << endl;
                                temp_data = 0x00000000; // write clear
                                ucm->com_lib.write_reg(temp_addr + Ucm_PhyConf_ControlReg, temp_data);
                            }
                        }
                        else
                        {
                            ui->PhyConfRegDataValue->setText("NA");
                        }    
                    }
                }
                else
                {
                    ui->PhyConfRegDataValue->setText("NA");
                }    
            }
            else
            {
                ui->PhyConfRegDataValue->setText("NA");
            }
        }
        else
        {
            ui->PhyConfRegAddrValue->setText("NA");
        }
    }
    else
    {
        ui->PhyConfPhyAddrValue->setText("NA");
    }
}

void Ucm_PhyConfTab::phy_conf_read_values_button_clicked(void)
{
    phy_conf_read_values();
}

void Ucm_PhyConfTab::phy_conf_write_values_button_clicked(void)
{
    phy_conf_write_values();
    phy_conf_read_values();
}

void Ucm_PhyConfTab::phy_conf_auto_refresh_button_clicked(void)
{
    if (ui->PhyConfAutoRefreshButton->text() == "Start Refresh")
    {
        ui->PhyConfAutoRefreshButton->setEnabled(false);

        ui->PhyConfInstanceComboBox->setEnabled(false);
        ui->PhyConfReadValuesButton->setEnabled(false);
        ui->PhyConfWriteValuesButton->setEnabled(false);

        phy_conf_timer->start(1000);

        ui->PhyConfAutoRefreshButton->setText("Stop Refresh");
        ui->PhyConfAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->PhyConfAutoRefreshButton->setEnabled(false);

        phy_conf_timer->stop();

        ui->PhyConfInstanceComboBox->setEnabled(true);
        ui->PhyConfReadValuesButton->setEnabled(true);
        ui->PhyConfWriteValuesButton->setEnabled(true);

        ui->PhyConfAutoRefreshButton->setText("Start Refresh");
        ui->PhyConfAutoRefreshButton->setEnabled(true);
    }
}


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

#include <Ucm_TsnIicTab.h>
#include <ui_Ucm_TsnIicTab.h>

Ucm_TsnIicTab::Ucm_TsnIicTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_TsnIicTab();
    ui->setupUi(this);

    tsn_iic_timer = new QTimer(this);
    tsn_iic_timer->stop();
    connect(ui->TsnIicReadValuesButton, SIGNAL(clicked()), this, SLOT(tsn_iic_read_values_button_clicked()));
    connect(ui->TsnIicWriteValuesButton, SIGNAL(clicked()), this, SLOT(tsn_iic_write_values_button_clicked()));
    connect(ui->TsnIicAutoRefreshButton, SIGNAL(clicked()), this, SLOT(tsn_iic_auto_refresh_button_clicked()));
    connect(tsn_iic_timer, SIGNAL(timeout()), this, SLOT(tsn_iic_read_values_button_clicked()));
}

Ucm_TsnIicTab::~Ucm_TsnIicTab()
{
    tsn_iic_timer->stop();
    delete ui;
    delete tsn_iic_timer;
}

int Ucm_TsnIicTab::tsn_iic_resize(int height, int width)
{
    return 0;
}

void Ucm_TsnIicTab::tsn_iic_add_instance(unsigned int instance)
{
    ui->TsnIicInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_TsnIicTab::tsn_iic_enable(void)
{
    return 0;
}

int Ucm_TsnIicTab::tsn_iic_disable(void)
{
    tsn_iic_timer->stop();
    ui->TsnIicAutoRefreshButton->setText("Start Refresh");
    ui->TsnIicInstanceComboBox->setEnabled(true);
    ui->TsnIicReadValuesButton->setEnabled(true);
    ui->TsnIicWriteValuesButton->setEnabled(true);
    ui->TsnIicInstanceComboBox->clear();

    ui->TsnIicTalkerEnableCheckBox->setChecked(false);
    ui->TsnIicTalkerVlanValue->setText("NA");
    ui->TsnIicTalkerEnableVlanCheckBox->setChecked(false);
    ui->TsnIicTalkerTalkerIdValue->setText("NA");
    ui->TsnIicTalkerTxDelayValue->setText("NA");
    ui->TsnIicTalkerIntervalValue->setText("NA");
    ui->TsnIicTalkerOffsetValue->setText("NA");
    ui->TsnIicTalkerResetStatsCheckBox->setChecked(false);
    ui->TsnIicTalkerAppIdValue->setText("NA");
    ui->TsnIicTalkerDataValue->setText("NA");
    ui->TsnIicTalkerRegDataCheckBox->setChecked(false);

    ui->TsnIicListenerEnableCheckBox->setChecked(false);
    ui->TsnIicListenerVlanValue->setText("NA");
    ui->TsnIicListenerEnableVlanCheckBox->setChecked(false);
    ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(false);
    ui->TsnIicListenerRxDelayValue->setText("NA");

    ui->TsnIicAnalyzerEnableCheckBox->setChecked(false);
    ui->TsnIicAnalyzerVlanValue->setText("NA");
    ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(false);
    ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(false);
    ui->TsnIicAnalyzerIpValue->setText("NA");
    ui->TsnIicAnalyzerUdpPortValue->setText("NA");

    ui->TsnIicLedEnableCheckBox->setChecked(false);
    ui->TsnIicLedVlanValue->setText("NA");
    ui->TsnIicLedEnableVlanCheckBox->setChecked(false);
    ui->TsnIicLedIgnoreVlanCheckBox->setChecked(false);
    ui->TsnIicLedTalkerIdValue->setText("NA");

    ui->TsnIicMacValue->setText("NA");
    ui->TsnIicExtPortCheckBox->setChecked(false);
    ui->TsnIicEnableCheckBox->setChecked(false);
    ui->TsnIicVersionValue->setText("NA");

    return 0;
}

void Ucm_TsnIicTab::tsn_iic_read_values(void)
{
    unsigned long long temp_mac;
    unsigned long temp_ip;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->TsnIicInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TsnIicCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TsnIicTalkerEnableCheckBox->setChecked(false);
            ui->TsnIicTalkerVlanValue->setText("NA");
            ui->TsnIicTalkerEnableVlanCheckBox->setChecked(false);
            ui->TsnIicTalkerTalkerIdValue->setText("NA");
            ui->TsnIicTalkerTxDelayValue->setText("NA");
            ui->TsnIicTalkerIntervalValue->setText("NA");
            ui->TsnIicTalkerOffsetValue->setText("NA");
            ui->TsnIicTalkerResetStatsCheckBox->setChecked(false);
            ui->TsnIicTalkerAppIdValue->setText("NA");
            ui->TsnIicTalkerDataValue->setText("NA");
            ui->TsnIicTalkerRegDataCheckBox->setChecked(false);

            ui->TsnIicListenerEnableCheckBox->setChecked(false);
            ui->TsnIicListenerVlanValue->setText("NA");
            ui->TsnIicListenerEnableVlanCheckBox->setChecked(false);
            ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(false);
            ui->TsnIicListenerRxDelayValue->setText("NA");

            ui->TsnIicAnalyzerEnableCheckBox->setChecked(false);
            ui->TsnIicAnalyzerVlanValue->setText("NA");
            ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(false);
            ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(false);
            ui->TsnIicAnalyzerIpValue->setText("NA");
            ui->TsnIicAnalyzerUdpPortValue->setText("NA");

            ui->TsnIicLedEnableCheckBox->setChecked(false);
            ui->TsnIicLedVlanValue->setText("NA");
            ui->TsnIicLedEnableVlanCheckBox->setChecked(false);
            ui->TsnIicLedIgnoreVlanCheckBox->setChecked(false);
            ui->TsnIicLedTalkerIdValue->setText("NA");

            ui->TsnIicMacValue->setText("NA");
            ui->TsnIicExtPortCheckBox->setChecked(false);
            ui->TsnIicEnableCheckBox->setChecked(false);
            ui->TsnIicVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TsnIicEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TsnIicEnableCheckBox->setChecked(false);
    }

    // Talker Part
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerControlReg, temp_data))
    {
        // enabled
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TsnIicTalkerEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicTalkerEnableCheckBox->setChecked(true);
        }

        // reset stats
        if ((temp_data & 0x00000002) == 0)
        {
            ui->TsnIicTalkerResetStatsCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicTalkerResetStatsCheckBox->setChecked(true);
        }

        // reg data
        if ((temp_data & 0x00010000) == 0)
        {
            ui->TsnIicTalkerRegDataCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicTalkerRegDataCheckBox->setChecked(true);
        }

        // vlan enabled
        if ((temp_data & 0x00020000) == 0)
        {
            ui->TsnIicTalkerEnableVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicTalkerEnableVlanCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TsnIicTalkerEnableCheckBox->setChecked(false);
        ui->TsnIicTalkerResetStatsCheckBox->setChecked(false);
        ui->TsnIicTalkerRegDataCheckBox->setChecked(false);
        ui->TsnIicTalkerEnableVlanCheckBox->setChecked(false);
    }

    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicTalkerVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicTalkerVlanValue->setText("NA");
    }

    // talker id
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerIdReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicTalkerTalkerIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicTalkerTalkerIdValue->setText("NA");
    }

    // tx delay
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerDelayReg, temp_data))
    {
        ui->TsnIicTalkerTxDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->TsnIicTalkerTxDelayValue->setText("NA");
    }

    // interval
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerIntervalReg, temp_data))
    {
        ui->TsnIicTalkerIntervalValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->TsnIicTalkerIntervalValue->setText("NA");
    }

    // offset
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerOffsetReg, temp_data))
    {
        ui->TsnIicTalkerOffsetValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->TsnIicTalkerOffsetValue->setText("NA");
    }

    // app id
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerAppIdReg, temp_data))
    {
        temp_data &= 0x000000FF;
        ui->TsnIicTalkerAppIdValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->TsnIicTalkerAppIdValue->setText("NA");
    }

    // data
    temp_string.clear();
    for (unsigned int i = 0; i < (32/4); i++)
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerAppDataReg + (i*4), temp_data))
        {
            temp_string.append("0x");
            temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(" 0x");
            temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(" 0x");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(" 0x");
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append("\n");
        }
        else
        {
            temp_string.append("NA NA NA NA\n");
        }
    }
    ui->TsnIicTalkerDataValue->setText(temp_string);


    // Listener Part
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_ListenerControlReg, temp_data))
    {
        // enabled
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TsnIicListenerEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicListenerEnableCheckBox->setChecked(true);
        }

        // vlan enabled
        if ((temp_data & 0x00020000) == 0)
        {
            ui->TsnIicListenerEnableVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicListenerEnableVlanCheckBox->setChecked(true);
        }
        
        // vlan ignore
        if ((temp_data & 0x00040000) == 0)
        {
            ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TsnIicListenerEnableCheckBox->setChecked(false);
        ui->TsnIicListenerEnableVlanCheckBox->setChecked(false);
        ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(false);
    }

    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_ListenerVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicListenerVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicListenerVlanValue->setText("NA");
    }

    // Rx delay
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_ListenerDelayReg, temp_data))
    {
        ui->TsnIicListenerRxDelayValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->TsnIicListenerRxDelayValue->setText("NA");
    }

    // Analyzer Part
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AnalyzerControlReg, temp_data))
    {
        // enabled
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TsnIicAnalyzerEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicAnalyzerEnableCheckBox->setChecked(true);
        }

        // vlan enabled
        if ((temp_data & 0x00020000) == 0)
        {
            ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(true);
        }
        
        // vlan ignore
        if ((temp_data & 0x00040000) == 0)
        {
            ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TsnIicAnalyzerEnableCheckBox->setChecked(false);
        ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(false);
        ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(false);
    }

    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AnalyzerVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicAnalyzerVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicAnalyzerVlanValue->setText("NA");
    }

    // ip
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AnalyzerIpReg, temp_data))
    {
        temp_ip = 0x00000000;
        temp_ip |= (temp_data >> 0) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 8) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 16) & 0x000000FF;
        temp_ip = temp_ip << 8;
        temp_ip |= (temp_data >> 24) & 0x000000FF;

        temp_string = QHostAddress(temp_ip).toString();

        ui->TsnIicAnalyzerIpValue->setText(temp_string);

    }
    else
    {
        ui->TsnIicAnalyzerIpValue->setText("NA");
    }

    // udp port
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AnalyzerUdpPortReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicAnalyzerUdpPortValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicAnalyzerUdpPortValue->setText("NA");
    }

    // Led Part
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AppLedControlReg, temp_data))
    {
        // enabled
        if ((temp_data & 0x00000001) == 0)
        {
            ui->TsnIicLedEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicLedEnableCheckBox->setChecked(true);
        }

        // vlan enabled
        if ((temp_data & 0x00020000) == 0)
        {
            ui->TsnIicLedEnableVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicLedEnableVlanCheckBox->setChecked(true);
        }
        
        // vlan ignore
        if ((temp_data & 0x00040000) == 0)
        {
            ui->TsnIicLedIgnoreVlanCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicLedIgnoreVlanCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TsnIicLedEnableCheckBox->setChecked(false);
        ui->TsnIicLedEnableVlanCheckBox->setChecked(false);
        ui->TsnIicLedIgnoreVlanCheckBox->setChecked(false);
    }

    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AppLedVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicLedVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicLedVlanValue->setText("NA");
    }

    // talker id
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_AppLedIdReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->TsnIicLedTalkerIdValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->TsnIicLedTalkerIdValue->setText("NA");
    }

    // General Part
    // ext port
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_ControlReg, temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ui->TsnIicExtPortCheckBox->setChecked(false);
        }
        else
        {
            ui->TsnIicExtPortCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->TsnIicExtPortCheckBox->setChecked(false);
    }

    // mac
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerMac1Reg, temp_data))
    {
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_TalkerMac2Reg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            ui->TsnIicMacValue->setText(temp_string);
        }
        else
        {
            ui->TsnIicMacValue->setText("NA");
        }

    }
    else
    {
        ui->TsnIicMacValue->setText("NA");
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_TsnIic_VersionReg, temp_data))
    {
        ui->TsnIicVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->TsnIicVersionValue->setText("NA");
    }

}

void Ucm_TsnIicTab::tsn_iic_write_values(void)
{
    unsigned long long temp_mac;
    unsigned long temp_ip;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->TsnIicInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_TsnIicCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->TsnIicTalkerEnableCheckBox->setChecked(false);
            ui->TsnIicTalkerVlanValue->setText("NA");
            ui->TsnIicTalkerEnableVlanCheckBox->setChecked(false);
            ui->TsnIicTalkerTalkerIdValue->setText("NA");
            ui->TsnIicTalkerTxDelayValue->setText("NA");
            ui->TsnIicTalkerIntervalValue->setText("NA");
            ui->TsnIicTalkerOffsetValue->setText("NA");
            ui->TsnIicTalkerResetStatsCheckBox->setChecked(false);
            ui->TsnIicTalkerAppIdValue->setText("NA");
            ui->TsnIicTalkerDataValue->setText("NA");
            ui->TsnIicTalkerRegDataCheckBox->setChecked(false);

            ui->TsnIicListenerEnableCheckBox->setChecked(false);
            ui->TsnIicListenerVlanValue->setText("NA");
            ui->TsnIicListenerEnableVlanCheckBox->setChecked(false);
            ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(false);
            ui->TsnIicListenerRxDelayValue->setText("NA");

            ui->TsnIicAnalyzerEnableCheckBox->setChecked(false);
            ui->TsnIicAnalyzerVlanValue->setText("NA");
            ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(false);
            ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(false);
            ui->TsnIicAnalyzerIpValue->setText("NA");
            ui->TsnIicAnalyzerUdpPortValue->setText("NA");

            ui->TsnIicLedEnableCheckBox->setChecked(false);
            ui->TsnIicLedVlanValue->setText("NA");
            ui->TsnIicLedEnableVlanCheckBox->setChecked(false);
            ui->TsnIicLedIgnoreVlanCheckBox->setChecked(false);
            ui->TsnIicLedTalkerIdValue->setText("NA");

            ui->TsnIicMacValue->setText("NA");
            ui->TsnIicExtPortCheckBox->setChecked(false);
            ui->TsnIicEnableCheckBox->setChecked(false);
            ui->TsnIicVersionValue->setText("NA");
            return;
        }
    }

    // Talker Part
    temp_data = 0x00000000; // nothing
    if(true == ui->TsnIicTalkerEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; 
    }
    if(true == ui->TsnIicTalkerResetStatsCheckBox->isChecked())
    {
        temp_data |= 0x00000002; 
    }
    if(true == ui->TsnIicTalkerRegDataCheckBox->isChecked())
    {
        temp_data |= 0x00010000; 
    }
    if(true == ui->TsnIicTalkerEnableVlanCheckBox->isChecked())
    {
        temp_data |= 0x00020000; 
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerEnableCheckBox->setChecked(false);
        ui->TsnIicTalkerResetStatsCheckBox->setChecked(false);
        ui->TsnIicTalkerRegDataCheckBox->setChecked(false);
        ui->TsnIicTalkerEnableVlanCheckBox->setChecked(false);
    }

    // vlan
    temp_string = ui->TsnIicTalkerVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerVlanReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerVlanValue->setText("NA");
    }

    // talker id
    temp_string = ui->TsnIicTalkerTalkerIdValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerIdReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerTalkerIdValue->setText("NA");
    }

    // tx delay
    temp_string = ui->TsnIicTalkerTxDelayValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerDelayReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerTxDelayValue->setText("NA");
    }

    // interval
    temp_string = ui->TsnIicTalkerIntervalValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerIntervalReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerIntervalValue->setText("NA");
    }

    // offset
    temp_string = ui->TsnIicTalkerOffsetValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerOffsetReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerOffsetValue->setText("NA");
    }

    // app id
    temp_string = ui->TsnIicTalkerAppIdValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerAppIdReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicTalkerAppIdValue->setText("NA");
    }

    // data 
    temp_string = ui->TsnIicTalkerDataValue->toPlainText();
    QRegExp sep("\\s+");
    for (unsigned int i = 0; i < (32/4); i++)
    {
        temp_data = 0;
        temp_data |= ((temp_string.section(sep, (int)((i*4)+0), (int)((i*4)+0)).toUInt(nullptr, 16)) & 0x000000FF);
        temp_data = temp_data << 8;
        temp_data |= ((temp_string.section(sep, (int)((i*4)+1), (int)((i*4)+1)).toUInt(nullptr, 16)) & 0x000000FF);
        temp_data = temp_data << 8;
        temp_data |= ((temp_string.section(sep, (int)((i*4)+2), (int)((i*4)+2)).toUInt(nullptr, 16)) & 0x000000FF);
        temp_data = temp_data << 8;
        temp_data |= ((temp_string.section(sep, (int)((i*4)+3), (int)((i*4)+3)).toUInt(nullptr, 16)) & 0x000000FF);
        
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerAppDataReg + (i*4), temp_data))
        {
            // nothing            
        }
        else
        {
           // let the read part handle it
        }
    }

    // Listener Part
    temp_data = 0x00000000; // nothing
    if(true == ui->TsnIicListenerEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001;
    }
    if(true == ui->TsnIicListenerEnableVlanCheckBox->isChecked())
    {
        temp_data |= 0x00020000;
    }
    if(true == ui->TsnIicListenerIgnoreVlanCheckBox->isChecked())
    {
        temp_data |= 0x00040000;
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_ListenerControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicListenerEnableCheckBox->setChecked(false);
        ui->TsnIicListenerEnableVlanCheckBox->setChecked(false);
        ui->TsnIicListenerIgnoreVlanCheckBox->setChecked(false);
    }

    // vlan
    temp_string = ui->TsnIicListenerVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_ListenerVlanReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicListenerVlanValue->setText("NA");
    }

    // Rx delay
    temp_string = ui->TsnIicListenerRxDelayValue->text();
    temp_data = temp_string.toUInt(nullptr, 10);
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_ListenerDelayReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicListenerRxDelayValue->setText("NA");
    }

    // Analyzer Part
    temp_data = 0x00000000; // nothing
    if(true == ui->TsnIicAnalyzerEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001;
    }
    if(true == ui->TsnIicAnalyzerEnableVlanCheckBox->isChecked())
    {
        temp_data |= 0x00020000;
    }
    if(true == ui->TsnIicAnalyzerIgnoreVlanCheckBox->isChecked())
    {
        temp_data |= 0x00040000;
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AnalyzerControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicAnalyzerEnableCheckBox->setChecked(false);
        ui->TsnIicAnalyzerEnableVlanCheckBox->setChecked(false);
        ui->TsnIicAnalyzerIgnoreVlanCheckBox->setChecked(false);
    }

    // vlan
    temp_string = ui->TsnIicAnalyzerVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AnalyzerVlanReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicAnalyzerVlanValue->setText("NA");
    }

    // ip
    temp_string = ui->TsnIicAnalyzerIpValue->text();
    temp_ip = QHostAddress(temp_string).toIPv4Address();

    temp_data = 0x00000000;
    temp_data |= (temp_ip >> 0) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 8) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 16) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_ip >> 24) & 0x000000FF;

    if (temp_string == "NA")
    {
        // nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AnalyzerIpReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicAnalyzerIpValue->setText("NA");
    }

    // udp port
    temp_string = ui->TsnIicAnalyzerUdpPortValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AnalyzerUdpPortReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicAnalyzerUdpPortValue->setText("NA");
    }

    // Led Part
    temp_data = 0x00000000; // nothing
    if(true == ui->TsnIicLedEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001;
    }
    if(true == ui->TsnIicLedEnableVlanCheckBox->isChecked())
    {
        temp_data |= 0x00020000;
    }
    if(true == ui->TsnIicLedIgnoreVlanCheckBox->isChecked())
    {
        temp_data |= 0x00040000;
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AppLedControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicLedEnableCheckBox->setChecked(false);
        ui->TsnIicLedEnableVlanCheckBox->setChecked(false);
        ui->TsnIicLedIgnoreVlanCheckBox->setChecked(false);
    }

    // vlan
    temp_string = ui->TsnIicLedVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AppLedVlanReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicLedVlanValue->setText("NA");
    }

    // talker id
    temp_string = ui->TsnIicLedTalkerIdValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_AppLedIdReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicLedTalkerIdValue->setText("NA");
    }

    // General Part
    // mac
    temp_string = ui->TsnIicMacValue->text();
    temp_string.remove(QChar(':'), Qt::CaseInsensitive);
    temp_mac = temp_string.toULongLong(nullptr, 16);
    temp_string.clear();
    temp_data = 0x00000000;
    temp_data |= (temp_mac >> 16) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 24) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 32) & 0x000000FF;
    temp_data = temp_data << 8;
    temp_data |= (temp_mac >> 40) & 0x000000FF;

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerMac1Reg, temp_data))
    {
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");

        temp_data = 0x00000000;
        temp_data |= (temp_mac >> 0) & 0x000000FF;
        temp_data = temp_data << 8;
        temp_data |= (temp_mac >> 8) & 0x000000FF;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_TalkerMac2Reg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        }
        else
        {
            ui->TsnIicMacValue->setText("NA");
        }

    }
    else
    {
        ui->TsnIicMacValue->setText("NA");
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->TsnIicEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if(true == ui->TsnIicExtPortCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // ext port
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_TsnIic_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->TsnIicEnableCheckBox->setChecked(false);
    }
}

void Ucm_TsnIicTab::tsn_iic_read_values_button_clicked(void)
{
    tsn_iic_read_values();
}

void Ucm_TsnIicTab::tsn_iic_write_values_button_clicked(void)
{
    tsn_iic_write_values();
    tsn_iic_read_values();
}

void Ucm_TsnIicTab::tsn_iic_auto_refresh_button_clicked(void)
{
    if (ui->TsnIicAutoRefreshButton->text() == "Start Refresh")
    {
        ui->TsnIicAutoRefreshButton->setEnabled(false);

        ui->TsnIicInstanceComboBox->setEnabled(false);
        ui->TsnIicReadValuesButton->setEnabled(false);
        ui->TsnIicWriteValuesButton->setEnabled(false);

        tsn_iic_timer->start(1000);

        ui->TsnIicAutoRefreshButton->setText("Stop Refresh");
        ui->TsnIicAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->TsnIicAutoRefreshButton->setEnabled(false);

        tsn_iic_timer->stop();

        ui->TsnIicInstanceComboBox->setEnabled(true);
        ui->TsnIicReadValuesButton->setEnabled(true);
        ui->TsnIicWriteValuesButton->setEnabled(true);

        ui->TsnIicAutoRefreshButton->setText("Start Refresh");
        ui->TsnIicAutoRefreshButton->setEnabled(true);
    }
}


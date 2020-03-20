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

#include <Ucm_RedHsrPrpTab.h>
#include <ui_Ucm_RedHsrPrpTab.h>

Ucm_RedHsrPrpTab::Ucm_RedHsrPrpTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_RedHsrPrpTab();
    ui->setupUi(this);

    red_hsrprp_timer = new QTimer(this);
    red_hsrprp_timer->stop();
    connect(ui->RedHsrPrpReadValuesButton, SIGNAL(clicked()), this, SLOT(red_hsrprp_read_values_button_clicked()));
    connect(ui->RedHsrPrpWriteValuesButton, SIGNAL(clicked()), this, SLOT(red_hsrprp_write_values_button_clicked()));
    connect(ui->RedHsrPrpAutoRefreshButton, SIGNAL(clicked()), this, SLOT(red_hsrprp_auto_refresh_button_clicked()));
    connect(red_hsrprp_timer, SIGNAL(timeout()), this, SLOT(red_hsrprp_read_values_button_clicked()));
}

Ucm_RedHsrPrpTab::~Ucm_RedHsrPrpTab()
{
    red_hsrprp_timer->stop();
    delete ui;
    delete red_hsrprp_timer;
}

int Ucm_RedHsrPrpTab::red_hsrprp_resize(int height, int width)
{
    return 0;
}

void Ucm_RedHsrPrpTab::red_hsrprp_add_instance(unsigned int instance)
{
    ui->RedHsrPrpInstanceComboBox->addItem(QString::number(instance));
}

int Ucm_RedHsrPrpTab::red_hsrprp_enable(void)
{
    return 0;
}

int Ucm_RedHsrPrpTab::red_hsrprp_disable(void)
{
    red_hsrprp_timer->stop();
    ui->RedHsrPrpAutoRefreshButton->setText("Start Refresh");
    ui->RedHsrPrpInstanceComboBox->setEnabled(true);
    ui->RedHsrPrpReadValuesButton->setEnabled(true);
    ui->RedHsrPrpWriteValuesButton->setEnabled(true);
    ui->RedHsrPrpInstanceComboBox->clear();

    ui->RedHsrPrpMacValue->setText("NA");
    ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
    ui->RedHsrPrpVlanValue->setText("NA");
    ui->RedHsrPrpModeValue->setCurrentText("NA");
    ui->RedHsrPrpVersionValue->setText("NA");
    ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
    ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
    ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
    ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
    ui->RedHsrPrpEnableCheckBox->setChecked(false);
    ui->RedHsrPrpLanACheckBox->setChecked(false);
    ui->RedHsrPrpLanBCheckBox->setChecked(false);

    ui->RedHsrPrpRxFrameAValue->setText("NA");
    ui->RedHsrPrpRxErrorAValue->setText("NA");
    ui->RedHsrPrpTxFrameAValue->setText("NA");
    ui->RedHsrPrpTxErrorAValue->setText("NA");
    ui->RedHsrPrpRxFrameBValue->setText("NA");
    ui->RedHsrPrpRxErrorBValue->setText("NA");
    ui->RedHsrPrpTxFrameBValue->setText("NA");
    ui->RedHsrPrpTxErrorBValue->setText("NA");
    ui->RedHsrPrpRxFrameCValue->setText("NA");
    ui->RedHsrPrpRxErrorCValue->setText("NA");
    ui->RedHsrPrpTxFrameCValue->setText("NA");
    ui->RedHsrPrpTxErrorCValue->setText("NA");
    ui->RedHsrPrpClearCountersCheckBox->setChecked(false);

    ui->RedHsrPrpRedBoxIdValue->setText("NA");
    ui->RedHsrPrpLanIdValue->setText("NA");

    return 0;
}

void Ucm_RedHsrPrpTab::red_hsrprp_read_values(void)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->RedHsrPrpInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RedHsrPrpCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->RedHsrPrpMacValue->setText("NA");
            ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
            ui->RedHsrPrpVlanValue->setText("NA");
            ui->RedHsrPrpModeValue->setCurrentText("NA");
            ui->RedHsrPrpVersionValue->setText("NA");
            ui->RedHsrPrpEnableCheckBox->setChecked(false);
            ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
            ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
            ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
            ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
            ui->RedHsrPrpLanACheckBox->setChecked(false);
            ui->RedHsrPrpLanBCheckBox->setChecked(false);

            ui->RedHsrPrpRxFrameAValue->setText("NA");
            ui->RedHsrPrpRxErrorAValue->setText("NA");
            ui->RedHsrPrpTxFrameAValue->setText("NA");
            ui->RedHsrPrpTxErrorAValue->setText("NA");
            ui->RedHsrPrpRxFrameBValue->setText("NA");
            ui->RedHsrPrpRxErrorBValue->setText("NA");
            ui->RedHsrPrpTxFrameBValue->setText("NA");
            ui->RedHsrPrpTxErrorBValue->setText("NA");
            ui->RedHsrPrpRxFrameCValue->setText("NA");
            ui->RedHsrPrpRxErrorCValue->setText("NA");
            ui->RedHsrPrpTxFrameCValue->setText("NA");
            ui->RedHsrPrpTxErrorCValue->setText("NA");
            ui->RedHsrPrpClearCountersCheckBox->setChecked(false);

            ui->RedHsrPrpRedBoxIdValue->setText("NA");
            ui->RedHsrPrpLanIdValue->setText("NA");

            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->RedHsrPrpEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->RedHsrPrpEnableCheckBox->setChecked(false);
    }


    // mac
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_Mac1_RegReg, temp_data))
    {
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_Mac2_RegReg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            ui->RedHsrPrpMacValue->setText(temp_string);
        }
        else
        {
            ui->RedHsrPrpMacValue->setText("NA");
        }

    }
    else
    {
        ui->RedHsrPrpMacValue->setText("NA");
    }

    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_ConfigVlanReg, temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpVlanEnableCheckBox->setChecked(true);
        }

        temp_data &= 0x0000FFFF;

        ui->RedHsrPrpVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
        ui->RedHsrPrpVlanValue->setText("NA");
    }


    // mode
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_ConfigModeReg, temp_data))
    {
        switch (temp_data & 0x0000000F)
        {
        case 0:
            ui->RedHsrPrpModeValue->setCurrentText("NO");
            ui->RedHsrPrpRedBoxIdValue->setText("NA");
            ui->RedHsrPrpLanIdValue->setText("NA");
            break;
        case 1:
            ui->RedHsrPrpModeValue->setCurrentText("PRP");
            ui->RedHsrPrpRedBoxIdValue->setText("NA");
            ui->RedHsrPrpLanIdValue->setText("NA");
            break;
        case 2:
            ui->RedHsrPrpModeValue->setCurrentText("HSR");
            ui->RedHsrPrpRedBoxIdValue->setText("NA");
            ui->RedHsrPrpLanIdValue->setText("NA");
            break;
        case 5:
            ui->RedHsrPrpModeValue->setCurrentText("HSR-PRP");
            ui->RedHsrPrpRedBoxIdValue->setText(QString("0x%1").arg(((temp_data >> 12) & 0x0000000F), 1, 16, QLatin1Char('0')));
            ui->RedHsrPrpLanIdValue->setText(QString("0x%1").arg(((temp_data >> 8) & 0x00000007), 1, 16, QLatin1Char('0')));
            break;
        case 6:
            ui->RedHsrPrpModeValue->setCurrentText("HSR-HSR");
            ui->RedHsrPrpRedBoxIdValue->setText(QString("0x%1").arg(((temp_data >> 12) & 0x0000000F), 1, 16, QLatin1Char('0')));
            ui->RedHsrPrpLanIdValue->setText("NA");
            break;
        default:
            ui->RedHsrPrpModeValue->setCurrentText("NA");
            ui->RedHsrPrpRedBoxIdValue->setText("NA");
            ui->RedHsrPrpLanIdValue->setText("NA");
            break;
        }

        if ((temp_data & 0x00010000) == 0)
        {
            ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00020000) == 0)
        {
            ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpNoForwardingCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00040000) == 0)
        {
            ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpTailTaggingCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00080000) == 0)
        {
            ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->RedHsrPrpModeValue->setCurrentText("NA");
        ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
        ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
        ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
        ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
    }


    // lan status
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_StatusReg, temp_data))
    {
        if ((temp_data & 0x00000101) == 0x00000100)
        {
            ui->RedHsrPrpLanACheckBox->setChecked(true);
        }
        else
        {
            ui->RedHsrPrpLanACheckBox->setChecked(false);
        }

        if ((temp_data & 0x00000202) == 0x00000200)
        {
            ui->RedHsrPrpLanBCheckBox->setChecked(true);
        }
        else
        {
            ui->RedHsrPrpLanBCheckBox->setChecked(false);
        }

        temp_data &= 0x00000003;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_StatusReg, temp_data))
        {
            // nothing just cleared the values
        }
    }
    else
    {
        ui->RedHsrPrpLanACheckBox->setChecked(false);
        ui->RedHsrPrpLanBCheckBox->setChecked(false);
    }

    // status counter
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_RxFrameCountAReg, temp_data))
    {
        ui->RedHsrPrpRxFrameAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpRxFrameAValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_RxErrCountAReg, temp_data))
    {
        ui->RedHsrPrpRxErrorAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpRxErrorAValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_TxFrameCountAReg, temp_data))
    {
        ui->RedHsrPrpTxFrameAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpTxFrameAValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_TxErrCountAReg, temp_data))
    {
        ui->RedHsrPrpTxErrorAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpTxErrorAValue->setText("NA");
    }

    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_RxFrameCountBReg, temp_data))
    {
        ui->RedHsrPrpRxFrameBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpRxFrameBValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_RxErrCountBReg, temp_data))
    {
        ui->RedHsrPrpRxErrorBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpRxErrorBValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_TxFrameCountBReg, temp_data))
    {
        ui->RedHsrPrpTxFrameBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpTxFrameBValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_TxErrCountBReg, temp_data))
    {
        ui->RedHsrPrpTxErrorBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpTxErrorBValue->setText("NA");
    }

    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_RxFrameCountCReg, temp_data))
    {
        ui->RedHsrPrpRxFrameCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpRxFrameCValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_RxErrCountCReg, temp_data))
    {
        ui->RedHsrPrpRxErrorCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpRxErrorCValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_TxFrameCountCReg, temp_data))
    {
        ui->RedHsrPrpTxFrameCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpTxFrameCValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_TxErrCountCReg, temp_data))
    {
        ui->RedHsrPrpTxErrorCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedHsrPrpTxErrorCValue->setText("NA");
    }

    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_FrameCountControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->RedHsrPrpClearCountersCheckBox->setChecked(false);
        }
        else
        {
            ui->RedHsrPrpClearCountersCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->RedHsrPrpClearCountersCheckBox->setChecked(false);
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedHsrPrp_VersionReg, temp_data))
    {
        ui->RedHsrPrpVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->RedHsrPrpVersionValue->setText("NA");
    }
}

void Ucm_RedHsrPrpTab::red_hsrprp_write_values(void)
{
    unsigned long long temp_mac;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->RedHsrPrpInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RedHsrPrpCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->RedHsrPrpMacValue->setText("NA");
            ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
            ui->RedHsrPrpVlanValue->setText("NA");
            ui->RedHsrPrpModeValue->setCurrentText("NA");
            ui->RedHsrPrpVersionValue->setText("NA");
            ui->RedHsrPrpEnableCheckBox->setChecked(true);
            ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
            ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
            ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
            ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
            ui->RedHsrPrpLanACheckBox->setChecked(false);
            ui->RedHsrPrpLanBCheckBox->setChecked(false);

            ui->RedHsrPrpRxFrameAValue->setText("NA");
            ui->RedHsrPrpRxErrorAValue->setText("NA");
            ui->RedHsrPrpTxFrameAValue->setText("NA");
            ui->RedHsrPrpTxErrorAValue->setText("NA");
            ui->RedHsrPrpRxFrameBValue->setText("NA");
            ui->RedHsrPrpRxErrorBValue->setText("NA");
            ui->RedHsrPrpTxFrameBValue->setText("NA");
            ui->RedHsrPrpTxErrorBValue->setText("NA");
            ui->RedHsrPrpRxFrameCValue->setText("NA");
            ui->RedHsrPrpRxErrorCValue->setText("NA");
            ui->RedHsrPrpTxFrameCValue->setText("NA");
            ui->RedHsrPrpTxErrorCValue->setText("NA");
            ui->RedHsrPrpClearCountersCheckBox->setChecked(false);

            ui->RedHsrPrpRedBoxIdValue->setText("NA");
            ui->RedHsrPrpLanIdValue->setText("NA");

            return;
        }
    }


    // mac
    temp_string = ui->RedHsrPrpMacValue->text();
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

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_Mac1_RegReg, temp_data))
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
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_Mac2_RegReg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_MacControlReg, temp_data))
            {
                ui->RedHsrPrpMacValue->setText(temp_string);
            }
            else
            {
                ui->RedHsrPrpMacValue->setText("NA");
            }
        }
        else
        {
            ui->RedHsrPrpMacValue->setText("NA");
        }

    }
    else
    {
        ui->RedHsrPrpMacValue->setText("NA");
    }

    // vlan
    temp_string = ui->RedHsrPrpVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if(true == ui->RedHsrPrpVlanEnableCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // enable
    }
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_ConfigVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->RedHsrPrpVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
            ui->RedHsrPrpVlanValue->setText("NA");
        }
    }
    else
    {
        ui->RedHsrPrpVlanEnableCheckBox->setChecked(false);
        ui->RedHsrPrpVlanValue->setText("NA");
    }

    // mode
    temp_string = ui->RedHsrPrpModeValue->currentText();
    if (temp_string == "NO")
    {
        temp_data = 0x00000000;
    }
    else if (temp_string == "PRP")
    {
        temp_data = 0x00000001;
    }
    else if (temp_string == "HSR")
    {
        temp_data = 0x00000002;
    }
    else if (temp_string == "HSR-PRP")
    {
        temp_data = 0x00000005;
        temp_string = ui->RedHsrPrpLanIdValue->text();
        temp_data |= ((temp_string.toUInt(nullptr, 16) << 8) & 0x00000700);
        temp_string = ui->RedHsrPrpRedBoxIdValue->text();
        temp_data |= ((temp_string.toUInt(nullptr, 16) << 12) & 0x0000F000);
        temp_string = "HSR-PRP";
    }
    else if (temp_string == "HSR-HSR")
    {
        temp_data = 0x00000006;
        temp_string = ui->RedHsrPrpRedBoxIdValue->text();
        temp_data |= ((temp_string.toUInt(nullptr, 16) << 12) & 0x0000F000);
        temp_string = "HSR-HSR";
    }
    else
    {
        temp_data = 0x00000000;
    }

    if(true == ui->RedHsrPrpPromiscuousModeCheckBox->isChecked())
    {
        temp_data |= 0x00010000;
    }

    if(true == ui->RedHsrPrpNoForwardingCheckBox->isChecked())
    {
        temp_data |= 0x00020000;
    }

    if(true == ui->RedHsrPrpTailTaggingCheckBox->isChecked())
    {
        temp_data |= 0x00040000;
    }

    if(true == ui->RedHsrPrpPrpUntaggingCheckBox->isChecked())
    {
        temp_data |= 0x00080000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_ConfigModeReg, temp_data))
    {
        temp_data = 0x00000001; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedHsrPrpModeValue->setCurrentText("NA");
            ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
            ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
            ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
            ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedHsrPrpModeValue->setCurrentText("NA");
        ui->RedHsrPrpPromiscuousModeCheckBox->setChecked(false);
        ui->RedHsrPrpNoForwardingCheckBox->setChecked(false);
        ui->RedHsrPrpTailTaggingCheckBox->setChecked(false);
        ui->RedHsrPrpPrpUntaggingCheckBox->setChecked(false);
    }

    // status counter
    temp_data = 0x00000000; // nothing
    if(true == ui->RedHsrPrpClearCountersCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // clear
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_FrameCountControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->RedHsrPrpClearCountersCheckBox->setChecked(false);
    }

    temp_data = 0x00000000; // nothing
    if(true == ui->RedHsrPrpEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedHsrPrp_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->RedHsrPrpEnableCheckBox->setChecked(false);
    }
}

void Ucm_RedHsrPrpTab::red_hsrprp_read_values_button_clicked(void)
{
    red_hsrprp_read_values();
}

void Ucm_RedHsrPrpTab::red_hsrprp_write_values_button_clicked(void)
{
    red_hsrprp_write_values();
    red_hsrprp_read_values();
}

void Ucm_RedHsrPrpTab::red_hsrprp_auto_refresh_button_clicked(void)
{
    if (ui->RedHsrPrpAutoRefreshButton->text() == "Start Refresh")
    {
        ui->RedHsrPrpAutoRefreshButton->setEnabled(false);

        ui->RedHsrPrpInstanceComboBox->setEnabled(false);
        ui->RedHsrPrpReadValuesButton->setEnabled(false);
        ui->RedHsrPrpWriteValuesButton->setEnabled(false);

        red_hsrprp_timer->start(1000);

        ui->RedHsrPrpAutoRefreshButton->setText("Stop Refresh");
        ui->RedHsrPrpAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->RedHsrPrpAutoRefreshButton->setEnabled(false);

        red_hsrprp_timer->stop();

        ui->RedHsrPrpInstanceComboBox->setEnabled(true);
        ui->RedHsrPrpReadValuesButton->setEnabled(true);
        ui->RedHsrPrpWriteValuesButton->setEnabled(true);

        ui->RedHsrPrpAutoRefreshButton->setText("Start Refresh");
        ui->RedHsrPrpAutoRefreshButton->setEnabled(true);
    }
}


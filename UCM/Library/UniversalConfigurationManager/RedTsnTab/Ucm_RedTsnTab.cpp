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

#include <Ucm_RedTsnTab.h>
#include <ui_Ucm_RedTsnTab.h>
#include <ui_Ucm_RedTsnSchedulingScreen.h>

Ucm_RedTsnTab::Ucm_RedTsnTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;
    ui_scheduling = new Ucm_RedTsnSchedulingScreen;

    ui = new Ui::Ucm_RedTsnTab();
    ui->setupUi(this);

    red_tsn_timer = new QTimer(this);
    red_tsn_timer->stop();
    connect(ui->RedTsnInstanceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(red_tsn_instance_changed(int)));
    connect(ui->RedTsnReadValuesButton, SIGNAL(clicked()), this, SLOT(red_tsn_read_values_button_clicked()));
    connect(ui->RedTsnWriteValuesButton, SIGNAL(clicked()), this, SLOT(red_tsn_write_values_button_clicked()));
    connect(ui->RedTsnAutoRefreshButton, SIGNAL(clicked()), this, SLOT(red_tsn_auto_refresh_button_clicked()));
    connect(ui->RedTsnSchedulingButton, SIGNAL(clicked()), this, SLOT(red_tsn_scheduling_button_clicked()));
    connect(red_tsn_timer, SIGNAL(timeout()), this, SLOT(red_tsn_read_values_button_clicked()));
}

Ucm_RedTsnTab::~Ucm_RedTsnTab()
{
    red_tsn_timer->stop();
    delete ui;
    ui_scheduling->close();
    delete ui_scheduling;
    delete red_tsn_timer;
}

int Ucm_RedTsnTab::red_tsn_resize(int height, int width)
{
    return 0;
}

void Ucm_RedTsnTab::red_tsn_add_instance(unsigned int instance)
{
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;

    ui->RedTsnInstanceComboBox->addItem(QString::number(instance));

    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RedTsnCoreType) && (ucm->core_config.at(i).core_instance_nr == instance))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_StatusReg, temp_data))
            {
                red_tsn_priority_support.append((temp_data & 0x01000000) != 0);
                if ((temp_data & 0x01000000) != 0)
                {
                    red_tsn_nr_of_priorities.append((temp_data & 0x000F0000) >> 16);
                }
                else
                {
                    red_tsn_nr_of_priorities.append(0);
                }
                red_tsn_phase_support.append((temp_data & 0x02000000) != 0);
                red_tsn_credit_support.append((temp_data & 0x04000000) != 0);
                red_tsn_cycle_support.append((temp_data & 0x08000000) != 0);
                red_tsn_maxsize_support.append((temp_data & 0x10000000) != 0);
                red_tsn_preemption_support.append((temp_data & 0x20000000) != 0);
                red_tsn_simple_scheduler_support.append((temp_data & 0x40000000) != 0);

            }
            else
            {
                red_tsn_priority_support.append(false);
                red_tsn_nr_of_priorities.append(0);
                red_tsn_phase_support.append(false);
                red_tsn_credit_support.append(false);
                red_tsn_cycle_support.append(false);
                red_tsn_maxsize_support.append(false);
                red_tsn_preemption_support.append(false);
                red_tsn_simple_scheduler_support.append(false);
            }
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            red_tsn_priority_support.append(false);
            red_tsn_nr_of_priorities.append(0);
            red_tsn_phase_support.append(false);
            red_tsn_credit_support.append(false);
            red_tsn_cycle_support.append(false);
            red_tsn_maxsize_support.append(false);
            red_tsn_preemption_support.append(false);
            red_tsn_simple_scheduler_support.append(false);
            return;
        }
    }

    if (instance == 1)
    {
        red_tsn_capabilities();
    }
}

int Ucm_RedTsnTab::red_tsn_enable(void)
{
    return 0;
}

int Ucm_RedTsnTab::red_tsn_disable(void)
{
    red_tsn_timer->stop();
    ui->RedTsnAutoRefreshButton->setText("Start Refresh");
    ui->RedTsnInstanceComboBox->setEnabled(true);
    ui->RedTsnReadValuesButton->setEnabled(true);
    ui->RedTsnWriteValuesButton->setEnabled(true);
    ui->RedTsnSchedulingButton->setEnabled(false);
    ui->RedTsnInstanceComboBox->clear();

    ui->RedTsnMacValue->setText("NA");
    ui->RedTsnVlanEnableCheckBox->setChecked(false);
    ui->RedTsnVlanValue->setText("NA");
    ui->RedTsnModeValue->setCurrentText("NA");
    ui->RedTsnVersionValue->setText("NA");
    ui->RedTsnEnableCheckBox->setChecked(false);
    ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
    ui->RedTsnNoForwardingCheckBox->setChecked(false);
    ui->RedTsnTailTaggingCheckBox->setChecked(false);
    ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
    ui->RedTsnLanACheckBox->setChecked(false);
    ui->RedTsnLanBCheckBox->setChecked(false);
    ui->RedTsnRxFrameAValue->setText("NA");
    ui->RedTsnRxErrorAValue->setText("NA");
    ui->RedTsnTxFrameAValue->setText("NA");
    ui->RedTsnTxErrorAValue->setText("NA");
    ui->RedTsnRxFrameBValue->setText("NA");
    ui->RedTsnRxErrorBValue->setText("NA");
    ui->RedTsnTxFrameBValue->setText("NA");
    ui->RedTsnTxErrorBValue->setText("NA");
    ui->RedTsnRxFrameCValue->setText("NA");
    ui->RedTsnRxErrorCValue->setText("NA");
    ui->RedTsnTxFrameCValue->setText("NA");
    ui->RedTsnTxErrorCValue->setText("NA");
    ui->RedTsnClearCountersCheckBox->setChecked(false);

    ui->RedTsnPriorityEnableCheckBox->setChecked(false);
    ui->RedTsnPriorityEnableCheckBox->setEnabled(false);

    ui->RedTsnPhasePeriodValue->setText("NA");
    ui->RedTsnPhasePeriodValue->setEnabled(false);

    ui->RedTsnPhaseEnableCheckBox->setChecked(false);
    ui->RedTsnPhaseEnableCheckBox->setEnabled(false);

    ui->RedTsnCycleEnableCheckBox->setChecked(false);
    ui->RedTsnCycleEnableCheckBox->setEnabled(false);

    ui->RedTsnCreditEnableCheckBox->setChecked(false);
    ui->RedTsnCreditEnableCheckBox->setEnabled(false);

    ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnableCheckBox->setEnabled(false);

    ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
    ui->RedTsnPreemptionEnableCheckBox->setEnabled(false);

    ui->RedTsnVlanPrio1Value->setCurrentIndex(1);
    ui->RedTsnVlanPrio1Value->setEnabled(false);
    ui->RedTsnPhaseStart1Value->setText("NA");
    ui->RedTsnPhaseStart1Value->setEnabled(false);
    ui->RedTsnPhaseEnd1Value->setText("NA");
    ui->RedTsnPhaseEnd1Value->setEnabled(false);
    ui->RedTsnCreditInc1Value->setText("NA");
    ui->RedTsnCreditInc1Value->setEnabled(false);
    ui->RedTsnCreditDec1Value->setText("NA");
    ui->RedTsnCreditDec1Value->setEnabled(false);
    ui->RedTsnMaxSize1Value->setText("NA");
    ui->RedTsnMaxSize1Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable1CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable1CheckBox->setChecked(false);
    ui->RedTsnCreditEnable1CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio2Value->setCurrentIndex(2);
    ui->RedTsnVlanPrio2Value->setEnabled(false);
    ui->RedTsnPhaseStart2Value->setText("NA");
    ui->RedTsnPhaseStart2Value->setEnabled(false);
    ui->RedTsnPhaseEnd2Value->setText("NA");
    ui->RedTsnPhaseEnd2Value->setEnabled(false);
    ui->RedTsnCreditInc2Value->setText("NA");
    ui->RedTsnCreditInc2Value->setEnabled(false);
    ui->RedTsnCreditDec2Value->setText("NA");
    ui->RedTsnCreditDec2Value->setEnabled(false);
    ui->RedTsnMaxSize2Value->setText("NA");
    ui->RedTsnMaxSize2Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable2CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable2CheckBox->setChecked(false);
    ui->RedTsnCreditEnable2CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio3Value->setCurrentIndex(3);
    ui->RedTsnVlanPrio3Value->setEnabled(false);
    ui->RedTsnPhaseStart3Value->setText("NA");
    ui->RedTsnPhaseStart3Value->setEnabled(false);
    ui->RedTsnPhaseEnd3Value->setText("NA");
    ui->RedTsnPhaseEnd3Value->setEnabled(false);
    ui->RedTsnCreditInc3Value->setText("NA");
    ui->RedTsnCreditInc3Value->setEnabled(false);
    ui->RedTsnCreditDec3Value->setText("NA");
    ui->RedTsnCreditDec3Value->setEnabled(false);
    ui->RedTsnMaxSize3Value->setText("NA");
    ui->RedTsnMaxSize3Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable3CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable3CheckBox->setChecked(false);
    ui->RedTsnCreditEnable3CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio4Value->setCurrentIndex(4);
    ui->RedTsnVlanPrio4Value->setEnabled(false);
    ui->RedTsnPhaseStart4Value->setText("NA");
    ui->RedTsnPhaseStart4Value->setEnabled(false);
    ui->RedTsnPhaseEnd4Value->setText("NA");
    ui->RedTsnPhaseEnd4Value->setEnabled(false);
    ui->RedTsnCreditInc4Value->setText("NA");
    ui->RedTsnCreditInc4Value->setEnabled(false);
    ui->RedTsnCreditDec4Value->setText("NA");
    ui->RedTsnCreditDec4Value->setEnabled(false);
    ui->RedTsnMaxSize4Value->setText("NA");
    ui->RedTsnMaxSize4Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable4CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable4CheckBox->setChecked(false);
    ui->RedTsnCreditEnable4CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio5Value->setCurrentIndex(5);
    ui->RedTsnVlanPrio5Value->setEnabled(false);
    ui->RedTsnPhaseStart5Value->setText("NA");
    ui->RedTsnPhaseStart5Value->setEnabled(false);
    ui->RedTsnPhaseEnd5Value->setText("NA");
    ui->RedTsnPhaseEnd5Value->setEnabled(false);
    ui->RedTsnCreditInc5Value->setText("NA");
    ui->RedTsnCreditInc5Value->setEnabled(false);
    ui->RedTsnCreditDec5Value->setText("NA");
    ui->RedTsnCreditDec5Value->setEnabled(false);
    ui->RedTsnMaxSize5Value->setText("NA");
    ui->RedTsnMaxSize5Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable5CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable5CheckBox->setChecked(false);
    ui->RedTsnCreditEnable5CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio6Value->setCurrentIndex(6);
    ui->RedTsnVlanPrio6Value->setEnabled(false);
    ui->RedTsnPhaseStart6Value->setText("NA");
    ui->RedTsnPhaseStart6Value->setEnabled(false);
    ui->RedTsnPhaseEnd6Value->setText("NA");
    ui->RedTsnPhaseEnd6Value->setEnabled(false);
    ui->RedTsnCreditInc6Value->setText("NA");
    ui->RedTsnCreditInc6Value->setEnabled(false);
    ui->RedTsnCreditDec6Value->setText("NA");
    ui->RedTsnCreditDec6Value->setEnabled(false);
    ui->RedTsnMaxSize6Value->setText("NA");
    ui->RedTsnMaxSize6Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable6CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable6CheckBox->setChecked(false);
    ui->RedTsnCreditEnable6CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio7Value->setCurrentIndex(7);
    ui->RedTsnVlanPrio7Value->setEnabled(false);
    ui->RedTsnPhaseStart7Value->setText("NA");
    ui->RedTsnPhaseStart7Value->setEnabled(false);
    ui->RedTsnPhaseEnd7Value->setText("NA");
    ui->RedTsnPhaseEnd7Value->setEnabled(false);
    ui->RedTsnCreditInc7Value->setText("NA");
    ui->RedTsnCreditInc7Value->setEnabled(false);
    ui->RedTsnCreditDec7Value->setText("NA");
    ui->RedTsnCreditDec7Value->setEnabled(false);
    ui->RedTsnMaxSize7Value->setText("NA");
    ui->RedTsnMaxSize7Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable7CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable7CheckBox->setChecked(false);
    ui->RedTsnCreditEnable7CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(false);

    ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
    ui->RedTsnVlanPrio8Value->setEnabled(false);
    ui->RedTsnPhaseStart8Value->setText("NA");
    ui->RedTsnPhaseStart8Value->setEnabled(false);
    ui->RedTsnPhaseEnd8Value->setText("NA");
    ui->RedTsnPhaseEnd8Value->setEnabled(false);
    ui->RedTsnCreditInc8Value->setText("NA");
    ui->RedTsnCreditInc8Value->setEnabled(false);
    ui->RedTsnCreditDec8Value->setText("NA");
    ui->RedTsnCreditDec8Value->setEnabled(false);
    ui->RedTsnMaxSize8Value->setText("NA");
    ui->RedTsnMaxSize8Value->setEnabled(false);
    ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
    ui->RedTsnPrioPhaseEnable8CheckBox->setEnabled(false);
    ui->RedTsnCreditEnable8CheckBox->setChecked(false);
    ui->RedTsnCreditEnable8CheckBox->setEnabled(false);
    ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(false);

    ui_scheduling->close();
    ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
    ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(false);
    ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
    ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
    ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
    ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
    ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(false);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(false);

    red_tsn_priority_support.clear();
    red_tsn_nr_of_priorities.clear();
    red_tsn_phase_support.clear();
    red_tsn_credit_support.clear();
    red_tsn_cycle_support.clear();
    red_tsn_maxsize_support.clear();
    red_tsn_preemption_support.clear();
    red_tsn_simple_scheduler_support.clear();

    return 0;
}

void  Ucm_RedTsnTab::red_tsn_capabilities(void)
{
    unsigned int temp_data = 0;
    QString temp_string;

    temp_string = ui->RedTsnInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    temp_data = temp_data -1;

    // enable all first
    ui->RedTsnSchedulingButton->setEnabled(true);

    ui->RedTsnPriorityEnableCheckBox->setEnabled(true);
    ui->RedTsnPriorityEnableCheckBox->setChecked(false);

    ui->RedTsnPhasePeriodValue->setEnabled(true);
    ui->RedTsnPhasePeriodValue->setText("NA");

    ui->RedTsnPhaseEnableCheckBox->setEnabled(true);
    ui->RedTsnPhaseEnableCheckBox->setChecked(false);

    ui->RedTsnCycleEnableCheckBox->setEnabled(true);
    ui->RedTsnCycleEnableCheckBox->setChecked(false);

    ui->RedTsnCreditEnableCheckBox->setEnabled(true);
    ui->RedTsnCreditEnableCheckBox->setChecked(false);

    ui->RedTsnMaxSizeEnableCheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);

    ui->RedTsnPreemptionEnableCheckBox->setEnabled(true);
    ui->RedTsnPreemptionEnableCheckBox->setChecked(false);

    ui->RedTsnVlanPrio1Value->setEnabled(true);
    ui->RedTsnVlanPrio1Value->setCurrentIndex(1);
    ui->RedTsnPhaseStart1Value->setEnabled(true);
    ui->RedTsnPhaseStart1Value->setText("NA");
    ui->RedTsnPhaseEnd1Value->setEnabled(true);
    ui->RedTsnPhaseEnd1Value->setText("NA");
    ui->RedTsnCreditInc1Value->setEnabled(true);
    ui->RedTsnCreditInc1Value->setText("NA");
    ui->RedTsnCreditDec1Value->setEnabled(true);
    ui->RedTsnCreditDec1Value->setText("NA");
    ui->RedTsnMaxSize1Value->setEnabled(true);
    ui->RedTsnMaxSize1Value->setText("NA");
    ui->RedTsnPrioPhaseEnable1CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
    ui->RedTsnCreditEnable1CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable1CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);

    ui->RedTsnVlanPrio2Value->setEnabled(true);
    ui->RedTsnVlanPrio2Value->setCurrentIndex(2);
    ui->RedTsnPhaseStart2Value->setEnabled(true);
    ui->RedTsnPhaseStart2Value->setText("NA");
    ui->RedTsnPhaseEnd2Value->setEnabled(true);
    ui->RedTsnPhaseEnd2Value->setText("NA");
    ui->RedTsnCreditInc2Value->setEnabled(true);
    ui->RedTsnCreditInc2Value->setText("NA");
    ui->RedTsnCreditDec2Value->setEnabled(true);
    ui->RedTsnCreditDec2Value->setText("NA");
    ui->RedTsnMaxSize2Value->setEnabled(true);
    ui->RedTsnMaxSize2Value->setText("NA");
    ui->RedTsnPrioPhaseEnable2CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
    ui->RedTsnCreditEnable2CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable2CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);

    ui->RedTsnVlanPrio3Value->setEnabled(true);
    ui->RedTsnVlanPrio3Value->setCurrentIndex(3);
    ui->RedTsnPhaseStart3Value->setEnabled(true);
    ui->RedTsnPhaseStart3Value->setText("NA");
    ui->RedTsnPhaseEnd3Value->setEnabled(true);
    ui->RedTsnPhaseEnd3Value->setText("NA");
    ui->RedTsnCreditInc3Value->setEnabled(true);
    ui->RedTsnCreditInc3Value->setText("NA");
    ui->RedTsnCreditDec3Value->setEnabled(true);
    ui->RedTsnCreditDec3Value->setText("NA");
    ui->RedTsnMaxSize3Value->setEnabled(true);
    ui->RedTsnMaxSize3Value->setText("NA");
    ui->RedTsnPrioPhaseEnable3CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
    ui->RedTsnCreditEnable3CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable3CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);

    ui->RedTsnVlanPrio4Value->setEnabled(true);
    ui->RedTsnVlanPrio4Value->setCurrentIndex(4);
    ui->RedTsnPhaseStart4Value->setEnabled(true);
    ui->RedTsnPhaseStart4Value->setText("NA");
    ui->RedTsnPhaseEnd4Value->setEnabled(true);
    ui->RedTsnPhaseEnd4Value->setText("NA");
    ui->RedTsnCreditInc4Value->setEnabled(true);
    ui->RedTsnCreditInc4Value->setText("NA");
    ui->RedTsnCreditDec4Value->setEnabled(true);
    ui->RedTsnCreditDec4Value->setText("NA");
    ui->RedTsnMaxSize4Value->setEnabled(true);
    ui->RedTsnMaxSize4Value->setText("NA");
    ui->RedTsnPrioPhaseEnable4CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(true);
    ui->RedTsnCreditEnable4CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable4CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);

    ui->RedTsnVlanPrio5Value->setEnabled(true);
    ui->RedTsnVlanPrio5Value->setCurrentIndex(5);
    ui->RedTsnPhaseStart5Value->setEnabled(true);
    ui->RedTsnPhaseStart5Value->setText("NA");
    ui->RedTsnPhaseEnd5Value->setEnabled(true);
    ui->RedTsnPhaseEnd5Value->setText("NA");
    ui->RedTsnCreditInc5Value->setEnabled(true);
    ui->RedTsnCreditInc5Value->setText("NA");
    ui->RedTsnCreditDec5Value->setEnabled(true);
    ui->RedTsnCreditDec5Value->setText("NA");
    ui->RedTsnMaxSize5Value->setEnabled(true);
    ui->RedTsnMaxSize5Value->setText("NA");
    ui->RedTsnPrioPhaseEnable5CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
    ui->RedTsnCreditEnable5CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable5CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);

    ui->RedTsnVlanPrio6Value->setEnabled(true);
    ui->RedTsnVlanPrio6Value->setCurrentIndex(6);
    ui->RedTsnPhaseStart6Value->setEnabled(true);
    ui->RedTsnPhaseStart6Value->setText("NA");
    ui->RedTsnPhaseEnd6Value->setEnabled(true);
    ui->RedTsnPhaseEnd6Value->setText("NA");
    ui->RedTsnCreditInc6Value->setEnabled(true);
    ui->RedTsnCreditInc6Value->setText("NA");
    ui->RedTsnCreditDec6Value->setEnabled(true);
    ui->RedTsnCreditDec6Value->setText("NA");
    ui->RedTsnMaxSize6Value->setEnabled(true);
    ui->RedTsnMaxSize6Value->setText("NA");
    ui->RedTsnPrioPhaseEnable6CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
    ui->RedTsnCreditEnable6CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable6CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);

    ui->RedTsnVlanPrio7Value->setEnabled(true);
    ui->RedTsnVlanPrio7Value->setCurrentIndex(7);
    ui->RedTsnPhaseStart7Value->setEnabled(true);
    ui->RedTsnPhaseStart7Value->setText("NA");
    ui->RedTsnPhaseEnd7Value->setEnabled(true);
    ui->RedTsnPhaseEnd7Value->setText("NA");
    ui->RedTsnCreditInc7Value->setEnabled(true);
    ui->RedTsnCreditInc7Value->setText("NA");
    ui->RedTsnCreditDec7Value->setEnabled(true);
    ui->RedTsnCreditDec7Value->setText("NA");
    ui->RedTsnMaxSize7Value->setEnabled(true);
    ui->RedTsnMaxSize7Value->setText("NA");
    ui->RedTsnPrioPhaseEnable7CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
    ui->RedTsnCreditEnable7CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable7CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);

    ui->RedTsnVlanPrio8Value->setEnabled(true);
    ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
    ui->RedTsnPhaseStart8Value->setEnabled(true);
    ui->RedTsnPhaseStart8Value->setText("NA");
    ui->RedTsnPhaseEnd8Value->setEnabled(true);
    ui->RedTsnPhaseEnd8Value->setText("NA");
    ui->RedTsnCreditInc8Value->setEnabled(true);
    ui->RedTsnCreditInc8Value->setText("NA");
    ui->RedTsnCreditDec8Value->setEnabled(true);
    ui->RedTsnCreditDec8Value->setText("NA");
    ui->RedTsnMaxSize8Value->setEnabled(true);
    ui->RedTsnMaxSize8Value->setText("NA");
    ui->RedTsnPrioPhaseEnable8CheckBox->setEnabled(true);
    ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
    ui->RedTsnCreditEnable8CheckBox->setEnabled(true);
    ui->RedTsnCreditEnable8CheckBox->setChecked(false);
    ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(true);
    ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);

    ui->RedTsnCreditIncPortCValue->setEnabled(true);
    ui->RedTsnCreditIncPortCValue->setText("NA");
    ui->RedTsnCreditDecPortCValue->setEnabled(true);
    ui->RedTsnCreditDecPortCValue->setText("NA");
    ui->RedTsnCreditEnablePortCCheckBox->setEnabled(true);
    ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);

    ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
    ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(true);
    ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
    ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
    ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
    ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
    ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(true);

    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
    ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(true);
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
    ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(true);

    if (red_tsn_priority_support.at(temp_data) == false)
    {
        ui->RedTsnSchedulingButton->setEnabled(false);

        ui->RedTsnPriorityEnableCheckBox->setChecked(false);
        ui->RedTsnPriorityEnableCheckBox->setEnabled(false);

        ui->RedTsnPhasePeriodValue->setText("NA");
        ui->RedTsnPhasePeriodValue->setEnabled(false);

        ui->RedTsnPhaseEnableCheckBox->setChecked(false);
        ui->RedTsnPhaseEnableCheckBox->setEnabled(false);

        ui->RedTsnCycleEnableCheckBox->setChecked(false);
        ui->RedTsnCycleEnableCheckBox->setEnabled(false);

        ui->RedTsnCreditEnableCheckBox->setChecked(false);
        ui->RedTsnCreditEnableCheckBox->setEnabled(false);

        ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnableCheckBox->setEnabled(false);

        ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
        ui->RedTsnPreemptionEnableCheckBox->setEnabled(false);

        ui->RedTsnVlanPrio1Value->setCurrentIndex(1);
        ui->RedTsnVlanPrio1Value->setEnabled(false);
        ui->RedTsnPhaseStart1Value->setText("NA");
        ui->RedTsnPhaseStart1Value->setEnabled(false);
        ui->RedTsnPhaseEnd1Value->setText("NA");
        ui->RedTsnPhaseEnd1Value->setEnabled(false);
        ui->RedTsnCreditInc1Value->setText("NA");
        ui->RedTsnCreditInc1Value->setEnabled(false);
        ui->RedTsnCreditDec1Value->setText("NA");
        ui->RedTsnCreditDec1Value->setEnabled(false);
        ui->RedTsnMaxSize1Value->setText("NA");
        ui->RedTsnMaxSize1Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable1CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable1CheckBox->setChecked(false);
        ui->RedTsnCreditEnable1CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio2Value->setCurrentIndex(2);
        ui->RedTsnVlanPrio2Value->setEnabled(false);
        ui->RedTsnPhaseStart2Value->setText("NA");
        ui->RedTsnPhaseStart2Value->setEnabled(false);
        ui->RedTsnPhaseEnd2Value->setText("NA");
        ui->RedTsnPhaseEnd2Value->setEnabled(false);
        ui->RedTsnCreditInc2Value->setText("NA");
        ui->RedTsnCreditInc2Value->setEnabled(false);
        ui->RedTsnCreditDec2Value->setText("NA");
        ui->RedTsnCreditDec2Value->setEnabled(false);
        ui->RedTsnMaxSize2Value->setText("NA");
        ui->RedTsnMaxSize2Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable2CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable2CheckBox->setChecked(false);
        ui->RedTsnCreditEnable2CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio3Value->setCurrentIndex(3);
        ui->RedTsnVlanPrio3Value->setEnabled(false);
        ui->RedTsnPhaseStart3Value->setText("NA");
        ui->RedTsnPhaseStart3Value->setEnabled(false);
        ui->RedTsnPhaseEnd3Value->setText("NA");
        ui->RedTsnPhaseEnd3Value->setEnabled(false);
        ui->RedTsnCreditInc3Value->setText("NA");
        ui->RedTsnCreditInc3Value->setEnabled(false);
        ui->RedTsnCreditDec3Value->setText("NA");
        ui->RedTsnCreditDec3Value->setEnabled(false);
        ui->RedTsnMaxSize3Value->setText("NA");
        ui->RedTsnMaxSize3Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable3CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable3CheckBox->setChecked(false);
        ui->RedTsnCreditEnable3CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio4Value->setCurrentIndex(4);
        ui->RedTsnVlanPrio4Value->setEnabled(false);
        ui->RedTsnPhaseStart4Value->setText("NA");
        ui->RedTsnPhaseStart4Value->setEnabled(false);
        ui->RedTsnPhaseEnd4Value->setText("NA");
        ui->RedTsnPhaseEnd4Value->setEnabled(false);
        ui->RedTsnCreditInc4Value->setText("NA");
        ui->RedTsnCreditInc4Value->setEnabled(false);
        ui->RedTsnCreditDec4Value->setText("NA");
        ui->RedTsnCreditDec4Value->setEnabled(false);
        ui->RedTsnMaxSize4Value->setText("NA");
        ui->RedTsnMaxSize4Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable4CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable4CheckBox->setChecked(false);
        ui->RedTsnCreditEnable4CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio5Value->setCurrentIndex(5);
        ui->RedTsnVlanPrio5Value->setEnabled(false);
        ui->RedTsnPhaseStart5Value->setText("NA");
        ui->RedTsnPhaseStart5Value->setEnabled(false);
        ui->RedTsnPhaseEnd5Value->setText("NA");
        ui->RedTsnPhaseEnd5Value->setEnabled(false);
        ui->RedTsnCreditInc5Value->setText("NA");
        ui->RedTsnCreditInc5Value->setEnabled(false);
        ui->RedTsnCreditDec5Value->setText("NA");
        ui->RedTsnCreditDec5Value->setEnabled(false);
        ui->RedTsnMaxSize5Value->setText("NA");
        ui->RedTsnMaxSize5Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable5CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable5CheckBox->setChecked(false);
        ui->RedTsnCreditEnable5CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio6Value->setCurrentIndex(6);
        ui->RedTsnVlanPrio6Value->setEnabled(false);
        ui->RedTsnPhaseStart6Value->setText("NA");
        ui->RedTsnPhaseStart6Value->setEnabled(false);
        ui->RedTsnPhaseEnd6Value->setText("NA");
        ui->RedTsnPhaseEnd6Value->setEnabled(false);
        ui->RedTsnCreditInc6Value->setText("NA");
        ui->RedTsnCreditInc6Value->setEnabled(false);
        ui->RedTsnCreditDec6Value->setText("NA");
        ui->RedTsnCreditDec6Value->setEnabled(false);
        ui->RedTsnMaxSize6Value->setText("NA");
        ui->RedTsnMaxSize6Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable6CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable6CheckBox->setChecked(false);
        ui->RedTsnCreditEnable6CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio7Value->setCurrentIndex(7);
        ui->RedTsnVlanPrio7Value->setEnabled(false);
        ui->RedTsnPhaseStart7Value->setText("NA");
        ui->RedTsnPhaseStart7Value->setEnabled(false);
        ui->RedTsnPhaseEnd7Value->setText("NA");
        ui->RedTsnPhaseEnd7Value->setEnabled(false);
        ui->RedTsnCreditInc7Value->setText("NA");
        ui->RedTsnCreditInc7Value->setEnabled(false);
        ui->RedTsnCreditDec7Value->setText("NA");
        ui->RedTsnCreditDec7Value->setEnabled(false);
        ui->RedTsnMaxSize7Value->setText("NA");
        ui->RedTsnMaxSize7Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable7CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable7CheckBox->setChecked(false);
        ui->RedTsnCreditEnable7CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(false);

        ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio8Value->setEnabled(false);
        ui->RedTsnPhaseStart8Value->setText("NA");
        ui->RedTsnPhaseStart8Value->setEnabled(false);
        ui->RedTsnPhaseEnd8Value->setText("NA");
        ui->RedTsnPhaseEnd8Value->setEnabled(false);
        ui->RedTsnCreditInc8Value->setText("NA");
        ui->RedTsnCreditInc8Value->setEnabled(false);
        ui->RedTsnCreditDec8Value->setText("NA");
        ui->RedTsnCreditDec8Value->setEnabled(false);
        ui->RedTsnMaxSize8Value->setText("NA");
        ui->RedTsnMaxSize8Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable8CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable8CheckBox->setChecked(false);
        ui->RedTsnCreditEnable8CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(false);

        ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(false);
    }

    if (red_tsn_phase_support.at(temp_data) == false)
    {
        ui->RedTsnSchedulingButton->setEnabled(false);

        ui->RedTsnPhasePeriodValue->setText("NA");
        ui->RedTsnPhasePeriodValue->setEnabled(false);

        ui->RedTsnPhaseEnableCheckBox->setChecked(false);
        ui->RedTsnPhaseEnableCheckBox->setEnabled(false);

        ui->RedTsnCycleEnableCheckBox->setChecked(false);
        ui->RedTsnCycleEnableCheckBox->setEnabled(false);

        ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
        ui->RedTsnPreemptionEnableCheckBox->setEnabled(false);

        ui->RedTsnPhaseStart1Value->setText("NA");
        ui->RedTsnPhaseStart1Value->setEnabled(false);
        ui->RedTsnPhaseEnd1Value->setText("NA");
        ui->RedTsnPhaseEnd1Value->setEnabled(false);

        ui->RedTsnPhaseStart2Value->setText("NA");
        ui->RedTsnPhaseStart2Value->setEnabled(false);
        ui->RedTsnPhaseEnd2Value->setText("NA");
        ui->RedTsnPhaseEnd2Value->setEnabled(false);

        ui->RedTsnPhaseStart3Value->setText("NA");
        ui->RedTsnPhaseStart3Value->setEnabled(false);
        ui->RedTsnPhaseEnd3Value->setText("NA");
        ui->RedTsnPhaseEnd3Value->setEnabled(false);

        ui->RedTsnPhaseStart4Value->setText("NA");
        ui->RedTsnPhaseStart4Value->setEnabled(false);
        ui->RedTsnPhaseEnd4Value->setText("NA");
        ui->RedTsnPhaseEnd4Value->setEnabled(false);

        ui->RedTsnPhaseStart5Value->setText("NA");
        ui->RedTsnPhaseStart5Value->setEnabled(false);
        ui->RedTsnPhaseEnd5Value->setText("NA");
        ui->RedTsnPhaseEnd5Value->setEnabled(false);

        ui->RedTsnPhaseStart6Value->setText("NA");
        ui->RedTsnPhaseStart6Value->setEnabled(false);
        ui->RedTsnPhaseEnd6Value->setText("NA");
        ui->RedTsnPhaseEnd6Value->setEnabled(false);

        ui->RedTsnPhaseStart7Value->setText("NA");
        ui->RedTsnPhaseStart7Value->setEnabled(false);
        ui->RedTsnPhaseEnd7Value->setText("NA");
        ui->RedTsnPhaseEnd7Value->setEnabled(false);

        ui->RedTsnPhaseStart8Value->setText("NA");
        ui->RedTsnPhaseStart8Value->setEnabled(false);
        ui->RedTsnPhaseEnd8Value->setText("NA");
        ui->RedTsnPhaseEnd8Value->setEnabled(false);

        ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(false);
    }

    if (red_tsn_simple_scheduler_support.at(temp_data) == true)
    {
        ui->RedTsnSchedulingButton->setEnabled(false);

        ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(false);

        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(false);
    }

    if (red_tsn_simple_scheduler_support.at(temp_data) == false)
    {
        ui->RedTsnPhasePeriodValue->setText("NA");
        ui->RedTsnPhasePeriodValue->setEnabled(false);

        ui->RedTsnPhaseStart1Value->setText("NA");
        ui->RedTsnPhaseStart1Value->setEnabled(false);
        ui->RedTsnPhaseEnd1Value->setText("NA");
        ui->RedTsnPhaseEnd1Value->setEnabled(false);

        ui->RedTsnPhaseStart2Value->setText("NA");
        ui->RedTsnPhaseStart2Value->setEnabled(false);
        ui->RedTsnPhaseEnd2Value->setText("NA");
        ui->RedTsnPhaseEnd2Value->setEnabled(false);

        ui->RedTsnPhaseStart3Value->setText("NA");
        ui->RedTsnPhaseStart3Value->setEnabled(false);
        ui->RedTsnPhaseEnd3Value->setText("NA");
        ui->RedTsnPhaseEnd3Value->setEnabled(false);

        ui->RedTsnPhaseStart4Value->setText("NA");
        ui->RedTsnPhaseStart4Value->setEnabled(false);
        ui->RedTsnPhaseEnd4Value->setText("NA");
        ui->RedTsnPhaseEnd4Value->setEnabled(false);

        ui->RedTsnPhaseStart5Value->setText("NA");
        ui->RedTsnPhaseStart5Value->setEnabled(false);
        ui->RedTsnPhaseEnd5Value->setText("NA");
        ui->RedTsnPhaseEnd5Value->setEnabled(false);

        ui->RedTsnPhaseStart6Value->setText("NA");
        ui->RedTsnPhaseStart6Value->setEnabled(false);
        ui->RedTsnPhaseEnd6Value->setText("NA");
        ui->RedTsnPhaseEnd6Value->setEnabled(false);

        ui->RedTsnPhaseStart7Value->setText("NA");
        ui->RedTsnPhaseStart7Value->setEnabled(false);
        ui->RedTsnPhaseEnd7Value->setText("NA");
        ui->RedTsnPhaseEnd7Value->setEnabled(false);

        ui->RedTsnPhaseStart8Value->setText("NA");
        ui->RedTsnPhaseStart8Value->setEnabled(false);
        ui->RedTsnPhaseEnd8Value->setText("NA");
        ui->RedTsnPhaseEnd8Value->setEnabled(false);
    }

    if (red_tsn_cycle_support.at(temp_data) == false)
    {
        ui->RedTsnCycleEnableCheckBox->setChecked(false);
        ui->RedTsnCycleEnableCheckBox->setEnabled(false);
    }

    if (red_tsn_credit_support.at(temp_data) == false)
    {
        ui->RedTsnCreditEnableCheckBox->setChecked(false);
        ui->RedTsnCreditEnableCheckBox->setEnabled(false);

        ui->RedTsnCreditInc1Value->setText("NA");
        ui->RedTsnCreditInc1Value->setEnabled(false);
        ui->RedTsnCreditDec1Value->setText("NA");
        ui->RedTsnCreditDec1Value->setEnabled(false);
        ui->RedTsnCreditEnable1CheckBox->setChecked(false);
        ui->RedTsnCreditEnable1CheckBox->setEnabled(false);

        ui->RedTsnCreditInc2Value->setText("NA");
        ui->RedTsnCreditInc2Value->setEnabled(false);
        ui->RedTsnCreditDec2Value->setText("NA");
        ui->RedTsnCreditDec2Value->setEnabled(false);
        ui->RedTsnCreditEnable2CheckBox->setChecked(false);
        ui->RedTsnCreditEnable2CheckBox->setEnabled(false);

        ui->RedTsnCreditInc3Value->setText("NA");
        ui->RedTsnCreditInc3Value->setEnabled(false);
        ui->RedTsnCreditDec3Value->setText("NA");
        ui->RedTsnCreditDec3Value->setEnabled(false);
        ui->RedTsnCreditEnable3CheckBox->setChecked(false);
        ui->RedTsnCreditEnable3CheckBox->setEnabled(false);

        ui->RedTsnCreditInc4Value->setText("NA");
        ui->RedTsnCreditInc4Value->setEnabled(false);
        ui->RedTsnCreditDec4Value->setText("NA");
        ui->RedTsnCreditDec4Value->setEnabled(false);
        ui->RedTsnCreditEnable4CheckBox->setChecked(false);
        ui->RedTsnCreditEnable4CheckBox->setEnabled(false);

        ui->RedTsnCreditInc5Value->setText("NA");
        ui->RedTsnCreditInc5Value->setEnabled(false);
        ui->RedTsnCreditDec5Value->setText("NA");
        ui->RedTsnCreditDec5Value->setEnabled(false);
        ui->RedTsnCreditEnable5CheckBox->setChecked(false);
        ui->RedTsnCreditEnable5CheckBox->setEnabled(false);

        ui->RedTsnCreditInc6Value->setText("NA");
        ui->RedTsnCreditInc6Value->setEnabled(false);
        ui->RedTsnCreditDec6Value->setText("NA");
        ui->RedTsnCreditDec6Value->setEnabled(false);
        ui->RedTsnCreditEnable6CheckBox->setChecked(false);
        ui->RedTsnCreditEnable6CheckBox->setEnabled(false);

        ui->RedTsnCreditInc7Value->setText("NA");
        ui->RedTsnCreditInc7Value->setEnabled(false);
        ui->RedTsnCreditDec7Value->setText("NA");
        ui->RedTsnCreditDec7Value->setEnabled(false);
        ui->RedTsnCreditEnable7CheckBox->setChecked(false);
        ui->RedTsnCreditEnable7CheckBox->setEnabled(false);

        ui->RedTsnCreditInc8Value->setText("NA");
        ui->RedTsnCreditInc8Value->setEnabled(false);
        ui->RedTsnCreditDec8Value->setText("NA");
        ui->RedTsnCreditDec8Value->setEnabled(false);
        ui->RedTsnCreditEnable8CheckBox->setChecked(false);
        ui->RedTsnCreditEnable8CheckBox->setEnabled(false);

        ui->RedTsnCreditIncPortCValue->setText("NA");
        ui->RedTsnCreditIncPortCValue->setEnabled(false);
        ui->RedTsnCreditDecPortCValue->setText("NA");
        ui->RedTsnCreditDecPortCValue->setEnabled(false);
        ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
        ui->RedTsnCreditEnablePortCCheckBox->setEnabled(false);
    }

    if (red_tsn_maxsize_support.at(temp_data) == false)
    {        
        ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnableCheckBox->setEnabled(false);

        ui->RedTsnMaxSize1Value->setText("NA");
        ui->RedTsnMaxSize1Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(false);

        ui->RedTsnMaxSize2Value->setText("NA");
        ui->RedTsnMaxSize2Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(false);

        ui->RedTsnMaxSize3Value->setText("NA");
        ui->RedTsnMaxSize3Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(false);

        ui->RedTsnMaxSize4Value->setText("NA");
        ui->RedTsnMaxSize4Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(false);

        ui->RedTsnMaxSize5Value->setText("NA");
        ui->RedTsnMaxSize5Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(false);

        ui->RedTsnMaxSize6Value->setText("NA");
        ui->RedTsnMaxSize6Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(false);

        ui->RedTsnMaxSize7Value->setText("NA");
        ui->RedTsnMaxSize7Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(false);

        ui->RedTsnMaxSize8Value->setText("NA");
        ui->RedTsnMaxSize8Value->setEnabled(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(false);
    }

    if (red_tsn_preemption_support.at(temp_data) == false)
    {
        ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnableCheckBox->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 1)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 1)
        {
            ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio1Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio1Value->setEnabled(false);
        ui->RedTsnPhaseStart1Value->setText("NA");
        ui->RedTsnPhaseStart1Value->setEnabled(false);
        ui->RedTsnPhaseEnd1Value->setText("NA");
        ui->RedTsnPhaseEnd1Value->setEnabled(false);
        ui->RedTsnCreditInc1Value->setText("NA");
        ui->RedTsnCreditInc1Value->setEnabled(false);
        ui->RedTsnCreditDec1Value->setText("NA");
        ui->RedTsnCreditDec1Value->setEnabled(false);
        ui->RedTsnMaxSize1Value->setText("NA");
        ui->RedTsnMaxSize1Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable1CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable1CheckBox->setChecked(false);
        ui->RedTsnCreditEnable1CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 2)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 2)
        {
            ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio2Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio2Value->setEnabled(false);
        ui->RedTsnPhaseStart2Value->setText("NA");
        ui->RedTsnPhaseStart2Value->setEnabled(false);
        ui->RedTsnPhaseEnd2Value->setText("NA");
        ui->RedTsnPhaseEnd2Value->setEnabled(false);
        ui->RedTsnCreditInc2Value->setText("NA");
        ui->RedTsnCreditInc2Value->setEnabled(false);
        ui->RedTsnCreditDec2Value->setText("NA");
        ui->RedTsnCreditDec2Value->setEnabled(false);
        ui->RedTsnMaxSize2Value->setText("NA");
        ui->RedTsnMaxSize2Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable2CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable2CheckBox->setChecked(false);
        ui->RedTsnCreditEnable2CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 3)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 3)
        {
            ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio3Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio3Value->setEnabled(false);
        ui->RedTsnPhaseStart3Value->setText("NA");
        ui->RedTsnPhaseStart3Value->setEnabled(false);
        ui->RedTsnPhaseEnd3Value->setText("NA");
        ui->RedTsnPhaseEnd3Value->setEnabled(false);
        ui->RedTsnCreditInc3Value->setText("NA");
        ui->RedTsnCreditInc3Value->setEnabled(false);
        ui->RedTsnCreditDec3Value->setText("NA");
        ui->RedTsnCreditDec3Value->setEnabled(false);
        ui->RedTsnMaxSize3Value->setText("NA");
        ui->RedTsnMaxSize3Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable3CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable3CheckBox->setChecked(false);
        ui->RedTsnCreditEnable3CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 4)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 4)
        {
            ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio4Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio4Value->setEnabled(false);
        ui->RedTsnPhaseStart4Value->setText("NA");
        ui->RedTsnPhaseStart4Value->setEnabled(false);
        ui->RedTsnPhaseEnd4Value->setText("NA");
        ui->RedTsnPhaseEnd4Value->setEnabled(false);
        ui->RedTsnCreditInc4Value->setText("NA");
        ui->RedTsnCreditInc4Value->setEnabled(false);
        ui->RedTsnCreditDec4Value->setText("NA");
        ui->RedTsnCreditDec4Value->setEnabled(false);
        ui->RedTsnMaxSize4Value->setText("NA");
        ui->RedTsnMaxSize4Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable4CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable4CheckBox->setChecked(false);
        ui->RedTsnCreditEnable4CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 5)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 5)
        {
            ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio5Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio5Value->setEnabled(false);
        ui->RedTsnPhaseStart5Value->setText("NA");
        ui->RedTsnPhaseStart5Value->setEnabled(false);
        ui->RedTsnPhaseEnd5Value->setText("NA");
        ui->RedTsnPhaseEnd5Value->setEnabled(false);
        ui->RedTsnCreditInc5Value->setText("NA");
        ui->RedTsnCreditInc5Value->setEnabled(false);
        ui->RedTsnCreditDec5Value->setText("NA");
        ui->RedTsnCreditDec5Value->setEnabled(false);
        ui->RedTsnMaxSize5Value->setText("NA");
        ui->RedTsnMaxSize5Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable5CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable5CheckBox->setChecked(false);
        ui->RedTsnCreditEnable5CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 6)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 6)
        {
            ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio6Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio6Value->setEnabled(false);
        ui->RedTsnPhaseStart6Value->setText("NA");
        ui->RedTsnPhaseStart6Value->setEnabled(false);
        ui->RedTsnPhaseEnd6Value->setText("NA");
        ui->RedTsnPhaseEnd6Value->setEnabled(false);
        ui->RedTsnCreditInc6Value->setText("NA");
        ui->RedTsnCreditInc6Value->setEnabled(false);
        ui->RedTsnCreditDec6Value->setText("NA");
        ui->RedTsnCreditDec6Value->setEnabled(false);
        ui->RedTsnMaxSize6Value->setText("NA");
        ui->RedTsnMaxSize6Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable6CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable6CheckBox->setChecked(false);
        ui->RedTsnCreditEnable6CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 7)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 7)
        {
            ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio7Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio7Value->setEnabled(false);
        ui->RedTsnPhaseStart7Value->setText("NA");
        ui->RedTsnPhaseStart7Value->setEnabled(false);
        ui->RedTsnPhaseEnd7Value->setText("NA");
        ui->RedTsnPhaseEnd7Value->setEnabled(false);
        ui->RedTsnCreditInc7Value->setText("NA");
        ui->RedTsnCreditInc7Value->setEnabled(false);
        ui->RedTsnCreditDec7Value->setText("NA");
        ui->RedTsnCreditDec7Value->setEnabled(false);
        ui->RedTsnMaxSize7Value->setText("NA");
        ui->RedTsnMaxSize7Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable7CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable7CheckBox->setChecked(false);
        ui->RedTsnCreditEnable7CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
    }

    if (red_tsn_nr_of_priorities.at(temp_data) >= 8)
    {
        if (red_tsn_nr_of_priorities.at(temp_data) == 8)
        {
            ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio8Value->setEnabled(false);
        }
    }
    else
    {
        ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
        ui->RedTsnVlanPrio8Value->setEnabled(false);
        ui->RedTsnPhaseStart8Value->setText("NA");
        ui->RedTsnPhaseStart8Value->setEnabled(false);
        ui->RedTsnPhaseEnd8Value->setText("NA");
        ui->RedTsnPhaseEnd8Value->setEnabled(false);
        ui->RedTsnCreditInc8Value->setText("NA");
        ui->RedTsnCreditInc8Value->setEnabled(false);
        ui->RedTsnCreditDec8Value->setText("NA");
        ui->RedTsnCreditDec8Value->setEnabled(false);
        ui->RedTsnMaxSize8Value->setText("NA");
        ui->RedTsnMaxSize8Value->setEnabled(false);
        ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable8CheckBox->setEnabled(false);
        ui->RedTsnCreditEnable8CheckBox->setChecked(false);
        ui->RedTsnCreditEnable8CheckBox->setEnabled(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
    }
}

void Ucm_RedTsnTab::red_tsn_read_values(void)
{
    unsigned int instance_idx = 0;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->RedTsnInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    instance_idx = temp_data -1;
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RedTsnCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->RedTsnMacValue->setText("NA");
            ui->RedTsnVlanEnableCheckBox->setChecked(false);
            ui->RedTsnVlanValue->setText("NA");
            ui->RedTsnModeValue->setCurrentText("NA");
            ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
            ui->RedTsnNoForwardingCheckBox->setChecked(false);
            ui->RedTsnTailTaggingCheckBox->setChecked(false);
            ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
            ui->RedTsnLanACheckBox->setChecked(false);
            ui->RedTsnLanBCheckBox->setChecked(false);
            ui->RedTsnRxFrameAValue->setText("NA");
            ui->RedTsnRxErrorAValue->setText("NA");
            ui->RedTsnTxFrameAValue->setText("NA");
            ui->RedTsnTxErrorAValue->setText("NA");
            ui->RedTsnRxFrameBValue->setText("NA");
            ui->RedTsnRxErrorBValue->setText("NA");
            ui->RedTsnTxFrameBValue->setText("NA");
            ui->RedTsnTxErrorBValue->setText("NA");
            ui->RedTsnRxFrameCValue->setText("NA");
            ui->RedTsnRxErrorCValue->setText("NA");
            ui->RedTsnTxFrameCValue->setText("NA");
            ui->RedTsnTxErrorCValue->setText("NA");
            ui->RedTsnClearCountersCheckBox->setChecked(false);

            ui->RedTsnPriorityEnableCheckBox->setChecked(false);
            ui->RedTsnPriorityEnableCheckBox->setEnabled(false);

            ui->RedTsnPhasePeriodValue->setText("NA");
            ui->RedTsnPhasePeriodValue->setEnabled(false);

            ui->RedTsnPhaseEnableCheckBox->setChecked(false);
            ui->RedTsnPhaseEnableCheckBox->setEnabled(false);

            ui->RedTsnCycleEnableCheckBox->setChecked(false);
            ui->RedTsnCycleEnableCheckBox->setEnabled(false);

            ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
            ui->RedTsnPreemptionEnableCheckBox->setEnabled(false);

            ui->RedTsnVlanPrio1Value->setCurrentIndex(1);
            ui->RedTsnVlanPrio1Value->setEnabled(false);
            ui->RedTsnPhaseStart1Value->setText("NA");
            ui->RedTsnPhaseStart1Value->setEnabled(false);
            ui->RedTsnPhaseEnd1Value->setText("NA");
            ui->RedTsnPhaseEnd1Value->setEnabled(false);
            ui->RedTsnCreditInc1Value->setText("NA");
            ui->RedTsnCreditInc1Value->setEnabled(false);
            ui->RedTsnCreditDec1Value->setText("NA");
            ui->RedTsnCreditDec1Value->setEnabled(false);
            ui->RedTsnMaxSize1Value->setText("NA");
            ui->RedTsnMaxSize1Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable1CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable1CheckBox->setChecked(false);
            ui->RedTsnCreditEnable1CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio2Value->setCurrentIndex(2);
            ui->RedTsnVlanPrio2Value->setEnabled(false);
            ui->RedTsnPhaseStart2Value->setText("NA");
            ui->RedTsnPhaseStart2Value->setEnabled(false);
            ui->RedTsnPhaseEnd2Value->setText("NA");
            ui->RedTsnPhaseEnd2Value->setEnabled(false);
            ui->RedTsnCreditInc2Value->setText("NA");
            ui->RedTsnCreditInc2Value->setEnabled(false);
            ui->RedTsnCreditDec2Value->setText("NA");
            ui->RedTsnCreditDec2Value->setEnabled(false);
            ui->RedTsnMaxSize2Value->setText("NA");
            ui->RedTsnMaxSize2Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable2CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable2CheckBox->setChecked(false);
            ui->RedTsnCreditEnable2CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio3Value->setCurrentIndex(3);
            ui->RedTsnVlanPrio3Value->setEnabled(false);
            ui->RedTsnPhaseStart3Value->setText("NA");
            ui->RedTsnPhaseStart3Value->setEnabled(false);
            ui->RedTsnPhaseEnd3Value->setText("NA");
            ui->RedTsnPhaseEnd3Value->setEnabled(false);
            ui->RedTsnCreditInc3Value->setText("NA");
            ui->RedTsnCreditInc3Value->setEnabled(false);
            ui->RedTsnCreditDec3Value->setText("NA");
            ui->RedTsnCreditDec3Value->setEnabled(false);
            ui->RedTsnMaxSize3Value->setText("NA");
            ui->RedTsnMaxSize3Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable3CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable3CheckBox->setChecked(false);
            ui->RedTsnCreditEnable3CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio4Value->setCurrentIndex(4);
            ui->RedTsnVlanPrio4Value->setEnabled(false);
            ui->RedTsnPhaseStart4Value->setText("NA");
            ui->RedTsnPhaseStart4Value->setEnabled(false);
            ui->RedTsnPhaseEnd4Value->setText("NA");
            ui->RedTsnPhaseEnd4Value->setEnabled(false);
            ui->RedTsnCreditInc4Value->setText("NA");
            ui->RedTsnCreditInc4Value->setEnabled(false);
            ui->RedTsnCreditDec4Value->setText("NA");
            ui->RedTsnCreditDec4Value->setEnabled(false);
            ui->RedTsnMaxSize4Value->setText("NA");
            ui->RedTsnMaxSize4Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable4CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable4CheckBox->setChecked(false);
            ui->RedTsnCreditEnable4CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio5Value->setCurrentIndex(5);
            ui->RedTsnVlanPrio5Value->setEnabled(false);
            ui->RedTsnPhaseStart5Value->setText("NA");
            ui->RedTsnPhaseStart5Value->setEnabled(false);
            ui->RedTsnPhaseEnd5Value->setText("NA");
            ui->RedTsnPhaseEnd5Value->setEnabled(false);
            ui->RedTsnCreditInc5Value->setText("NA");
            ui->RedTsnCreditInc5Value->setEnabled(false);
            ui->RedTsnCreditDec5Value->setText("NA");
            ui->RedTsnCreditDec5Value->setEnabled(false);
            ui->RedTsnMaxSize5Value->setText("NA");
            ui->RedTsnMaxSize5Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable5CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable5CheckBox->setChecked(false);
            ui->RedTsnCreditEnable5CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio6Value->setCurrentIndex(6);
            ui->RedTsnVlanPrio6Value->setEnabled(false);
            ui->RedTsnPhaseStart6Value->setText("NA");
            ui->RedTsnPhaseStart6Value->setEnabled(false);
            ui->RedTsnPhaseEnd6Value->setText("NA");
            ui->RedTsnPhaseEnd6Value->setEnabled(false);
            ui->RedTsnCreditInc6Value->setText("NA");
            ui->RedTsnCreditInc6Value->setEnabled(false);
            ui->RedTsnCreditDec6Value->setText("NA");
            ui->RedTsnCreditDec6Value->setEnabled(false);
            ui->RedTsnMaxSize6Value->setText("NA");
            ui->RedTsnMaxSize6Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable6CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable6CheckBox->setChecked(false);
            ui->RedTsnCreditEnable6CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio7Value->setCurrentIndex(7);
            ui->RedTsnVlanPrio7Value->setEnabled(false);
            ui->RedTsnPhaseStart7Value->setText("NA");
            ui->RedTsnPhaseStart7Value->setEnabled(false);
            ui->RedTsnPhaseEnd7Value->setText("NA");
            ui->RedTsnPhaseEnd7Value->setEnabled(false);
            ui->RedTsnCreditInc7Value->setText("NA");
            ui->RedTsnCreditInc7Value->setEnabled(false);
            ui->RedTsnCreditDec7Value->setText("NA");
            ui->RedTsnCreditDec7Value->setEnabled(false);
            ui->RedTsnMaxSize7Value->setText("NA");
            ui->RedTsnMaxSize7Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable7CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable7CheckBox->setChecked(false);
            ui->RedTsnCreditEnable7CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio8Value->setEnabled(false);
            ui->RedTsnPhaseStart8Value->setText("NA");
            ui->RedTsnPhaseStart8Value->setEnabled(false);
            ui->RedTsnPhaseEnd8Value->setText("NA");
            ui->RedTsnPhaseEnd8Value->setEnabled(false);
            ui->RedTsnCreditInc8Value->setText("NA");
            ui->RedTsnCreditInc8Value->setEnabled(false);
            ui->RedTsnCreditDec8Value->setText("NA");
            ui->RedTsnCreditDec8Value->setEnabled(false);
            ui->RedTsnMaxSize8Value->setText("NA");
            ui->RedTsnMaxSize8Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable8CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable8CheckBox->setChecked(false);
            ui->RedTsnCreditEnable8CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(false);

            ui->RedTsnCreditIncPortCValue->setText("NA");
            ui->RedTsnCreditIncPortCValue->setEnabled(false);
            ui->RedTsnCreditDecPortCValue->setText("NA");
            ui->RedTsnCreditDecPortCValue->setEnabled(false);
            ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
            ui->RedTsnCreditEnablePortCCheckBox->setEnabled(false);

            ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
            ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(false);
            ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
            ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
            ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
            ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
            ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(false);

            ui->RedTsnEnableCheckBox->setChecked(true);
            ui->RedTsnVersionValue->setText("NA");
            return;
        }
    }

    // enabled
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_ControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->RedTsnEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnEnableCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->RedTsnEnableCheckBox->setChecked(false);
    }


    // mac
    temp_string.clear();
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_Mac1_RegReg, temp_data))
    {
        temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 16) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        temp_string.append(QString("%1").arg(((temp_data >> 24) & 0x000000FF), 2, 16, QLatin1Char('0')));
        temp_string.append(":");
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_Mac2_RegReg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));
            ui->RedTsnMacValue->setText(temp_string);
        }
        else
        {
            ui->RedTsnMacValue->setText("NA");
        }

    }
    else
    {
        ui->RedTsnMacValue->setText("NA");
    }

    // vlan
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_ConfigVlanReg, temp_data))
    {
        if ((temp_data & 0x00010000) == 0)
        {
            ui->RedTsnVlanEnableCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnVlanEnableCheckBox->setChecked(true);
        }

        temp_data &= 0x0000FFFF;

        ui->RedTsnVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));
    }
    else
    {
        ui->RedTsnVlanEnableCheckBox->setChecked(false);
        ui->RedTsnVlanValue->setText("NA");
    }


    // mode
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_ConfigModeReg, temp_data))
    {
        switch (temp_data & 0x0000000F)
        {
        case 0:
            ui->RedTsnModeValue->setCurrentText("NO");
            break;
        case 1:
            ui->RedTsnModeValue->setCurrentText("PRP");
            break;
        case 2:
            ui->RedTsnModeValue->setCurrentText("HSR");
            break;
        case 3:
            ui->RedTsnModeValue->setCurrentText("TSN");
            break;
        default:
            ui->RedTsnModeValue->setCurrentText("NA");
            break;
        }

        if ((temp_data & 0x00010000) == 0)
        {
            ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnPromiscuousModeCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00020000) == 0)
        {
            ui->RedTsnNoForwardingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnNoForwardingCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00040000) == 0)
        {
            ui->RedTsnTailTaggingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnTailTaggingCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00080000) == 0)
        {
            ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnPrpUntaggingCheckBox->setChecked(true);
        }

    }
    else
    {
        ui->RedTsnModeValue->setCurrentText("NA");
        ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
        ui->RedTsnNoForwardingCheckBox->setChecked(false);
        ui->RedTsnTailTaggingCheckBox->setChecked(false);
        ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
    }


    // lan status
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_StatusReg, temp_data))
    {
        if ((temp_data & 0x00000101) == 0x00000100)
        {
            ui->RedTsnLanACheckBox->setChecked(true);
        }
        else
        {
            ui->RedTsnLanACheckBox->setChecked(false);
        }

        if ((temp_data & 0x00000202) == 0x00000200)
        {
            ui->RedTsnLanBCheckBox->setChecked(true);
        }
        else
        {
            ui->RedTsnLanBCheckBox->setChecked(false);
        }

        temp_data &= 0x00000003;
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_StatusReg, temp_data))
        {
            // nothing just cleared the values
        }
    }
    else
    {
        ui->RedTsnLanACheckBox->setChecked(false);
        ui->RedTsnLanBCheckBox->setChecked(false);
    }


    // phase period prio & phase & cycle & preemption
    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
        {
            if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplPeriodReg, temp_data))
            {
                ui->RedTsnPhasePeriodValue->setText(QString::number(temp_data));
            }
            else
            {
                ui->RedTsnPhasePeriodValue->setText("NA");
            }
        }
        else
        {
            ui->RedTsnPhasePeriodValue->setText("NA");
        }

        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioPhaseControlReg, temp_data))
        {
            if ((temp_data & 0x00000001) != 0)
            {
                ui->RedTsnPriorityEnableCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPriorityEnableCheckBox->setChecked(false);
            }

            if ((temp_data & 0x00000002) != 0)
            {
                ui->RedTsnPhaseEnableCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPhaseEnableCheckBox->setChecked(false);
            }

            if ((temp_data & 0x00000004) != 0)
            {
                ui->RedTsnCycleEnableCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCycleEnableCheckBox->setChecked(false);
            }

            if ((temp_data & 0x00000008) != 0)
            {
                ui->RedTsnPreemptionEnableCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
            }

            if (((temp_data & 0x00010000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00020000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00040000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00080000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00100000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00200000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00400000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00800000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
            }
        }
        else
        {
            ui->RedTsnPriorityEnableCheckBox->setChecked(false);
            ui->RedTsnPhaseEnableCheckBox->setChecked(false);
            ui->RedTsnCycleEnableCheckBox->setChecked(false);
            ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnPhasePeriodValue->setText("NA");
        ui->RedTsnPriorityEnableCheckBox->setChecked(false);
        ui->RedTsnPhaseEnableCheckBox->setChecked(false);
        ui->RedTsnCycleEnableCheckBox->setChecked(false);
        ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
        ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
    }


    // credit
    if (red_tsn_credit_support.at(instance_idx) == true)
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditControlReg, temp_data))
        {
            if ((temp_data & 0x00000001) != 0)
            {
                ui->RedTsnCreditEnableCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnableCheckBox->setChecked(false);
            }

            if (((temp_data & 0x00010000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui->RedTsnCreditEnable1CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable1CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00020000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui->RedTsnCreditEnable2CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable2CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00040000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui->RedTsnCreditEnable3CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable3CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00080000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui->RedTsnCreditEnable4CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable4CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00100000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui->RedTsnCreditEnable5CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable5CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00200000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui->RedTsnCreditEnable6CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable6CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00400000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui->RedTsnCreditEnable7CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable7CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00800000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui->RedTsnCreditEnable8CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnable8CheckBox->setChecked(false);
            }
        }
        else
        {
            ui->RedTsnCreditEnableCheckBox->setChecked(false);
            ui->RedTsnCreditEnable1CheckBox->setChecked(false);
            ui->RedTsnCreditEnable2CheckBox->setChecked(false);
            ui->RedTsnCreditEnable3CheckBox->setChecked(false);
            ui->RedTsnCreditEnable4CheckBox->setChecked(false);
            ui->RedTsnCreditEnable5CheckBox->setChecked(false);
            ui->RedTsnCreditEnable6CheckBox->setChecked(false);
            ui->RedTsnCreditEnable7CheckBox->setChecked(false);
            ui->RedTsnCreditEnable8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnCreditEnableCheckBox->setChecked(false);
        ui->RedTsnCreditEnable1CheckBox->setChecked(false);
        ui->RedTsnCreditEnable2CheckBox->setChecked(false);
        ui->RedTsnCreditEnable3CheckBox->setChecked(false);
        ui->RedTsnCreditEnable4CheckBox->setChecked(false);
        ui->RedTsnCreditEnable5CheckBox->setChecked(false);
        ui->RedTsnCreditEnable6CheckBox->setChecked(false);
        ui->RedTsnCreditEnable7CheckBox->setChecked(false);
        ui->RedTsnCreditEnable8CheckBox->setChecked(false);
    }


    // max size
    if (red_tsn_maxsize_support.at(instance_idx) == true)
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeControlReg, temp_data))
        {
            if ((temp_data & 0x00000001) != 0)
            {
                ui->RedTsnMaxSizeEnableCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
            }

            if (((temp_data & 0x00010000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui->RedTsnMaxSizeEnable1CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00020000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui->RedTsnMaxSizeEnable2CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00040000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui->RedTsnMaxSizeEnable3CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00080000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui->RedTsnMaxSizeEnable4CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00100000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui->RedTsnMaxSizeEnable5CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00200000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui->RedTsnMaxSizeEnable6CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00400000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui->RedTsnMaxSizeEnable7CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
            }

            if (((temp_data & 0x00800000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui->RedTsnMaxSizeEnable8CheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
            }
        }
        else
        {
            ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
    }


    // priority, phase start, phase end, credit inc, credit dec, max size
    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 1)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 1)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 0, temp_data))
                {
                    ui->RedTsnVlanPrio1Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 0, temp_data))
                {
                    ui->RedTsnPhaseStart1Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart1Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 0, temp_data))
                {
                    ui->RedTsnPhaseEnd1Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd1Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart1Value->setText("NA");
                ui->RedTsnPhaseEnd1Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 0, temp_data))
                {
                    ui->RedTsnCreditInc1Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec1Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc1Value->setText("NA");
                    ui->RedTsnCreditDec1Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc1Value->setText("NA");
                ui->RedTsnCreditDec1Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 0, temp_data))
                {
                    ui->RedTsnMaxSize1Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize1Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize1Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart1Value->setText("NA");
            ui->RedTsnPhaseEnd1Value->setText("NA");
            ui->RedTsnCreditInc1Value->setText("NA");
            ui->RedTsnCreditDec1Value->setText("NA");
            ui->RedTsnMaxSize1Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 2)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 2)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 4, temp_data))
                {
                    ui->RedTsnVlanPrio2Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 4, temp_data))
                {
                    ui->RedTsnPhaseStart2Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart2Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 4, temp_data))
                {
                    ui->RedTsnPhaseEnd2Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd2Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart2Value->setText("NA");
                ui->RedTsnPhaseEnd2Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 4, temp_data))
                {
                    ui->RedTsnCreditInc2Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec2Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc2Value->setText("NA");
                    ui->RedTsnCreditDec2Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc2Value->setText("NA");
                ui->RedTsnCreditDec2Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 4, temp_data))
                {
                    ui->RedTsnMaxSize2Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize2Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize2Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart2Value->setText("NA");
            ui->RedTsnPhaseEnd2Value->setText("NA");
            ui->RedTsnCreditInc2Value->setText("NA");
            ui->RedTsnCreditDec2Value->setText("NA");
            ui->RedTsnMaxSize2Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 3)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 3)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 8, temp_data))
                {
                    ui->RedTsnVlanPrio3Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 8, temp_data))
                {
                    ui->RedTsnPhaseStart3Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart3Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 8, temp_data))
                {
                    ui->RedTsnPhaseEnd3Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd3Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart3Value->setText("NA");
                ui->RedTsnPhaseEnd3Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 8, temp_data))
                {
                    ui->RedTsnCreditInc3Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec3Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc3Value->setText("NA");
                    ui->RedTsnCreditDec3Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc3Value->setText("NA");
                ui->RedTsnCreditDec3Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 8, temp_data))
                {
                    ui->RedTsnMaxSize3Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize3Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize3Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart3Value->setText("NA");
            ui->RedTsnPhaseEnd3Value->setText("NA");
            ui->RedTsnCreditInc3Value->setText("NA");
            ui->RedTsnCreditDec3Value->setText("NA");
            ui->RedTsnMaxSize3Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 4)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 4)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 12, temp_data))
                {
                    ui->RedTsnVlanPrio4Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 12, temp_data))
                {
                    ui->RedTsnPhaseStart4Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart4Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 12, temp_data))
                {
                    ui->RedTsnPhaseEnd4Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd4Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart4Value->setText("NA");
                ui->RedTsnPhaseEnd4Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 12, temp_data))
                {
                    ui->RedTsnCreditInc4Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec4Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc4Value->setText("NA");
                    ui->RedTsnCreditDec4Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc4Value->setText("NA");
                ui->RedTsnCreditDec4Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 12, temp_data))
                {
                    ui->RedTsnMaxSize4Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize4Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize4Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart4Value->setText("NA");
            ui->RedTsnPhaseEnd4Value->setText("NA");
            ui->RedTsnCreditInc4Value->setText("NA");
            ui->RedTsnCreditDec4Value->setText("NA");
            ui->RedTsnMaxSize4Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 5)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 5)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 16, temp_data))
                {
                    ui->RedTsnVlanPrio5Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 16, temp_data))
                {
                    ui->RedTsnPhaseStart5Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart5Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 16, temp_data))
                {
                    ui->RedTsnPhaseEnd5Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd5Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart5Value->setText("NA");
                ui->RedTsnPhaseEnd5Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 16, temp_data))
                {
                    ui->RedTsnCreditInc5Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec5Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc5Value->setText("NA");
                    ui->RedTsnCreditDec5Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc5Value->setText("NA");
                ui->RedTsnCreditDec5Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 16, temp_data))
                {
                    ui->RedTsnMaxSize5Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize5Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize5Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart5Value->setText("NA");
            ui->RedTsnPhaseEnd5Value->setText("NA");
            ui->RedTsnCreditInc5Value->setText("NA");
            ui->RedTsnCreditDec5Value->setText("NA");
            ui->RedTsnMaxSize5Value->setText("NA");
        }
    }
    
    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 6)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 6)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 20, temp_data))
                {
                    ui->RedTsnVlanPrio6Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 20, temp_data))
                {
                    ui->RedTsnPhaseStart6Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart6Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 20, temp_data))
                {
                    ui->RedTsnPhaseEnd6Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd6Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart6Value->setText("NA");
                ui->RedTsnPhaseEnd6Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 20, temp_data))
                {
                    ui->RedTsnCreditInc6Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec6Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc6Value->setText("NA");
                    ui->RedTsnCreditDec6Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc6Value->setText("NA");
                ui->RedTsnCreditDec6Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 20, temp_data))
                {
                    ui->RedTsnMaxSize6Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize6Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize6Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart6Value->setText("NA");
            ui->RedTsnPhaseEnd6Value->setText("NA");
            ui->RedTsnCreditInc6Value->setText("NA");
            ui->RedTsnCreditDec6Value->setText("NA");
            ui->RedTsnMaxSize6Value->setText("NA");
        }
    }
    
    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 7)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 7)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 24, temp_data))
                {
                    ui->RedTsnVlanPrio7Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 24, temp_data))
                {
                    ui->RedTsnPhaseStart7Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart7Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 24, temp_data))
                {
                    ui->RedTsnPhaseEnd7Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd7Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart7Value->setText("NA");
                ui->RedTsnPhaseEnd7Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 24, temp_data))
                {
                    ui->RedTsnCreditInc7Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec7Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc7Value->setText("NA");
                    ui->RedTsnCreditDec7Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc7Value->setText("NA");
                ui->RedTsnCreditDec7Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 24, temp_data))
                {
                    ui->RedTsnMaxSize7Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize7Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize7Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart7Value->setText("NA");
            ui->RedTsnPhaseEnd7Value->setText("NA");
            ui->RedTsnCreditInc7Value->setText("NA");
            ui->RedTsnCreditDec7Value->setText("NA");
            ui->RedTsnMaxSize7Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 8)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 8) // should never be the case
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 28, temp_data))
                {
                    ui->RedTsnVlanPrio8Value->setCurrentIndex(temp_data & 0x00000007);
                }
            }
            else
            {
                ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 28, temp_data))
                {
                    ui->RedTsnPhaseStart8Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseStart8Value->setText("NA");
                }
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 28, temp_data))
                {
                    ui->RedTsnPhaseEnd8Value->setText(QString::number(temp_data));
                }
                else
                {
                    ui->RedTsnPhaseEnd8Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnPhaseStart8Value->setText("NA");
                ui->RedTsnPhaseEnd8Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditReg + 28, temp_data))
                {
                    ui->RedTsnCreditInc8Value->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
                    ui->RedTsnCreditDec8Value->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
                }
                else
                {
                    ui->RedTsnCreditInc8Value->setText("NA");
                    ui->RedTsnCreditDec8Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnCreditInc8Value->setText("NA");
                ui->RedTsnCreditDec8Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 28, temp_data))
                {
                    ui->RedTsnMaxSize8Value->setText(QString::number(temp_data & 0x000007FF));
                }
                else
                {
                    ui->RedTsnMaxSize8Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize8Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart8Value->setText("NA");
            ui->RedTsnPhaseEnd8Value->setText("NA");
            ui->RedTsnCreditInc8Value->setText("NA");
            ui->RedTsnCreditDec8Value->setText("NA");
            ui->RedTsnMaxSize8Value->setText("NA");
        }
    }

    // advanced scheduling
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvCycleTimeReg, temp_data))
        {
            ui_scheduling->ui->RedTsnPhasePeriodValue->setText(QString::number(temp_data));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
        }
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvCycleTimeExtensionReg, temp_data))
        {
            ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText(QString::number(temp_data));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
        }
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvBaseTimeHReg, temp_data))
        {
            ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText(QString::number(temp_data));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
        }
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvBaseTimeLReg, temp_data))
        {
            ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText(QString::number(temp_data));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
        }
        
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListLengthReg, temp_data))
        {
            if (temp_data < 17)
            {
                ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(temp_data & 0x000000FF);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
            }
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
        }
    } 
    else
    {
        ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvGateStatesReg, temp_data))
        {
            if (((temp_data & 0x00000001) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000002) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000004) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000008) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000010) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000020) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000040) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
            }
            if (((temp_data & 0x00000080) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
            }
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 0, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 4, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 8, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 12, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 16, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 20, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 24, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 28, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 32, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 36, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 40, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 44, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 48, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 52, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 56, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 60, temp_data))
        {
            if (((temp_data & 0x01000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x02000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x04000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x08000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x10000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x20000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x40000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
            }
            if (((temp_data & 0x80000000) != 0) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(true);
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
            }

            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText(QString::number(temp_data & 0x00FFFFFF));
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
    }

    // port c
    if (red_tsn_credit_support.at(instance_idx) == true)
    {
        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditPortCReg, temp_data))
        {
            ui->RedTsnCreditIncPortCValue->setText(QString::number((temp_data >> 0) & 0x0000FFFF));
            ui->RedTsnCreditDecPortCValue->setText(QString::number((temp_data >> 16) & 0x0000FFFF));
        }
        else
        {
            ui->RedTsnCreditIncPortCValue->setText("NA");
            ui->RedTsnCreditDecPortCValue->setText("NA");
        }

        if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_CreditPortCControlReg, temp_data))
        {
            if ((temp_data & 0x00000001) != 0)
            {
                ui->RedTsnCreditEnablePortCCheckBox->setChecked(true);
            }
            else
            {
                ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
            }
        }
        else
        {
            ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);

    }

    // status counter
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_RxFrameCountAReg, temp_data))
    {
        ui->RedTsnRxFrameAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnRxFrameAValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_RxErrCountAReg, temp_data))
    {
        ui->RedTsnRxErrorAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnRxErrorAValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_TxFrameCountAReg, temp_data))
    {
        ui->RedTsnTxFrameAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnTxFrameAValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_TxErrCountAReg, temp_data))
    {
        ui->RedTsnTxErrorAValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnTxErrorAValue->setText("NA");
    }

    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_RxFrameCountBReg, temp_data))
    {
        ui->RedTsnRxFrameBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnRxFrameBValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_RxErrCountBReg, temp_data))
    {
        ui->RedTsnRxErrorBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnRxErrorBValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_TxFrameCountBReg, temp_data))
    {
        ui->RedTsnTxFrameBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnTxFrameBValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_TxErrCountBReg, temp_data))
    {
        ui->RedTsnTxErrorBValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnTxErrorBValue->setText("NA");
    }

    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_RxFrameCountCReg, temp_data))
    {
        ui->RedTsnRxFrameCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnRxFrameCValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_RxErrCountCReg, temp_data))
    {
        ui->RedTsnRxErrorCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnRxErrorCValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_TxFrameCountCReg, temp_data))
    {
        ui->RedTsnTxFrameCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnTxFrameCValue->setText("NA");
    }
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_TxErrCountCReg, temp_data))
    {
        ui->RedTsnTxErrorCValue->setText(QString::number(temp_data));
    }
    else
    {
        ui->RedTsnTxErrorCValue->setText("NA");
    }

    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_FrameCountControlReg, temp_data))
    {
        if ((temp_data & 0x00000001) == 0)
        {
            ui->RedTsnClearCountersCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnClearCountersCheckBox->setChecked(true);
        }
    }
    else
    {
        ui->RedTsnClearCountersCheckBox->setChecked(false);
    }

    // version
    if (0 == ucm->com_lib.read_reg(temp_addr + Ucm_RedTsn_VersionReg, temp_data))
    {
        ui->RedTsnVersionValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));

    }
    else
    {
        ui->RedTsnCreditIncPortCValue->setText("NA");
        ui->RedTsnCreditDecPortCValue->setText("NA");
        ui->RedTsnVersionValue->setText("NA");
    }
}

void Ucm_RedTsnTab::red_tsn_write_values(void)
{
    unsigned int instance_idx = 0;
    unsigned long long temp_mac;
    unsigned int temp_data = 0;
    unsigned int temp_addr = 0;
    QString temp_string;

    temp_string = ui->RedTsnInstanceComboBox->currentText();
    temp_data = temp_string.toUInt(nullptr, 10);
    instance_idx = temp_data -1;
    for (int i = 0; i < ucm->core_config.size(); i++)
    {
        if ((ucm->core_config.at(i).core_type == Ucm_CoreConfig_RedTsnCoreType) && (ucm->core_config.at(i).core_instance_nr == temp_data))
        {
            temp_addr = ucm->core_config.at(i).address_range_low;
            break;
        }
        else if (i == (ucm->core_config.size()-1))
        {
            ui->RedTsnMacValue->setText("NA");
            ui->RedTsnVlanEnableCheckBox->setChecked(false);
            ui->RedTsnVlanValue->setText("NA");
            ui->RedTsnLanACheckBox->setChecked(false);
            ui->RedTsnLanBCheckBox->setChecked(false);
            ui->RedTsnRxFrameAValue->setText("NA");
            ui->RedTsnRxErrorAValue->setText("NA");
            ui->RedTsnTxFrameAValue->setText("NA");
            ui->RedTsnTxErrorAValue->setText("NA");
            ui->RedTsnRxFrameBValue->setText("NA");
            ui->RedTsnRxErrorBValue->setText("NA");
            ui->RedTsnTxFrameBValue->setText("NA");
            ui->RedTsnTxErrorBValue->setText("NA");
            ui->RedTsnRxFrameCValue->setText("NA");
            ui->RedTsnRxErrorCValue->setText("NA");
            ui->RedTsnTxFrameCValue->setText("NA");
            ui->RedTsnTxErrorCValue->setText("NA");
            ui->RedTsnClearCountersCheckBox->setChecked(false);

            ui->RedTsnPriorityEnableCheckBox->setChecked(false);
            ui->RedTsnPriorityEnableCheckBox->setEnabled(false);

            ui->RedTsnPhasePeriodValue->setText("NA");
            ui->RedTsnPhasePeriodValue->setEnabled(false);

            ui->RedTsnPhaseEnableCheckBox->setChecked(false);
            ui->RedTsnPhaseEnableCheckBox->setEnabled(false);

            ui->RedTsnCycleEnableCheckBox->setChecked(false);
            ui->RedTsnCycleEnableCheckBox->setEnabled(false);

            ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
            ui->RedTsnPreemptionEnableCheckBox->setEnabled(false);

            ui->RedTsnVlanPrio1Value->setCurrentIndex(1);
            ui->RedTsnVlanPrio1Value->setEnabled(false);
            ui->RedTsnPhaseStart1Value->setText("NA");
            ui->RedTsnPhaseStart1Value->setEnabled(false);
            ui->RedTsnPhaseEnd1Value->setText("NA");
            ui->RedTsnPhaseEnd1Value->setEnabled(false);
            ui->RedTsnCreditInc1Value->setText("NA");
            ui->RedTsnCreditInc1Value->setEnabled(false);
            ui->RedTsnCreditDec1Value->setText("NA");
            ui->RedTsnCreditDec1Value->setEnabled(false);
            ui->RedTsnMaxSize1Value->setText("NA");
            ui->RedTsnMaxSize1Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable1CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable1CheckBox->setChecked(false);
            ui->RedTsnCreditEnable1CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable1CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio2Value->setCurrentIndex(2);
            ui->RedTsnVlanPrio2Value->setEnabled(false);
            ui->RedTsnPhaseStart2Value->setText("NA");
            ui->RedTsnPhaseStart2Value->setEnabled(false);
            ui->RedTsnPhaseEnd2Value->setText("NA");
            ui->RedTsnPhaseEnd2Value->setEnabled(false);
            ui->RedTsnCreditInc2Value->setText("NA");
            ui->RedTsnCreditInc2Value->setEnabled(false);
            ui->RedTsnCreditDec2Value->setText("NA");
            ui->RedTsnCreditDec2Value->setEnabled(false);
            ui->RedTsnMaxSize2Value->setText("NA");
            ui->RedTsnMaxSize2Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable2CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable2CheckBox->setChecked(false);
            ui->RedTsnCreditEnable2CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable2CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio3Value->setCurrentIndex(3);
            ui->RedTsnVlanPrio3Value->setEnabled(false);
            ui->RedTsnPhaseStart3Value->setText("NA");
            ui->RedTsnPhaseStart3Value->setEnabled(false);
            ui->RedTsnPhaseEnd3Value->setText("NA");
            ui->RedTsnPhaseEnd3Value->setEnabled(false);
            ui->RedTsnCreditInc3Value->setText("NA");
            ui->RedTsnCreditInc3Value->setEnabled(false);
            ui->RedTsnCreditDec3Value->setText("NA");
            ui->RedTsnCreditDec3Value->setEnabled(false);
            ui->RedTsnMaxSize3Value->setText("NA");
            ui->RedTsnMaxSize3Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable3CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable3CheckBox->setChecked(false);
            ui->RedTsnCreditEnable3CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable3CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio4Value->setCurrentIndex(4);
            ui->RedTsnVlanPrio4Value->setEnabled(false);
            ui->RedTsnPhaseStart4Value->setText("NA");
            ui->RedTsnPhaseStart4Value->setEnabled(false);
            ui->RedTsnPhaseEnd4Value->setText("NA");
            ui->RedTsnPhaseEnd4Value->setEnabled(false);
            ui->RedTsnCreditInc4Value->setText("NA");
            ui->RedTsnCreditInc4Value->setEnabled(false);
            ui->RedTsnCreditDec4Value->setText("NA");
            ui->RedTsnCreditDec4Value->setEnabled(false);
            ui->RedTsnMaxSize4Value->setText("NA");
            ui->RedTsnMaxSize4Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable4CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable4CheckBox->setChecked(false);
            ui->RedTsnCreditEnable4CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable4CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio5Value->setCurrentIndex(5);
            ui->RedTsnVlanPrio5Value->setEnabled(false);
            ui->RedTsnPhaseStart5Value->setText("NA");
            ui->RedTsnPhaseStart5Value->setEnabled(false);
            ui->RedTsnPhaseEnd5Value->setText("NA");
            ui->RedTsnPhaseEnd5Value->setEnabled(false);
            ui->RedTsnCreditInc5Value->setText("NA");
            ui->RedTsnCreditInc5Value->setEnabled(false);
            ui->RedTsnCreditDec5Value->setText("NA");
            ui->RedTsnCreditDec5Value->setEnabled(false);
            ui->RedTsnMaxSize5Value->setText("NA");
            ui->RedTsnMaxSize5Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable5CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable5CheckBox->setChecked(false);
            ui->RedTsnCreditEnable5CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable5CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio6Value->setCurrentIndex(6);
            ui->RedTsnVlanPrio6Value->setEnabled(false);
            ui->RedTsnPhaseStart6Value->setText("NA");
            ui->RedTsnPhaseStart6Value->setEnabled(false);
            ui->RedTsnPhaseEnd6Value->setText("NA");
            ui->RedTsnPhaseEnd6Value->setEnabled(false);
            ui->RedTsnCreditInc6Value->setText("NA");
            ui->RedTsnCreditInc6Value->setEnabled(false);
            ui->RedTsnCreditDec6Value->setText("NA");
            ui->RedTsnCreditDec6Value->setEnabled(false);
            ui->RedTsnMaxSize6Value->setText("NA");
            ui->RedTsnMaxSize6Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable6CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable6CheckBox->setChecked(false);
            ui->RedTsnCreditEnable6CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable6CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio7Value->setCurrentIndex(7);
            ui->RedTsnVlanPrio7Value->setEnabled(false);
            ui->RedTsnPhaseStart7Value->setText("NA");
            ui->RedTsnPhaseStart7Value->setEnabled(false);
            ui->RedTsnPhaseEnd7Value->setText("NA");
            ui->RedTsnPhaseEnd7Value->setEnabled(false);
            ui->RedTsnCreditInc7Value->setText("NA");
            ui->RedTsnCreditInc7Value->setEnabled(false);
            ui->RedTsnCreditDec7Value->setText("NA");
            ui->RedTsnCreditDec7Value->setEnabled(false);
            ui->RedTsnMaxSize7Value->setText("NA");
            ui->RedTsnMaxSize7Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable7CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable7CheckBox->setChecked(false);
            ui->RedTsnCreditEnable7CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable7CheckBox->setEnabled(false);

            ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            ui->RedTsnVlanPrio8Value->setEnabled(false);
            ui->RedTsnPhaseStart8Value->setText("NA");
            ui->RedTsnPhaseStart8Value->setEnabled(false);
            ui->RedTsnPhaseEnd8Value->setText("NA");
            ui->RedTsnPhaseEnd8Value->setEnabled(false);
            ui->RedTsnCreditInc8Value->setText("NA");
            ui->RedTsnCreditInc8Value->setEnabled(false);
            ui->RedTsnCreditDec8Value->setText("NA");
            ui->RedTsnCreditDec8Value->setEnabled(false);
            ui->RedTsnMaxSize8Value->setText("NA");
            ui->RedTsnMaxSize8Value->setEnabled(false);
            ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable8CheckBox->setEnabled(false);
            ui->RedTsnCreditEnable8CheckBox->setChecked(false);
            ui->RedTsnCreditEnable8CheckBox->setEnabled(false);
            ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable8CheckBox->setEnabled(false);

            ui->RedTsnCreditIncPortCValue->setText("NA");
            ui->RedTsnCreditIncPortCValue->setEnabled(false);
            ui->RedTsnCreditDecPortCValue->setText("NA");
            ui->RedTsnCreditDecPortCValue->setEnabled(false);
            ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
            ui->RedTsnCreditEnablePortCCheckBox->setEnabled(false);

            ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
            ui_scheduling->ui->RedTsnPhasePeriodValue->setEnabled(false);
            ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
            ui_scheduling->ui->RedTsnPhasePeriodExtValue->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
            ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
            ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
            ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setEnabled(false);

            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setEnabled(false);
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
            ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setEnabled(false);

            ui->RedTsnModeValue->setCurrentText("NA");
            ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
            ui->RedTsnNoForwardingCheckBox->setChecked(false);
            ui->RedTsnTailTaggingCheckBox->setChecked(false);
            ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
            ui->RedTsnVersionValue->setText("NA");
            return;
        }
    }


    // mac
    temp_string = ui->RedTsnMacValue->text();
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

    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_Mac1_RegReg, temp_data))
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
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_Mac2_RegReg, temp_data))
        {
            temp_string.append(QString("%1").arg(((temp_data >> 0) & 0x000000FF), 2, 16, QLatin1Char('0')));
            temp_string.append(":");
            temp_string.append(QString("%1").arg(((temp_data >> 8) & 0x000000FF), 2, 16, QLatin1Char('0')));

            temp_data = 0x00000001; // write
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MacControlReg, temp_data))
            {
                ui->RedTsnMacValue->setText(temp_string);
            }
            else
            {
                ui->RedTsnMacValue->setText("NA");
            }
        }
        else
        {
            ui->RedTsnMacValue->setText("NA");
        }

    }
    else
    {
        ui->RedTsnMacValue->setText("NA");
    }

    // vlan
    temp_string = ui->RedTsnVlanValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);
    temp_data &= 0x0000FFFF;
    if (true == ui->RedTsnVlanEnableCheckBox->isChecked())
    {
        temp_data |= 0x00010000; // enable
    }
    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_ConfigVlanReg, temp_data))
    {
        temp_data &= 0x0000FFFF;
        ui->RedTsnVlanValue->setText(QString("0x%1").arg(temp_data, 4, 16, QLatin1Char('0')));

        temp_data = 0x00000002; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedTsnVlanEnableCheckBox->setChecked(false);
            ui->RedTsnVlanValue->setText("NA");
        }
    }
    else
    {
        ui->RedTsnVlanEnableCheckBox->setChecked(false);
        ui->RedTsnVlanValue->setText("NA");
    }

    // mode
    temp_string = ui->RedTsnModeValue->currentText();
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
    else if (temp_string == "TSN")
    {
        temp_data = 0x00000003;
    }
    else
    {
        temp_data = 0x00000000;
    }

    if (true == ui->RedTsnPromiscuousModeCheckBox->isChecked())
    {
        temp_data |= 0x00010000;
    }

    if (true == ui->RedTsnNoForwardingCheckBox->isChecked())
    {
        temp_data |= 0x00020000;
    }

    if (true == ui->RedTsnTailTaggingCheckBox->isChecked())
    {
        temp_data |= 0x00040000;
    }

    if (true == ui->RedTsnPrpUntaggingCheckBox->isChecked())
    {
        temp_data |= 0x00080000;
    }

    if (temp_string == "NA")
    {
        //nothing
    }
    else if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_ConfigModeReg, temp_data))
    {
        switch (temp_data & 0x0000000F)
        {
        case 0:
            ui->RedTsnModeValue->setCurrentText("NO");
            break;
        case 1:
            ui->RedTsnModeValue->setCurrentText("PRP");
            break;
        case 2:
            ui->RedTsnModeValue->setCurrentText("HSR");
            break;
        default:
            ui->RedTsnModeValue->setCurrentText("NA");
            break;
        }

        if ((temp_data & 0x00010000) == 0)
        {
            ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnPromiscuousModeCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00020000) == 0)
        {
            ui->RedTsnNoForwardingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnNoForwardingCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00040000) == 0)
        {
            ui->RedTsnTailTaggingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnTailTaggingCheckBox->setChecked(true);
        }

        if ((temp_data & 0x00080000) == 0)
        {
            ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
        }
        else
        {
            ui->RedTsnPrpUntaggingCheckBox->setChecked(true);
        }

        temp_data = 0x00000001; // write
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_ConfigControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedTsnModeValue->setCurrentText("NA");
            ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
            ui->RedTsnNoForwardingCheckBox->setChecked(false);
            ui->RedTsnTailTaggingCheckBox->setChecked(false);
            ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnModeValue->setCurrentText("NA");
        ui->RedTsnPromiscuousModeCheckBox->setChecked(false);
        ui->RedTsnNoForwardingCheckBox->setChecked(false);
        ui->RedTsnTailTaggingCheckBox->setChecked(false);
        ui->RedTsnPrpUntaggingCheckBox->setChecked(false);
    }

    // credit
    if (red_tsn_credit_support.at(instance_idx) == true)
    {
        temp_data = 0x00000000; // nothing
        if (true == ui->RedTsnCreditEnableCheckBox->isChecked())
        {
            temp_data |= 0x00000001; // enable
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 1)
        {
            if (true == ui->RedTsnCreditEnable1CheckBox->isChecked())
            {
                temp_data |= 0x00010000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 2)
        {
            if (true == ui->RedTsnCreditEnable2CheckBox->isChecked())
            {
                temp_data |= 0x00020000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 3)
        {
            if (true == ui->RedTsnCreditEnable3CheckBox->isChecked())
            {
                temp_data |= 0x00040000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 4)
        {
            if (true == ui->RedTsnCreditEnable4CheckBox->isChecked())
            {
                temp_data |= 0x00080000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 5)
        {
            if (true == ui->RedTsnCreditEnable5CheckBox->isChecked())
            {
                temp_data |= 0x00100000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 6)
        {
            if (true == ui->RedTsnCreditEnable6CheckBox->isChecked())
            {
                temp_data |= 0x00200000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 7)
        {
            if (true == ui->RedTsnCreditEnable7CheckBox->isChecked())
            {
                temp_data |= 0x00400000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 8)
        {
            if (true == ui->RedTsnCreditEnable8CheckBox->isChecked())
            {
                temp_data |= 0x00800000; // enable
            }
        }
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedTsnCreditEnableCheckBox->setChecked(false);
            ui->RedTsnCreditEnable1CheckBox->setChecked(false);
            ui->RedTsnCreditEnable2CheckBox->setChecked(false);
            ui->RedTsnCreditEnable3CheckBox->setChecked(false);
            ui->RedTsnCreditEnable4CheckBox->setChecked(false);
            ui->RedTsnCreditEnable5CheckBox->setChecked(false);
            ui->RedTsnCreditEnable6CheckBox->setChecked(false);
            ui->RedTsnCreditEnable7CheckBox->setChecked(false);
            ui->RedTsnCreditEnable8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnCreditEnableCheckBox->setChecked(false);
        ui->RedTsnCreditEnable1CheckBox->setChecked(false);
        ui->RedTsnCreditEnable2CheckBox->setChecked(false);
        ui->RedTsnCreditEnable3CheckBox->setChecked(false);
        ui->RedTsnCreditEnable4CheckBox->setChecked(false);
        ui->RedTsnCreditEnable5CheckBox->setChecked(false);
        ui->RedTsnCreditEnable6CheckBox->setChecked(false);
        ui->RedTsnCreditEnable7CheckBox->setChecked(false);
        ui->RedTsnCreditEnable8CheckBox->setChecked(false);
    }

    // max size
    if (red_tsn_maxsize_support.at(instance_idx) == true)
    {
        temp_data = 0x00000000; // nothing
        if (true == ui->RedTsnMaxSizeEnableCheckBox->isChecked())
        {
            temp_data |= 0x00000001; // enable
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 1)
        {
            if (true == ui->RedTsnMaxSizeEnable1CheckBox->isChecked())
            {
                temp_data |= 0x00010000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 2)
        {
            if (true == ui->RedTsnMaxSizeEnable2CheckBox->isChecked())
            {
                temp_data |= 0x00020000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 3)
        {
            if (true == ui->RedTsnMaxSizeEnable3CheckBox->isChecked())
            {
                temp_data |= 0x00040000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 4)
        {
            if (true == ui->RedTsnMaxSizeEnable4CheckBox->isChecked())
            {
                temp_data |= 0x00080000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 5)
        {
            if (true == ui->RedTsnMaxSizeEnable5CheckBox->isChecked())
            {
                temp_data |= 0x00100000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 6)
        {
            if (true == ui->RedTsnMaxSizeEnable6CheckBox->isChecked())
            {
                temp_data |= 0x00200000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 7)
        {
            if (true == ui->RedTsnMaxSizeEnable7CheckBox->isChecked())
            {
                temp_data |= 0x00400000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 8)
        {
            if (true == ui->RedTsnMaxSizeEnable8CheckBox->isChecked())
            {
                temp_data |= 0x00800000; // enable
            }
        }
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
            ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnMaxSizeEnableCheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable1CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable2CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable3CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable4CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable5CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable6CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable7CheckBox->setChecked(false);
        ui->RedTsnMaxSizeEnable8CheckBox->setChecked(false);
    }

    // priority, phase start, phase end, credit inc, credit dec, max size
    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 1)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 1)
            {
                temp_string = ui->RedTsnVlanPrio1Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 0, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart1Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 0, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart1Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd1Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 0, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd1Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart1Value->setText("NA");
                ui->RedTsnPhaseEnd1Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec1Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc1Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 0, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc1Value->setText("NA");
                            ui->RedTsnCreditDec1Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc1Value->setText("NA");
                ui->RedTsnCreditDec1Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize1Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 0, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize1Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize1Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio1Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart1Value->setText("NA");
            ui->RedTsnPhaseEnd1Value->setText("NA");
            ui->RedTsnCreditInc1Value->setText("NA");
            ui->RedTsnCreditDec1Value->setText("NA");
            ui->RedTsnMaxSize1Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 2)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 2)
            {
                temp_string = ui->RedTsnVlanPrio2Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 4, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart2Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 4, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart2Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd2Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 4, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd2Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart2Value->setText("NA");
                ui->RedTsnPhaseEnd2Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec2Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc2Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 4, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc2Value->setText("NA");
                            ui->RedTsnCreditDec2Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc2Value->setText("NA");
                ui->RedTsnCreditDec2Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize2Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 4, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize2Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize2Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio2Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart2Value->setText("NA");
            ui->RedTsnPhaseEnd2Value->setText("NA");
            ui->RedTsnCreditInc2Value->setText("NA");
            ui->RedTsnCreditDec2Value->setText("NA");
            ui->RedTsnMaxSize2Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 3)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 3)
            {
                temp_string = ui->RedTsnVlanPrio3Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 8, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart3Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 8, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart3Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd3Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 8, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd3Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart3Value->setText("NA");
                ui->RedTsnPhaseEnd3Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec3Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc3Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 8, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc3Value->setText("NA");
                            ui->RedTsnCreditDec3Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc3Value->setText("NA");
                ui->RedTsnCreditDec3Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize3Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 8, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize3Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize3Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio3Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart3Value->setText("NA");
            ui->RedTsnPhaseEnd3Value->setText("NA");
            ui->RedTsnCreditInc3Value->setText("NA");
            ui->RedTsnCreditDec3Value->setText("NA");
            ui->RedTsnMaxSize3Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 4)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 4)
            {
                temp_string = ui->RedTsnVlanPrio4Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 12, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart4Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 12, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart4Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd4Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 12, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd4Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart4Value->setText("NA");
                ui->RedTsnPhaseEnd4Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec4Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc4Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 12, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc4Value->setText("NA");
                            ui->RedTsnCreditDec4Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc4Value->setText("NA");
                ui->RedTsnCreditDec4Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize4Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 12, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize4Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize4Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio4Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart4Value->setText("NA");
            ui->RedTsnPhaseEnd4Value->setText("NA");
            ui->RedTsnCreditInc4Value->setText("NA");
            ui->RedTsnCreditDec4Value->setText("NA");
            ui->RedTsnMaxSize4Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 5)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 5)
            {
                temp_string = ui->RedTsnVlanPrio5Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 16, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart5Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 16, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart5Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd5Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 16, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd5Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart5Value->setText("NA");
                ui->RedTsnPhaseEnd5Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec5Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc5Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 16, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc5Value->setText("NA");
                            ui->RedTsnCreditDec5Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc5Value->setText("NA");
                ui->RedTsnCreditDec5Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize5Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 16, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize5Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize5Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio5Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart5Value->setText("NA");
            ui->RedTsnPhaseEnd5Value->setText("NA");
            ui->RedTsnCreditInc5Value->setText("NA");
            ui->RedTsnCreditDec5Value->setText("NA");
            ui->RedTsnMaxSize5Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 6)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 6)
            {
                temp_string = ui->RedTsnVlanPrio6Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 20, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart6Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 20, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart6Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd6Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 20, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd6Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart6Value->setText("NA");
                ui->RedTsnPhaseEnd6Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec6Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc6Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 20, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc6Value->setText("NA");
                            ui->RedTsnCreditDec6Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc6Value->setText("NA");
                ui->RedTsnCreditDec6Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize6Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 20, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize6Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize6Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio6Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart6Value->setText("NA");
            ui->RedTsnPhaseEnd6Value->setText("NA");
            ui->RedTsnCreditInc6Value->setText("NA");
            ui->RedTsnCreditDec6Value->setText("NA");
            ui->RedTsnMaxSize6Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 7)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 7)
            {
                temp_string = ui->RedTsnVlanPrio7Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 24, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart7Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 24, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart7Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd7Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 24, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd7Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart7Value->setText("NA");
                ui->RedTsnPhaseEnd7Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec7Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc7Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 24, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc7Value->setText("NA");
                            ui->RedTsnCreditDec7Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc7Value->setText("NA");
                ui->RedTsnCreditDec7Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize7Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 24, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize7Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize7Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio7Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart7Value->setText("NA");
            ui->RedTsnPhaseEnd7Value->setText("NA");
            ui->RedTsnCreditInc7Value->setText("NA");
            ui->RedTsnCreditDec7Value->setText("NA");
            ui->RedTsnMaxSize7Value->setText("NA");
        }
    }

    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 8)
        {
            if (red_tsn_nr_of_priorities.at(instance_idx) > 8)
            {
                temp_string = ui->RedTsnVlanPrio8Value->currentText();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioVlanReg + 28, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
                }
            }
            else
            {
                ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            }

            if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
            {
                temp_string = ui->RedTsnPhaseStart8Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStartReg + 28, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseStart8Value->setText("NA");
                    }
                }

                temp_string = ui->RedTsnPhaseEnd8Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplStopReg + 28, temp_data))
                    {
                        // nothing
                    }
                    else
                    {
                        ui->RedTsnPhaseEnd8Value->setText("NA");
                    }
                }
            }
            else
            {
                ui->RedTsnPhaseStart8Value->setText("NA");
                ui->RedTsnPhaseEnd8Value->setText("NA");
            }

            if (red_tsn_credit_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnCreditDec8Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                temp_data = (temp_data & 0x0000FFFF) << 16;
                if (temp_string == "NA")
                {
                    //nothing
                }
                else
                {
                    temp_string = ui->RedTsnCreditInc8Value->text();
                    if (temp_string == "NA")
                    {
                        //nothing
                    }
                    else
                    {
                        temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditReg + 28, temp_data))
                        {
                            // nothing
                        }
                        else
                        {
                            ui->RedTsnCreditInc8Value->setText("NA");
                            ui->RedTsnCreditDec8Value->setText("NA");
                        }
                    }
                }
            }
            else
            {
                ui->RedTsnCreditInc8Value->setText("NA");
                ui->RedTsnCreditDec8Value->setText("NA");
            }

            if (red_tsn_maxsize_support.at(instance_idx) == true)
            {
                temp_string = ui->RedTsnMaxSize8Value->text();
                temp_data = temp_string.toUInt(nullptr, 10);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_MaxDataSizeReg + 28, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnMaxSize8Value->setText("NA");
                }
            }
            else
            {
                ui->RedTsnMaxSize8Value->setText("NA");
            }
        }
        else
        {
            ui->RedTsnVlanPrio8Value->setCurrentIndex(0);
            ui->RedTsnPhaseStart8Value->setText("NA");
            ui->RedTsnPhaseEnd8Value->setText("NA");
            ui->RedTsnCreditInc8Value->setText("NA");
            ui->RedTsnCreditDec8Value->setText("NA");
            ui->RedTsnMaxSize8Value->setText("NA");
        }
    }

    // advanced scheduling
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhasePeriodValue->text();
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvCycleTimeReg, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
            }
        }
        temp_string = ui_scheduling->ui->RedTsnPhasePeriodExtValue->text();
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvCycleTimeExtensionReg, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
            }
        }
        temp_string = ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->text();
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvBaseTimeHReg, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
            }
        }
        temp_string = ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->text();
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvBaseTimeLReg, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
            }
        }
        temp_string = ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->currentText();
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            if (temp_data >= 17)
            {
                temp_data = 16;
            }
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListLengthReg, temp_data))
            {
                //nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
            }
        }
    } 
    else
    {
        ui_scheduling->ui->RedTsnPhasePeriodValue->setText("NA");
        ui_scheduling->ui->RedTsnPhasePeriodExtValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseSecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseBaseNanosecondsValue->setText("NA");
        ui_scheduling->ui->RedTsnPhaseNrOfEntriesValue->setCurrentIndex(17);
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_data = 0;

        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
        {
            temp_data |= 0x00000001;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
        {
            temp_data |= 0x00000002;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
        {
            temp_data |= 0x00000004;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
        {
            temp_data |= 0x00000008;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
        {
            temp_data |= 0x00000010;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
        {
            temp_data |= 0x00000020;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
        {
            temp_data |= 0x00000040;
        }
        if ((true == ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
        {
            temp_data |= 0x00000080;
        }

        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvGateStatesReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
            ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseInitialP1CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP2CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP3CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP4CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP5CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP6CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP7CheckBox->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseInitialP8CheckBox->setChecked(false);
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }

            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 0, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_0->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_0->setText("NA");
    }

    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 4, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_1->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_1->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 8, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_2->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_2->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 12, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_3->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_3->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 16, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_4->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_4->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 20, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_5->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_5->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 24, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_6->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_6->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 28, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_7->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_7->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 32, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_8->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_8->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 36, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_9->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_9->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 40, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_10->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_10->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 44, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_11->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_11->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 48, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_12->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_12->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 52, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_13->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_13->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 56, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_14->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_14->setText("NA");
    }
    
    if ((red_tsn_priority_support.at(instance_idx) == true) && (red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == false))
    {
        temp_string = ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->text();
        if (temp_string == "NA")
        {
            temp_data = 0;
        }
        else
        {
            temp_data = temp_string.toUInt(nullptr, 10);
            temp_data &= 0x00FFFFFF;

            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 1))
            {
                temp_data |= 0x01000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 2))
            {
                temp_data |= 0x02000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 3))
            {
                temp_data |= 0x04000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 4))
            {
                temp_data |= 0x08000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 5))
            {
                temp_data |= 0x10000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 6))
            {
                temp_data |= 0x20000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 7))
            {
                temp_data |= 0x40000000;
            }
            if ((true == ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->isChecked()) && (red_tsn_nr_of_priorities.at(instance_idx) >= 8))
            {
                temp_data |= 0x80000000;
            }
            
            if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseAdvControlListEntryReg + 60, temp_data))
            {
                // nothing
            }
            else
            {
                ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
                ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
            }
        }
    }
    else
    {
        ui_scheduling->ui->RedTsnPhaseEntryP1CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP2CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP3CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP4CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP5CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP6CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP7CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryP8CheckBox_15->setChecked(false);
        ui_scheduling->ui->RedTsnPhaseEntryDurationValue_15->setText("NA");
    }
    
    // phase period prio & phase & cycle
    if (red_tsn_priority_support.at(instance_idx) == true)
    {
        if ((red_tsn_phase_support.at(instance_idx) == true) && (red_tsn_simple_scheduler_support.at(instance_idx) == true))
        {
            temp_string = ui->RedTsnPhasePeriodValue->text();
            temp_data = temp_string.toUInt(nullptr, 10);
            if (temp_string == "NA")
            {
                //nothing
            }
            else
            {
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PhaseSplPeriodReg, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnPhasePeriodValue->setText("NA");
                }
            }
        }
        else
        {
            ui->RedTsnPhasePeriodValue->setText("NA");
        }

        temp_data = 0x00000000; // nothing
        if (red_tsn_phase_support.at(instance_idx) == true)
        {
            temp_data |= 0x00000100; // enable
        }
        if (true == ui->RedTsnPriorityEnableCheckBox->isChecked())
        {
            temp_data |= 0x00000001; // enable
        }
        if (red_tsn_phase_support.at(instance_idx) == true)
        {
            if (true == ui->RedTsnPhaseEnableCheckBox->isChecked())
            {
                temp_data |= 0x00000002; // enable
            }
        }
        if (red_tsn_cycle_support.at(instance_idx) == true)
        {
            if (true == ui->RedTsnCycleEnableCheckBox->isChecked())
            {
                temp_data |= 0x00000004; // enable
            }
        }
        if (red_tsn_preemption_support.at(instance_idx) == true)
        {
            if (true == ui->RedTsnPreemptionEnableCheckBox->isChecked())
            {
                temp_data |= 0x00000008; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 1)
        {
            if (true == ui->RedTsnPrioPhaseEnable1CheckBox->isChecked())
            {
                temp_data |= 0x00010000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 2)
        {
            if (true == ui->RedTsnPrioPhaseEnable2CheckBox->isChecked())
            {
                temp_data |= 0x00020000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 3)
        {
            if (true == ui->RedTsnPrioPhaseEnable3CheckBox->isChecked())
            {
                temp_data |= 0x00040000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 4)
        {
            if (true == ui->RedTsnPrioPhaseEnable4CheckBox->isChecked())
            {
                temp_data |= 0x00080000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 5)
        {
            if (true == ui->RedTsnPrioPhaseEnable5CheckBox->isChecked())
            {
                temp_data |= 0x00100000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 6)
        {
            if (true == ui->RedTsnPrioPhaseEnable6CheckBox->isChecked())
            {
                temp_data |= 0x00200000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 7)
        {
            if (true == ui->RedTsnPrioPhaseEnable7CheckBox->isChecked())
            {
                temp_data |= 0x00400000; // enable
            }
        }
        if (red_tsn_nr_of_priorities.at(instance_idx) >= 8)
        {
            if (true == ui->RedTsnPrioPhaseEnable8CheckBox->isChecked())
            {
                temp_data |= 0x00800000; // enable
            }
        }
        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_PrioPhaseControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedTsnPriorityEnableCheckBox->setChecked(false);
            ui->RedTsnPhaseEnableCheckBox->setChecked(false);
            ui->RedTsnCycleEnableCheckBox->setChecked(false);
            ui->RedTsnPreemptionEnableCheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable1CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable2CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable3CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable4CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable5CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable6CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable7CheckBox->setChecked(false);
            ui->RedTsnPrioPhaseEnable8CheckBox->setChecked(false);
        }
    }

    // port c
    if (red_tsn_credit_support.at(instance_idx) == true)
    {
        temp_string = ui->RedTsnCreditDecPortCValue->text();
        temp_data = temp_string.toUInt(nullptr, 10);
        temp_data = (temp_data & 0x0000FFFF) << 16;
        if (temp_string == "NA")
        {
            //nothing
        }
        else
        {
            temp_string = ui->RedTsnCreditIncPortCValue->text();
            if (temp_string == "NA")
            {
                //nothing
            }
            else
            {
                temp_data |= (temp_string.toUInt(nullptr, 10) & 0x0000FFFF);
                if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditPortCReg, temp_data))
                {
                    // nothing
                }
                else
                {
                    ui->RedTsnCreditIncPortCValue->setText("NA");
                    ui->RedTsnCreditDecPortCValue->setText("NA");
                }
            }
        }

        temp_data = 0x00000000; // nothing
        if (true == ui->RedTsnCreditEnablePortCCheckBox->isChecked())
        {
            temp_data |= 0x00000001; // enable
        }

        if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_CreditPortCControlReg, temp_data))
        {
            // nothing
        }
        else
        {
            ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
        }
    }
    else
    {
        ui->RedTsnCreditIncPortCValue->setText("NA");
        ui->RedTsnCreditDecPortCValue->setText("NA");
        ui->RedTsnCreditEnablePortCCheckBox->setChecked(false);
    }

    // status counter
    temp_data = 0x00000000; // nothing
    if (true == ui->RedTsnClearCountersCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // clear
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_FrameCountControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->RedTsnClearCountersCheckBox->setChecked(false);
    }

    temp_data = 0x00000000; // nothing
    if (true == ui->RedTsnEnableCheckBox->isChecked())
    {
        temp_data |= 0x00000001; // enable
    }
    if (0 == ucm->com_lib.write_reg(temp_addr + Ucm_RedTsn_ControlReg, temp_data))
    {
        // nothing
    }
    else
    {
        ui->RedTsnEnableCheckBox->setChecked(false);
    }
}

void Ucm_RedTsnTab::red_tsn_instance_changed(int index)
{
    if ((ui->RedTsnInstanceComboBox->count() > 0) && (red_tsn_priority_support.count() > 0))
    {
        red_tsn_capabilities();
    }
}

void Ucm_RedTsnTab::red_tsn_read_values_button_clicked(void)
{
    red_tsn_read_values();
}

void Ucm_RedTsnTab::red_tsn_write_values_button_clicked(void)
{
    red_tsn_write_values();
    red_tsn_read_values();
}

void Ucm_RedTsnTab::red_tsn_auto_refresh_button_clicked(void)
{
    if (ui->RedTsnAutoRefreshButton->text() == "Start Refresh")
    {
        ui->RedTsnAutoRefreshButton->setEnabled(false);

        ui->RedTsnInstanceComboBox->setEnabled(false);
        ui->RedTsnReadValuesButton->setEnabled(false);
        ui->RedTsnWriteValuesButton->setEnabled(false);

        red_tsn_timer->start(1000);

        ui->RedTsnAutoRefreshButton->setText("Stop Refresh");
        ui->RedTsnAutoRefreshButton->setEnabled(true);
    }
    else
    {
        ui->RedTsnAutoRefreshButton->setEnabled(false);

        red_tsn_timer->stop();

        ui->RedTsnInstanceComboBox->setEnabled(true);
        ui->RedTsnReadValuesButton->setEnabled(true);
        ui->RedTsnWriteValuesButton->setEnabled(true);

        ui->RedTsnAutoRefreshButton->setText("Start Refresh");
        ui->RedTsnAutoRefreshButton->setEnabled(true);
    }
}

void Ucm_RedTsnTab::red_tsn_scheduling_button_clicked(void)
{
    ui_scheduling->show();
    ui_scheduling->activateWindow();
    ui_scheduling->raise();
}


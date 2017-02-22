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

#include "Ucm_AdvancedTab.h"
#include "ui_Ucm_AdvancedTab.h"

Ucm_AdvancedTab::Ucm_AdvancedTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_AdvancedTab();
    ui->setupUi(this);

    debug_stream = new Ucm_DebugStream(cout, ui->AdvancedDebugConsole);

    connect(ui->AdvancedLoadConfigButton, SIGNAL(clicked()), this, SLOT(advanced_load_config_button_clicked()));
    connect(ui->AdvancedConfigFileBrowseButton, SIGNAL(clicked()), this, SLOT(advanced_config_file_browse_button_clicked()));
    connect(ui->AdvancedReadValueButton, SIGNAL(clicked()), this, SLOT(advanced_read_value_button_clicked()));
    connect(ui->AdvancedWriteValueButton, SIGNAL(clicked()), this, SLOT(advanced_write_value_button_clicked()));
    connect(ui->AdvancedClearLogButton, SIGNAL(clicked()), this, SLOT(advanced_clear_log_button_clicked()));
}

Ucm_AdvancedTab::~Ucm_AdvancedTab()
{
    delete ui;
}

int Ucm_AdvancedTab::advanced_enable(void)
{
    ui->AdvancedLoadConfigButton->setEnabled(true);
    ui->AdvancedReadValueButton->setEnabled(true);
    ui->AdvancedWriteValueButton->setEnabled(true);
    return 0;
}

int Ucm_AdvancedTab::advanced_disable(void)
{
    ui->AdvancedLoadConfigButton->setEnabled(false);
    ui->AdvancedReadValueButton->setEnabled(false);
    ui->AdvancedWriteValueButton->setEnabled(false);

    return 0;
}

void Ucm_AdvancedTab::advanced_read_values(void)
{
    unsigned int temp_addr;
    unsigned int temp_data;
    QString temp_string;

    temp_string = ui->AdvancedAddressValue->text();
    temp_addr = temp_string.toUInt(nullptr, 16);
    if (temp_string == "NA")
    {
        ui->AdvancedValueValue->setText("NA");
        return;
    }

    if (0 == ucm->com_lib.read_reg(temp_addr, temp_data))
    {
        ui->AdvancedValueValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->AdvancedValueValue->setText("NA");
    }
}

void Ucm_AdvancedTab::advanced_write_values(void)
{
    unsigned int temp_addr;
    unsigned int temp_data;
    QString temp_string;

    temp_string = ui->AdvancedAddressValue->text();
    temp_addr = temp_string.toUInt(nullptr, 16);

    if (temp_string == "NA")
    {
        ui->AdvancedValueValue->setText("NA");
        return;
    }

    temp_string = ui->AdvancedValueValue->text();
    temp_data = temp_string.toUInt(nullptr, 16);

    if (temp_string == "NA")
    {
        ui->AdvancedValueValue->setText("NA");
        return;
    }

    if (0 == ucm->com_lib.write_reg(temp_addr, temp_data))
    {
        ui->AdvancedValueValue->setText(QString("0x%1").arg(temp_data, 8, 16, QLatin1Char('0')));
        ui->AdvancedAddressValue->setText(QString("0x%1").arg(temp_addr, 8, 16, QLatin1Char('0')));
    }
    else
    {
        ui->AdvancedValueValue->setText("NA");
        ui->AdvancedAddressValue->setText("NA");
    }
}

void Ucm_AdvancedTab::advanced_read_value_button_clicked(void)
{
    advanced_read_values();
}

void Ucm_AdvancedTab::advanced_write_value_button_clicked(void)
{
    advanced_write_values();
    advanced_read_values();
}

void Ucm_AdvancedTab::advanced_clear_log_button_clicked(void)
{
    ui->AdvancedDebugConsole->clear();
    debug_stream->line_nr = 0;
}

void Ucm_AdvancedTab::advanced_load_config_button_clicked(void)
{
    unsigned int temp_addr;
    unsigned int temp_data;

    QFile temp_file(ui->AdvancedConfigFileValue->text());


    if (false == temp_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "ERROR: " << "opening file:" << ui->AdvancedConfigFileValue->text().toLatin1().constData() << " failed" << endl;
        return;
    }

    while (false == temp_file.atEnd())
    {
        QByteArray temp_line = temp_file.readLine();

        if(true == temp_line.startsWith("$WC,"))
        {
            temp_addr = temp_line.mid(4,10).toULong(nullptr, 16);
            temp_data = temp_line.mid(15,10).toULong(nullptr, 16);
            if (0 != ucm->com_lib.write_reg(temp_addr, temp_data))
            {
                cout << "ERROR: " << "write reg failed" << endl;
            }
        }
        else if(true == temp_line.startsWith("$RC,"))
        {
            temp_addr = temp_line.mid(4,10).toULong(nullptr, 16);
            if (0 != ucm->com_lib.read_reg(temp_addr, temp_data))
            {
                cout << "ERROR: " << "read reg failed" << endl;
            }
        }
        else
        {
            continue;
        }
    }
}

void Ucm_AdvancedTab::advanced_config_file_browse_button_clicked(void)
{
    QString temp_file = QFileDialog::getOpenFileName(this, tr("open config"), QDir::currentPath(), tr("config files (*.ucm)"));
    if(false == temp_file.isEmpty())
    {
        ui->AdvancedConfigFileValue->setText(temp_file);
    }
}

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

#include <Ucm_AdvancedTab.h>
#include <ui_Ucm_AdvancedTab.h>

Ucm_AdvancedTab::Ucm_AdvancedTab(Ucm_UniversalConfigurationManager *parent) : QWidget()
{
    ucm = parent;

    ui = new Ui::Ucm_AdvancedTab();
    ui->setupUi(this);

    debug_stream = new Ucm_DebugStream(cout, ui->AdvancedDebugConsole);

    connect(ui->AdvancedReadValueButton, SIGNAL(clicked()), this, SLOT(advanced_read_value_button_clicked()));
    connect(ui->AdvancedWriteValueButton, SIGNAL(clicked()), this, SLOT(advanced_write_value_button_clicked()));
    connect(ui->AdvancedClearLogButton, SIGNAL(clicked()), this, SLOT(advanced_clear_log_button_clicked()));
    connect(ui->AdvancedLoadConfigButton, SIGNAL(clicked()), this, SLOT(advanced_load_config_button_clicked()));
    connect(ui->AdvancedSaveConfigButton, SIGNAL(clicked()), this, SLOT(advanced_save_config_button_clicked()));
    connect(ui->AdvancedSaveLogButton, SIGNAL(clicked()), this, SLOT(advanced_save_log_button_clicked()));
    connect(ui->AdvancedLoadConfigFileBrowseButton, SIGNAL(clicked()), this, SLOT(advanced_load_config_file_browse_button_clicked()));
    connect(ui->AdvancedSaveConfigFileBrowseButton, SIGNAL(clicked()), this, SLOT(advanced_save_config_file_browse_button_clicked()));
    connect(ui->AdvancedSaveLogFileBrowseButton, SIGNAL(clicked()), this, SLOT(advanced_save_log_file_browse_button_clicked()));
}

Ucm_AdvancedTab::~Ucm_AdvancedTab()
{
    delete ui;
}

int Ucm_AdvancedTab::advanced_resize(int height, int width)
{
    int height_delta = 0;
    int width_delta = 0;

    height_delta = (height-Ucm_MainHeight);
    width_delta = (width-Ucm_MainWidth);

    ui->AdvancedDebugConsole->setFixedHeight(580+height_delta);
    ui->AdvancedDebugConsole->setFixedWidth(1340+width_delta);

    updateGeometry();

    return 0;
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

    ui->AdvancedAddressValue->setText("NA");
    ui->AdvancedValueValue->setText("NA");

    return 0;
}

void Ucm_AdvancedTab::advanced_read_values(void)
{
    unsigned int temp_addr = 0;
    unsigned int temp_data = 0;
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
    unsigned int temp_addr = 0;
    unsigned int temp_data = 0;
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
    }
    else
    {
        ui->AdvancedValueValue->setText("NA");
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
    unsigned int temp_addr = 0;
    unsigned int temp_data = 0;

    if (ui->AdvancedLoadConfigFileValue->text() == "NA")
    {
        cout << "ERROR: " << "no load config file specified" << endl;
        QMessageBox::information(this, tr("Load Config"), tr("No load config file specified"));
        return;
    }

    QFile temp_file(ui->AdvancedLoadConfigFileValue->text());


    if (false == temp_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "ERROR: " << "opening load config file:" << ui->AdvancedLoadConfigFileValue->text().toLatin1().constData() << " failed" << endl;
        QMessageBox::information(this, tr("Load Config"), tr("Opening load config file failed"));
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

    temp_file.close();

    cout << "INFO: " << "config file loaded" << endl;

    QMessageBox::information(this, tr("Load Config"), tr("Config loaded from file"));
}

void Ucm_AdvancedTab::advanced_save_config_button_clicked(void)
{
    if (ui->AdvancedSaveConfigFileValue->text() == "NA")
    {
        cout << "ERROR: " << "no save config file specified" << endl;
        QMessageBox::information(this, tr("Save Config"), tr("No save config file specified"));
        return;
    }

    QFile temp_file(ui->AdvancedSaveConfigFileValue->text());


    if (false == temp_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cout << "ERROR: " << "opening save config file:" << ui->AdvancedSaveConfigFileValue->text().toLatin1().constData() << " failed" << endl;
        QMessageBox::information(this, tr("Save Config"), tr("Opening save config file failed"));
        return;
    }

    QTextStream temp_stream(&temp_file);

    QString temp_string = ui->AdvancedDebugConsole->toPlainText();

    QStringList temp_string_list = temp_string.split("\n");

    for (int i = 0; i < temp_string_list.count(); i++)
    {
        if (temp_string_list.at(i).contains("VERBOSE: sent command: $WC"))
        {
            QString temp_command = temp_string_list.at(i);
            temp_command = temp_command.remove("VERBOSE: sent command: ");
            temp_command = temp_command.remove(25,3);
            temp_stream << temp_command << endl;
        }
    }

    temp_file.close();

    cout << "INFO: " << "config file saved" << endl;

    QMessageBox::information(this, tr("Save Config"), tr("Config saved to file"));
}

void Ucm_AdvancedTab::advanced_save_log_button_clicked(void)
{
    if (ui->AdvancedSaveLogFileValue->text() == "NA")
    {
        cout << "ERROR: " << "no save log file specified" << endl;
        QMessageBox::information(this, tr("Save Log"), tr("No save log file specified"));
        return;
    }

    QFile temp_file(ui->AdvancedSaveLogFileValue->text());

    if (false == temp_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cout << "ERROR: " << "opening save log file:" << ui->AdvancedSaveLogFileValue->text().toLatin1().constData() << " failed" << endl;
        QMessageBox::information(this, tr("Save Log"), tr("Opening save log file failed"));
        return;
    }

    QTextStream temp_stream(&temp_file);
    QString temp_string = ui->AdvancedDebugConsole->toPlainText();

    temp_stream << temp_string << endl;

    temp_file.close();

    cout << "INFO: " << "log file saved" << endl;

    QMessageBox::information(this, tr("Save Log"), tr("Log saved to file"));
}

void Ucm_AdvancedTab::advanced_load_config_file_browse_button_clicked(void)
{
    QString temp_directory;

    if (ui->AdvancedLoadConfigFileValue->text() == "NA")
    {
        temp_directory = QDir::currentPath();
    }
    else
    {
        temp_directory = ui->AdvancedLoadConfigFileValue->text();
    }

    QString temp_file = QFileDialog::getOpenFileName(this, tr("open config"), temp_directory, tr("config files (*.ucm)"));
    if(false == temp_file.isEmpty())
    {
        ui->AdvancedLoadConfigFileValue->setText(temp_file);
    }
}

void Ucm_AdvancedTab::advanced_save_config_file_browse_button_clicked(void)
{
    QString temp_directory;

    if (ui->AdvancedSaveConfigFileValue->text() == "NA")
    {
        temp_directory = QDir::currentPath();
    }
    else
    {
        temp_directory = ui->AdvancedSaveConfigFileValue->text();
    }

    QString temp_file = QFileDialog::getSaveFileName(this, tr("save config"), temp_directory, tr("config files (*.ucm)"));
    if(false == temp_file.isEmpty())
    {
        ui->AdvancedSaveConfigFileValue->setText(temp_file);
    }
}

void Ucm_AdvancedTab::advanced_save_log_file_browse_button_clicked(void)
{
    QString temp_directory;

    if (ui->AdvancedSaveLogFileValue->text() == "NA")
    {
        temp_directory = QDir::currentPath();
    }
    else
    {
        temp_directory = ui->AdvancedSaveLogFileValue->text();
    }

    QString temp_file = QFileDialog::getSaveFileName(this, tr("save log"), temp_directory, tr("config files (*.log)"));
    if(false == temp_file.isEmpty())
    {
        ui->AdvancedSaveLogFileValue->setText(temp_file);
    }
}

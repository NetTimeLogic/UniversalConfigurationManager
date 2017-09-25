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

#ifndef UCM_ADVANCEDTAB_H
#define UCM_ADVANCEDTAB_H

#include <QWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_AdvancedTab;
}

class Ucm_AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_AdvancedTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_AdvancedTab();

    int advanced_resize(int height, int width);
    int advanced_disable(void);
    int advanced_enable(void);


private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_AdvancedTab *ui;

    // Advanced tab
    Ucm_DebugStream* debug_stream;

    void advanced_read_values(void);
    void advanced_write_values(void);

private slots:
    void advanced_read_value_button_clicked(void);
    void advanced_write_value_button_clicked(void);
    void advanced_clear_log_button_clicked(void);
    void advanced_save_log_button_clicked(void);
    void advanced_load_config_button_clicked(void);
    void advanced_save_config_button_clicked(void);
    void advanced_load_config_file_browse_button_clicked(void);
    void advanced_save_config_file_browse_button_clicked(void);
    void advanced_save_log_file_browse_button_clicked(void);
};

#endif // UCM_ADVANCEDTAB_H

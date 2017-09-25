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

#ifndef UCM_CONFIGTAB_H
#define UCM_CONFIGTAB_H

#include <QWidget>
#include <QMessageBox>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_ConfigTab;
}

class Ucm_ConfigTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_ConfigTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_ConfigTab();

    int config_resize(int height, int width);

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_ConfigTab *ui;

    // Config tab
    QTimer* config_timer;

private slots:
    void config_com_port_button_clicked(void);
    void config_full_screen_button_clicked(void);
    void config_com_port_check_timer(void);
};

#endif // UCM_CONFIGTAB_H

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

#ifndef UCM_REDHSRPRP_H
#define UCM_REDHSRPRP_H

#include <QWidget>
#include "../Ucm_UniversalConfigurationManager.h"

class Ucm_UniversalConfigurationManager;

namespace Ui {
class Ucm_RedHsrPrpTab;
}

class Ucm_RedHsrPrpTab : public QWidget
{
    Q_OBJECT

public:
    Ucm_RedHsrPrpTab(Ucm_UniversalConfigurationManager* parent);
    ~Ucm_RedHsrPrpTab();

    int red_hsrprp_resize(int height, int width);
    void red_hsrprp_add_instance(unsigned int instance);
    int red_hsrprp_disable(void);
    int red_hsrprp_enable(void);

public:

private:
    Ucm_UniversalConfigurationManager* ucm;
    Ui::Ucm_RedHsrPrpTab *ui;

    // RED HsrPrp tab
    QTimer* red_hsrprp_timer;

    void red_hsrprp_read_values(void);
    void red_hsrprp_write_values(void);

private slots:
    // RED HsrPrp tab
    void red_hsrprp_read_values_button_clicked(void);
    void red_hsrprp_write_values_button_clicked(void);
    void red_hsrprp_auto_refresh_button_clicked(void);

};

#endif // UCM_REDHSRPRP_H

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

#include <Ucm_RedTsnSchedulingScreen.h>
#include <ui_Ucm_RedTsnSchedulingScreen.h>

Ucm_RedTsnSchedulingScreen::Ucm_RedTsnSchedulingScreen(QWidget *parent) : QWidget(parent)
{

    ui = new Ui::Ucm_RedTsnSchedulingScreen();
    ui->setupUi(this);

    QPalette pal;
    pal.setColor(QPalette::Background, Qt::white);
    setPalette(pal);

    connect(ui->RedTsnDoneButton, SIGNAL(clicked()), this, SLOT(red_tsn_done_button_clicked()));
}

Ucm_RedTsnSchedulingScreen::~Ucm_RedTsnSchedulingScreen()
{
    delete ui;
}

void Ucm_RedTsnSchedulingScreen::red_tsn_done_button_clicked(void)
{
    close();
}


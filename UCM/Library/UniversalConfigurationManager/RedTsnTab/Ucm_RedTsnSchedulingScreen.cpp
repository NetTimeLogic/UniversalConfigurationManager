#include <Ucm_RedTsnSchedulingScreen.h>
#include <ui_Ucm_RedTsnSchedulingScreen.h>

Ucm_RedTsnSchedulingScreen::Ucm_RedTsnSchedulingScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ucm_RedTsnSchedulingScreen)
{
    ui->setupUi(this);
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


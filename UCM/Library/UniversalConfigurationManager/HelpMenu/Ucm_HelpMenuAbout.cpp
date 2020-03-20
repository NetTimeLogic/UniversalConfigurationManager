#include <Ucm_HelpMenuAbout.h>
#include <ui_Ucm_HelpMenuAbout.h>
#include <Ucm_UniversalConfigurationManager.h>

Ucm_HelpMenuAbout::Ucm_HelpMenuAbout() : QWidget()
{
    QString temp_version;

    ui = new Ui::Ucm_HelpMenuAbout();
    ui->setupUi(this);

    QPalette pal;
    pal.setColor(QPalette::Background, Qt::white);
    setPalette(pal);

    temp_version.append("Version: ");
    temp_version.append(Ucm_Version_Nr);
    temp_version.append(" Build time: ");
    temp_version.append(__TIME__);
    temp_version.append(" ");
    temp_version.append(__DATE__);
    ui->AboutVersionValue->setText(temp_version);

    connect(ui->AboutOkButton, SIGNAL(clicked()), this, SLOT(about_ok_button_clicked()));
}

Ucm_HelpMenuAbout::~Ucm_HelpMenuAbout()
{
    delete ui;
}

void Ucm_HelpMenuAbout::about_ok_button_clicked(void)
{
    close();
}

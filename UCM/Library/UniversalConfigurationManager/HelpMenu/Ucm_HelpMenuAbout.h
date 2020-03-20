#ifndef UCM_HELPMENUABOUT_H
#define UCM_HELPMENUABOUT_H

#include <QWidget>

namespace Ui {
class Ucm_HelpMenuAbout;
}

class Ucm_HelpMenuAbout : public QWidget
{
    Q_OBJECT
public:
    Ucm_HelpMenuAbout();
    ~Ucm_HelpMenuAbout();

    Ui::Ucm_HelpMenuAbout *ui;

private slots:
    // About screen
    void about_ok_button_clicked(void);
};

#endif // UCM_HELPMENUABOUT_H

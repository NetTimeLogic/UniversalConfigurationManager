#ifndef UCM_REDTSNSCHEDULINGSCREEN_H
#define UCM_REDTSNSCHEDULINGSCREEN_H

#include <QWidget>

namespace Ui {
class Ucm_RedTsnSchedulingScreen;
}

class Ucm_RedTsnSchedulingScreen : public QWidget
{
    Q_OBJECT

public:
    explicit Ucm_RedTsnSchedulingScreen(QWidget *parent = nullptr);
    ~Ucm_RedTsnSchedulingScreen();

    Ui::Ucm_RedTsnSchedulingScreen *ui;

private slots:
    // RED TSN Scheduling screen
    void red_tsn_done_button_clicked(void);
};

#endif // UCM_REDTSNSCHEDULINGSCREEN_H

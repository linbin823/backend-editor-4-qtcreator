#ifndef SIMDRIVERUI_H
#define SIMDRIVERUI_H

#include <QWidget>
#include "simdriver.h"

namespace Ui {
class SimDriverUI;
}

class SimDriverUI : public QWidget
{
    Q_OBJECT

public:
    explicit SimDriverUI(SimDriver* driver, QWidget *parent = 0);
    ~SimDriverUI();

private slots:
    void on_pbStart_clicked();
    void on_pbStop_clicked();
    void updateState(int state);
    void updateReceivedDataN();
    void updateAcceptedDataN();
    void on_pbClose_clicked();
signals:
    //to avoid create object in this ui thread
    void start();
private:

    Ui::SimDriverUI *ui;
    SimDriver* _pDriver;
    long _receivedDataN, _acceptedDataN;
};

#endif // SIMDRIVERUI_H

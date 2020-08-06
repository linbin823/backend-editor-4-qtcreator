#ifndef DRIVERUI_H
#define DRIVERUI_H

#include <QWidget>
class DriverModel;
class QAbstractTableModel;

namespace Ui {
class DriverUI;
}

class DriverUI : public QWidget
{
    Q_OBJECT
public:
    explicit DriverUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~DriverUI();

private slots:
    void on_pbAdd_clicked();
    void on_pbDelete_clicked();
    void on_pbClose_clicked();
    void on_pbStart_clicked();
    void on_pbStop_clicked();
    void on_pbAllStart_clicked();
    void on_pbAllStop_clicked();
    void on_tvDrivers_clicked(const QModelIndex &index);

private:
    Ui::DriverUI *ui;
    DriverModel* _pModel;
};

#endif // DRIVERUI_H

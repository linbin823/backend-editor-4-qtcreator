#ifndef DRIVERUI_H
#define DRIVERUI_H

#include <QWidget>
class DriverConfigModel;
class QAbstractTableModel;

namespace Ui {
class DriverConfigUI;
}

class DriverConfigUI : public QWidget
{
    Q_OBJECT
public:
    explicit DriverConfigUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~DriverConfigUI();

private slots:
    void on_pbAdd_clicked();
    void on_pbDelete_clicked();
    void on_pbSave_clicked();
    void on_tvDrivers_clicked(const QModelIndex &index);

private:
    Ui::DriverConfigUI *ui;
    DriverConfigModel* _pModel;
};

#endif // DRIVERUI_H

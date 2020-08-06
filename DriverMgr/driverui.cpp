#include "driverui.h"
#include "ui_driverui.h"
#include "drivermodel.h"
#include "drivermgr.h"

DriverUI::DriverUI(QAbstractTableModel* model, QWidget *parent) :
    QWidget(parent), _pModel( static_cast<DriverModel*>(model) ),
    ui(new Ui::DriverUI)
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
    //setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("驱动管理"));

    ui->tvDrivers->setModel(_pModel);
    ui->tvDrivers->horizontalHeader()->setStretchLastSection(true);
}

DriverUI::~DriverUI()
{
    delete ui;
}

void DriverUI::on_pbAdd_clicked()
{
    _pModel->insertRow(ui->tvDrivers->currentIndex().row());
}

void DriverUI::on_pbDelete_clicked()
{
    _pModel->removeRow(ui->tvDrivers->currentIndex().row());
}

void DriverUI::on_pbClose_clicked()
{
    hide();
}

void DriverUI::on_pbStart_clicked()
{
    _pModel->startDriver(ui->tvDrivers->currentIndex().row());
}

void DriverUI::on_pbStop_clicked()
{
    _pModel->stopDriver(ui->tvDrivers->currentIndex().row());
}

void DriverUI::on_pbAllStart_clicked()
{
    _pModel->startAllDriver();
}

void DriverUI::on_pbAllStop_clicked()
{
    _pModel->stopAllDriver();
}

void DriverUI::on_tvDrivers_clicked(const QModelIndex &index)
{
    _pModel->showConfigUI(index);
}

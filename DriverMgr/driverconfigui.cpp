#include "driverconfigui.h"
#include "ui_driverconfigui.h"
#include "driverconfigmodel.h"
#include "../Interface/Drivers/drivermgr.h"
#include "../Interface/Drivers/idriver.h"

DriverConfigUI::DriverConfigUI(QAbstractTableModel* model, QWidget *parent) :
      QWidget(parent),
      ui(new Ui::DriverConfigUI),
      _pModel( static_cast<DriverConfigModel *>(model) )
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
    //setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("驱动管理"));

    ui->tvDrivers->setModel(_pModel);
    ui->tvDrivers->horizontalHeader()->setStretchLastSection(true);
}

DriverConfigUI::~DriverConfigUI()
{
    delete ui;
}

void DriverConfigUI::on_pbAdd_clicked()
{
    _pModel->insertRow(ui->tvDrivers->currentIndex().row());
}

void DriverConfigUI::on_pbDelete_clicked()
{
    _pModel->removeRow(ui->tvDrivers->currentIndex().row());
}

void DriverConfigUI::on_pbSave_clicked()
{
}

void DriverConfigUI::on_tvDrivers_clicked(const QModelIndex &index)
{
    _pModel->showConfigUI(index);
}

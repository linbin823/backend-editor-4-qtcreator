#include "insertdriver.h"
#include "ui_insertdriver.h"
#include "../Interface/Drivers/drivermgr.h"
#include "../Interface/Drivers/idriver.h"

static QList<DriverInfo> info = QList<DriverInfo>();

InsertDriver::InsertDriver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InsertDriver)
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
    //setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("新增驱动"));

    info = DriverConfigMgr::Instance()->supportDriversInfo();
    foreach (DriverInfo i, info) {
        ui->lwSupportedDriverInfo->addItem(i.driverType);
    }

    ui->tbDriverInfo->clear();
    ui->tbDriverInfo->setText(tr("请选择驱动"));

    _pos = 0;
}

InsertDriver::~InsertDriver()
{
    delete ui;
}

void InsertDriver::setInsertPos(int pos){
    _pos = pos;
}

void InsertDriver::on_pbAdd_clicked()
{
    int row = ui->lwSupportedDriverInfo->currentIndex().row();
    if(row >=0 && row < info.size()){
        DriverConfigMgr::Instance()->insertDriver( _pos, info.at(row) );
    }
    on_pbClose_clicked();
}

void InsertDriver::on_pbClose_clicked()
{
    ui->lwSupportedDriverInfo->clearSelection();
    ui->tbDriverInfo->setText(tr("请选择驱动"));
    hide();
}

void InsertDriver::on_lwSupportedDriverInfo_clicked(const QModelIndex &index)
{
    int row = index.row();
    if(row >=0 && row < info.size()){
        DriverInfo i = info.at(row);
        ui->tbDriverInfo->clear();
        ui->tbDriverInfo->setText(
            tr("驱动类型：\n%1\n\n驱动设计厂商：\n%2\n\n版本号：%3.%4.%5\n\n驱动描述：\n%6")
                .arg(i.driverType).arg(i.driverManufacturer)
                .arg(i.majorVersion).arg(i.minorVersion).arg(i.microVersion)
                .arg(i.description) );
    }
}

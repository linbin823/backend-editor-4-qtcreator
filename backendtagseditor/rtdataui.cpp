#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include "rtdataui.h"
#include "rtdata.h"
#include "backendtagsdelegates.h"
#include "ui_rtdataui.h"
//#include "PollGroups/pollgroupmgr.h"

RTDataUI::RTDataUI(QAbstractTableModel *model, QWidget *parent) :
    QWidget(parent), _pModel( static_cast<RTDataTableModel*>(model) ),
    ui(new Ui::RTDataUI)
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
    //setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("数据点设置"));

    ui->tvTags->setModel( _pModel );
    ui->tvTags->horizontalHeader()->setStretchLastSection(true);

//    _pPollGroupMgr = PollGroupMgr::Instance();
    connect(_pModel,&RTDataTableModel::updateDelegates,this,&RTDataUI::updateDelegates);
    updateDelegates();

    ui->cbInfo->setChecked( _pModel->partVisible(RTDataTableModel::INFO));
    ui->cbScale->setChecked( _pModel->partVisible(RTDataTableModel::SCALE));
    ui->cbOnlineState->setChecked( _pModel->partVisible(RTDataTableModel::ONLINESTATE));
}

RTDataUI::~RTDataUI()
{
    delete ui;
}

void RTDataUI::on_pbLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open 711 Sqlit Config DB"), "/home", tr("Sqlite Files (*.db)"));
    if(fileName.isNull() || fileName.isEmpty()) return;
    RTData::Instance()->load_CSIC711_ConfigDB( fileName );
}

void RTDataUI::on_pbClose_clicked()
{
    QList<int> errRows = _pModel->errorRows();
    if (errRows.size()!=0 ){
        QString errRowsString;
        foreach(int row, errRows){
            errRowsString.append(tr("第")).append(QString::number(row+1)).append(tr("行\n"));
        }
        if( QMessageBox::Cancel ==
                QMessageBox::question(this, tr("注意"),
                                      tr("存在错误！检查输入。\n%1").arg(errRowsString),
                                      QMessageBox::Cancel, QMessageBox::Ignore) )
            return;
    }
    hide();
}

void RTDataUI::on_pbDelete_clicked()
{
    int row = ui->tvTags->currentIndex().row();
    if(row<0) row=0;
    _pModel->removeRow(row);
}

void RTDataUI::on_pbAdd_clicked()
{
    int row = ui->tvTags->currentIndex().row();
    if(row<0) row=0;
    _pModel->insertRow(row);
}

void RTDataUI::updateDelegates(){
    foreach(QString driverName, _pModel->driverNames){
        ui->tvTags->setItemDelegateForColumn(
                    _pModel->column(RTDataTableModel::RWStrategy,driverName),
                    new TagInfoRWStrategyDelegate(_pModel, this));
    }
    ui->tvTags->setItemDelegateForColumn(
                _pModel->column(RTDataTableModel::Type),
                new TagInfoTypeDelegate(_pModel, this));
//    ui->tvTags->setItemDelegateForColumn(
//                _pModel->column(RTDataTableModel::PollGroup),
//                new PollGroupNameDelegate(this));
}

void RTDataUI::on_cbInfo_toggled(bool checked)
{
    _pModel->setPartVisible(RTDataTableModel::INFO, checked);
}

void RTDataUI::on_cbScale_toggled(bool checked)
{
    _pModel->setPartVisible(RTDataTableModel::SCALE, checked);
}

void RTDataUI::on_cbOnlineState_toggled(bool checked)
{
    _pModel->setPartVisible(RTDataTableModel::ONLINESTATE, checked);
}

//void RTDataUI::on_pushButton_clicked()
//{
//    _pPollGroupMgr->showUI();
//}

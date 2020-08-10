#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <TagMgrUI.h>
#include "tagmgrui.h"
#include "../Interface/Tags/tagmgr.h"
#include "../Interface/Tags/tag.h"
#include "tagmgrdelegates.h"
#include "ui_tagmgrui.h"
//#include "PollGroups/pollgroupmgr.h"

TagMgrUI::TagMgrUI(QAbstractTableModel *model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagMgrUI),
    _pModel( static_cast<TagMgrTableModel*>(model) )
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
    //setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("数据点设置."));

    ui->tvTags->setModel( _pModel );
    ui->tvTags->horizontalHeader()->setStretchLastSection(true);

//    _pPollGroupMgr = PollGroupMgr::Instance();
    connect(_pModel,&TagMgrTableModel::updateDelegates,this,&TagMgrUI::updateDelegates);
    updateDelegates();

    ui->cbInfo->setChecked( _pModel->partVisible(TagMgrTableModel::INFO));
    ui->cbScale->setChecked( _pModel->partVisible(TagMgrTableModel::SCALE));
    ui->cbOnlineState->setChecked( _pModel->partVisible(TagMgrTableModel::ONLINESTATE));
}

TagMgrUI::~TagMgrUI()
{
    delete ui;
}

void TagMgrUI::on_pbLoad_clicked()
{
//    QString fileName = QFileDialog::getOpenFileName(this,
//          tr("Open 711 Sqlit Config DB"), "/home", tr("Sqlite Files (*.db)"));
//    if(fileName.isNull() || fileName.isEmpty()) return;
//    TagMgr::Instance()->load_CSIC711_ConfigDB( fileName );
}

void TagMgrUI::on_pbSave_clicked()
{
    QList<int> errRows = _pModel->errorRows();
    if (errRows.size()!=0 ){
        QString errRowsString;
        foreach(int row, errRows){
            errRowsString.append(tr("第 ")).append(QString::number(row+1)).append(tr(" 行.\n"));
        }
        if( QMessageBox::Cancel ==
                QMessageBox::question(this, tr("注意"),
                                      tr("存在错误！检查输入。\n%1").arg(errRowsString),
                                      QMessageBox::Cancel, QMessageBox::Ignore) )
            return;
    }
}

void TagMgrUI::on_pbDelete_clicked()
{
    int row = ui->tvTags->currentIndex().row();
    if(row<0) row=0;
    _pModel->removeRow(row);
}

void TagMgrUI::on_pbAdd_clicked()
{
    int row = ui->tvTags->currentIndex().row();
    if(row<0) row=0;
    _pModel->insertRow(row);
}

void TagMgrUI::updateDelegates(){
    foreach(QString driverName, _pModel->driverNames){
        ui->tvTags->setItemDelegateForColumn(
                    _pModel->column(TagMgrTableModel::RWStrategy,driverName),
                    new TagRWStrategyDelegate(_pModel, this));
    }
    ui->tvTags->setItemDelegateForColumn(
                _pModel->column(TagMgrTableModel::Type),
                new TagTypeDelegate(_pModel, this));
//    ui->tvTags->setItemDelegateForColumn(
//                _pModel->column(TagMgrTableModel::PollGroup),
//                new PollGroupNameDelegate(this));
}

void TagMgrUI::on_cbInfo_toggled(bool checked)
{
    _pModel->setPartVisible(TagMgrTableModel::INFO, checked);
}

void TagMgrUI::on_cbScale_toggled(bool checked)
{
    _pModel->setPartVisible(TagMgrTableModel::SCALE, checked);
}

void TagMgrUI::on_cbOnlineState_toggled(bool checked)
{
    _pModel->setPartVisible(TagMgrTableModel::ONLINESTATE, checked);
}

//void TagMgrUI::on_pushButton_clicked()
//{
//    _pPollGroupMgr->showUI();
//}

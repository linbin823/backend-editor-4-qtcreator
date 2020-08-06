#include "pollgroupui.h"
#include "ui_pollgroupui.h"

PollGroupUI::PollGroupUI(QAbstractTableModel *model, QWidget *parent) :
    QWidget(parent), _pModel(model),
    ui(new Ui::PollGroupUI)
{
    ui->setupUi(this);
    setLayout( ui->gridLayout );
    //setWindowFlag( Qt::WindowStaysOnTopHint );
    setWindowTitle(tr("轮询组设置"));

    ui->tvPollGroups->setModel(_pModel);
    ui->tvPollGroups->horizontalHeader()->setStretchLastSection(true);
}

PollGroupUI::~PollGroupUI()
{
    delete ui;
}

void PollGroupUI::on_pbAdd_clicked()
{
    _pModel->insertRow(ui->tvPollGroups->currentIndex().row() );
}

void PollGroupUI::on_pbDelete_clicked()
{
    _pModel->removeRow(ui->tvPollGroups->currentIndex().row() );
}

void PollGroupUI::on_pbClose_clicked()
{
    hide();
}

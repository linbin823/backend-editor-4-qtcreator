#include "modebackendmainwidget.h"
#include "ui_modebackendmainwidget.h"

ModeBackendMainWidget::ModeBackendMainWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModeBackendMainWidget)
{
    ui->setupUi(this);
}

ModeBackendMainWidget::~ModeBackendMainWidget()
{
    delete ui;
}

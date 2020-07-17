#include "modebackendmainwidget.h"
#include "ui_modebackendmainwidget.h"

namespace BackendEditor {
namespace Internal {

ModeBackendMainWidget::ModeBackendMainWidget(QWidget *parent) :
      QDialog(parent)
      , ui(new Ui::ModeBackendMainWidget)
{
    ui->setupUi(this);
}

ModeBackendMainWidget::~ModeBackendMainWidget()
{
    delete ui;
}

} // namespace Internal
} // namespace BackendEditor

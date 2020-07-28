#include "modebackendmainwidget.h"
#include "ui_modebackendmainwidget.h"
#include "modebackendcreatewizard.h"
#include <projectexplorer/projecttree.h>

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

void BackendEditor::Internal::ModeBackendMainWidget::on_pushButton_clicked()
{
    ProjectExplorer::BuildSystem* bs = ProjectExplorer::ProjectTree::currentBuildSystem();
    if(bs == nullptr) { return; }

    ModeBackendCreateWizard wizard(bs);
    wizard.exec();
}

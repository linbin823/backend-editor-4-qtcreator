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

void ModeBackendMainWidget::on_pbCreateTemplate_clicked()
{
    ProjectExplorer::BuildSystem* bs = ProjectExplorer::ProjectTree::currentBuildSystem();
    if(bs == nullptr) { return; }

    ModeBackendCreateWizard wizard(bs);
    wizard.exec();
}


} // namespace Internal
} // namespace BackendEditor

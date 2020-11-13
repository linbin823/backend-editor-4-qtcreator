#include "modemainwidget.h"
#include "ui_modemainwidget.h"
#include "createwizard.h"
#include "../backendeditorconstants.h"
#include "../Interface/Tags/tagmgr.h"
#include "../Interface/Drivers/drivermgr.h"

#include <projectexplorer/projecttree.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildtargetinfo.h>

#include <qmldesigner/designercore/include/model.h>
#include <qmldesigner/designercore/include/modelnode.h>

#include <QUrl>

namespace BackendEditor {
namespace Internal {

ModeMainWidget::ModeMainWidget(QWidget *parent) :
      QWidget(parent),
      ui(new Ui::ModeMainWidget),
      _pDriversModel(QmlDesigner::Model::create("")),
      _pTagsModel(nullptr)
{
    ui->setupUi(this);
    setLayout(ui->formLayout);

    ui->driversConfigEditor->setLayout(ui->driversLayout);
    ui->driversLayout->addWidget(DriverConfigMgr::Instance()->showUI());

    ui->tagsConfigEditor->setLayout(ui->tagsLayout);
    ui->tagsLayout->addWidget(TagMgr::Instance()->showUI());

    parseConfigFile();
}

ModeMainWidget::~ModeMainWidget()
{
    delete ui;
}

void ModeMainWidget::setActivePage(Utils::Id pageId)
{
    if (pageId == Utils::Id(Constants::DRIVERS_CONFIG_EDITOR_ID)) {
        ui->twConfigEditorsArea->setCurrentWidget(ui->driversConfigEditor);
    }
    else if (pageId == Utils::Id(Constants::TAGS_CONFIG_EDITOR_ID)) {
        ui->twConfigEditorsArea->setCurrentWidget(ui->tagsConfigEditor);
    }
}

void ModeMainWidget::parseConfigFile()
{
    ProjectExplorer::BuildSystem* bs = ProjectExplorer::ProjectTree::currentBuildSystem();
    if(bs == nullptr) { return; }

    const ProjectExplorer::Target *target = bs->target();
    const QString buildKey = target->activeBuildKey();
    const ProjectExplorer::BuildTargetInfo bti = target->buildTarget(buildKey);
    const QString projectDir = bti.projectFilePath.toFileInfo().absolutePath();

    QString backendTemplateDir;
    backendTemplateDir = projectDir + "/backend/";
    ui->lbTemplatePath->setText("    后端模板文件夹所在位置：  " + projectDir);

    QUrl driversConfigFile = QUrl::fromLocalFile(projectDir + "Drivers.config.qml");
    _pDriversModel->setFileUrl(driversConfigFile);
    //    ModelNode* rootNode = _pDriversModel->
}

void ModeMainWidget::on_pbCreateTemplate_clicked()
{
    ProjectExplorer::BuildSystem* bs = ProjectExplorer::ProjectTree::currentBuildSystem();
    if(bs == nullptr) { return; }

    CreateWizard wizard(bs);
    wizard.exec();

    parseConfigFile();
}

} // namespace Internal
} // namespace BackendEditor

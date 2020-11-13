#include "createwizard.h"
#include "../backendeditorconstants.h"
#include <coreplugin/editormanager/editormanager.h>

#include <projectexplorer/buildsystem.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/target.h>

#include <qtsupport/qtkitinformation.h>

#include <cmakeprojectmanager/cmakeprojectconstants.h>

#include <qmlprojectmanager/qmlprojectmanagerconstants.h>
#include <qmlprojectmanager/qmlproject.h>

#include <qmakeprojectmanager/qmakeprojectmanagerconstants.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <utils/infolabel.h>
#include <utils/pathchooser.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

using namespace ProjectExplorer;
using namespace Utils;

namespace BackendEditor {
namespace Internal {

//
// CanNotCreateTemplatePage
//

class CanNotCreateTemplatePage : public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(BackendEditor::CanNotCreateTemplatePage)

public:
    CanNotCreateTemplatePage(CreateWizard *wizard);
};

CanNotCreateTemplatePage::CanNotCreateTemplatePage(CreateWizard *)
{
    auto layout = new QVBoxLayout(this);
    auto label = new QLabel(this);
    label->setWordWrap(true);
    label->setText(tr("不支持的Project类型，或者找不到App Project。   ")); // TODO: 乱码
    layout->addWidget(label);
    setTitle(tr("无法添加后台模板文件。   ")); // TODO: 乱码
}


//
// ChooseProFilePage
//

class ChooseProFilePage : public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(BackendEditor::ChooseProfilePage)

public:
    explicit ChooseProFilePage(CreateWizard *wizard);

private:
    void nodeSelected(int index);

    CreateWizard *m_wizard;
    QComboBox *m_comboBox;
};

ChooseProFilePage::ChooseProFilePage(CreateWizard *wizard)
    : m_wizard(wizard)
{
    auto fl = new QFormLayout(this);
    QLabel *label = new QLabel(this);
    label->setWordWrap(true);
    label->setText(tr("请选择您要创建后台模板文件的工程文件：   "));
    fl->addRow(label);

    BuildSystem *buildSystem = wizard->buildSystem();
    QString currentBuildKey = buildSystem->target()->activeBuildKey();

    m_comboBox = new QComboBox(this);
    for (const BuildTargetInfo &bti : buildSystem->applicationTargets()) {
        const QString displayName = bti.buildKey;
        m_comboBox->addItem(displayName, QVariant(bti.buildKey)); // TODO something more?
        if (bti.buildKey == currentBuildKey)
            m_comboBox->setCurrentIndex(m_comboBox->count() - 1);
    }

    nodeSelected(m_comboBox->currentIndex());
    connect(m_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChooseProFilePage::nodeSelected);

    fl->addRow(tr("工程文件：   "), m_comboBox);
    setTitle(tr("选择一个工程文件。   "));
}

void ChooseProFilePage::nodeSelected(int index)
{
    Q_UNUSED(index)
    m_wizard->setBuildKey(m_comboBox->itemData(m_comboBox->currentIndex()).toString());
}


//
// ChooseDirectoryPage
//

class ChooseDirectoryPage : public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(BackendEditor::ChooseDirectoryPage)

public:
    ChooseDirectoryPage(CreateWizard *wizard);

private:
    void initializePage();
    bool isComplete() const;
    void checkPackageSourceDir();

    CreateWizard *m_wizard;
    PathChooser *m_backendTemplateDir = nullptr;
    InfoLabel *m_sourceDirectoryWarning = nullptr;
    QLabel *m_label;
    QFormLayout *m_layout;
    bool m_complete = true;
};

ChooseDirectoryPage::ChooseDirectoryPage(CreateWizard *wizard)
    : m_wizard(wizard)
{
    m_layout = new QFormLayout(this);
    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    m_layout->addRow(m_label);

    m_backendTemplateDir = new PathChooser(this);
    m_backendTemplateDir->setExpectedKind(PathChooser::Directory);
    m_layout->addRow(tr("添加文件的路径：   "), m_backendTemplateDir);

    m_sourceDirectoryWarning =
            new InfoLabel(tr("后台模板文件的路径不能与工程路径相同：   "), InfoLabel::Error, this);
    m_sourceDirectoryWarning->setVisible(false);
    m_sourceDirectoryWarning->setElideMode(Qt::ElideNone);
    m_sourceDirectoryWarning->setWordWrap(true);
    m_layout->addRow(m_sourceDirectoryWarning);

    connect(m_backendTemplateDir, &PathChooser::pathChanged,
            m_wizard, &CreateWizard::setDirectory);
}

void ChooseDirectoryPage::checkPackageSourceDir()
{
    const QString buildKey = m_wizard->buildKey();
    const BuildTargetInfo bti = m_wizard->buildSystem()->buildTarget(buildKey);
    const QString projectDir = bti.projectFilePath.toFileInfo().absolutePath();

    const QString newDir = m_backendTemplateDir->filePath().toString();
    bool isComplete = QFileInfo(projectDir) != QFileInfo(newDir);

    m_sourceDirectoryWarning->setVisible(!isComplete);

    if (isComplete != m_complete) {
        m_complete = isComplete;
        emit completeChanged();
    }
}

bool ChooseDirectoryPage::isComplete() const
{
    return m_complete;
}

void ChooseDirectoryPage::initializePage()
{
    const Target *target = m_wizard->buildSystem()->target();
    const QString buildKey = m_wizard->buildKey();
    const BuildTargetInfo bti = target->buildTarget(buildKey);
    const QString projectDir = bti.projectFilePath.toFileInfo().absolutePath();

    QString backendTemplateDir;
//    if (const ProjectNode *node = target->project()->findNodeForBuildKey(buildKey))
//        backendTemplateDir = node->data(BackendEditor::Constants::BACKEND_TEMPLATE_DIR).toString();

//    if (backendTemplateDir.isEmpty()) {
    m_label->setText(tr("选择后台模板文件的路径：\n\n后台模板文件默认会拷贝到工程路径下的backend文件夹. "));  // TODO: 乱码

    m_backendTemplateDir->setPath(projectDir + "/backend");
    connect(m_backendTemplateDir, &PathChooser::rawPathChanged,
            this, &ChooseDirectoryPage::checkPackageSourceDir);
//    } else {
//        m_label->setText(tr("后台模板文件的路径会保存到工程文件的BACKEND_TEMPLATE_DIR变量中。  ")); // TODO: 乱码
//        m_backendTemplateDir->setPath(backendTemplateDir);
//        m_backendTemplateDir->setReadOnly(true);
//    }


    m_wizard->setDirectory(m_backendTemplateDir->filePath().toString());
}

//
// CreateWizard
//
CreateWizard::CreateWizard(BuildSystem *buildSystem)
    : m_buildSystem(buildSystem), m_copyState(Ask)
{
    setWindowTitle(tr("添加后端模板文件向导"));

    const QList<BuildTargetInfo> buildTargets = buildSystem->applicationTargets();

    Utils::Id projectId = buildSystem->project()->id();
    if (projectId == CMakeProjectManager::Constants::CMAKE_PROJECT_ID) {
        m_projectType = ProjectTypeCmake;
    } else if (projectId == QmlProjectManager::Constants::QML_PROJECT_ID) {
        m_projectType = ProjectTypeQMLProject;
    } else if (projectId == QmakeProjectManager::Constants::QMAKEPROJECT_ID) {
        m_projectType = ProjectTypeQmake;
    } else {
        m_projectType = ProjectTypeUnknow;
    }

    if (buildTargets.isEmpty() || m_projectType == ProjectTypeUnknow) {
        // oh uhm can't create anything
        addPage(new CanNotCreateTemplatePage(this));
    } else if (buildTargets.size() == 1) {
        setBuildKey(buildTargets.first().buildKey);
        addPage(new ChooseDirectoryPage(this));
    } else {
        addPage(new ChooseProFilePage(this));
        addPage(new ChooseDirectoryPage(this));
    }
}

QString CreateWizard::buildKey() const
{
    return m_buildKey;
}

void CreateWizard::setBuildKey(const QString &buildKey)
{
    m_buildKey = buildKey;
}

void CreateWizard::setDirectory(const QString &directory)
{
    m_directory = directory;
}

bool CreateWizard::copy(const QFileInfo &src, const QFileInfo &dst, QStringList * addedFiles)
{
    bool copyState_onetime = true;
    if (dst.exists()) {
        switch (m_copyState) {
        case Ask: {
            int res = QMessageBox::question(this,
                            tr("覆盖 %1 文件").arg(dst.fileName()),
                            tr("覆盖已存在的 \"%1\"?")
                                .arg(QDir(m_directory).relativeFilePath(dst.absoluteFilePath())),
                            QMessageBox::Yes | QMessageBox::YesToAll |
                            QMessageBox::No | QMessageBox::NoToAll |
                            QMessageBox::Cancel);

            switch (res) {
            case QMessageBox::YesToAll: {
                m_copyState = OverwriteAll;
                copyState_onetime = true;
                break;
            }
            case QMessageBox::Yes: {
                m_copyState = Ask;
                copyState_onetime = true;
                break;
            }
            case QMessageBox::NoToAll: {
                m_copyState = SkipAll;
                copyState_onetime = false;
                break;
            }
            case QMessageBox::No: {
                m_copyState = Ask;
                copyState_onetime = false;
                break;
            }
            default:
                return false;
            }
            break;
        }
        case OverwriteAll:
        case SkipAll: {
            copyState_onetime = false;
            break;
        }
        default:
            return false;
        }

        if (copyState_onetime) {
            QString path = dst.filePath();
            QFile file(path);
            file.setPermissions(QFile::ReadOther | QFile::WriteOther);
            bool ret = file.remove();
            if (!ret) {
                QMessageBox::warning(this, tr("文件替换失败！   "),
                                           tr("无法删除\"%1\""),
                                           tr("原因：%2")
                                         .arg(dst.filePath()).arg(file.errorString()));
                return false;
            }
        }
    }

    if (!dst.absoluteDir().exists())
        dst.absoluteDir().mkpath(dst.absolutePath());

    if (copyState_onetime && !QFile::copy(src.filePath(), dst.filePath())) {
        QMessageBox::warning(this, tr("文件拷贝失败！   "),
                                   tr("无法复制，从\"%1\"   到   \"%2\"")
                                .arg(src.filePath()).arg(dst.filePath()));
        return false;
    }
    addedFiles->append(dst.absoluteFilePath());
    return true;
}

void CreateWizard::createBackendTemplateFiles()
{
    if (m_directory.isEmpty())
        return;

    QStringList addedFiles;
    Target *target = m_buildSystem->target();
    QtSupport::BaseQtVersion *version = QtSupport::QtKitAspect::qtVersion(target->kit());
    if (!version)
        return;
    if (version->qtVersion() >= QtSupport::QtVersionNumber(5, 0, 0)) {
        const QString src = ":/template/backend";
        FileUtils::copyRecursively(FilePath::fromString(src),
                                   FilePath::fromString(m_directory),
                                   nullptr, [this, &addedFiles](QFileInfo src, QFileInfo dst, QString *){return copy(src, dst, &addedFiles);});

    } else {
        return;
    }

    ProjectNode *node = target->project()->findNodeForBuildKey(m_buildKey);

    if (m_projectType == ProjectTypeQmake) {
        QString backendTemplateDir;
        if (node) {
            node->addFiles(addedFiles);
//            backendTemplateDir = node->data(BackendEditor::Constants::BACKEND_TEMPLATE_DIR).toString();
//            if (backendTemplateDir.isEmpty()) {

            const BuildTargetInfo bti = target->buildTarget(m_buildKey);
            const QString value = "$$PWD/"
                                  + bti.projectFilePath.toFileInfo().absoluteDir().relativeFilePath(m_directory);

            // and now time for some magic
            QString scope;
            int flags = QmakeProjectManager::Internal::ProWriter::ReplaceValues;
            bool result = false;
            QmakeProjectManager::QmakeProFile *pro = dynamic_cast<QmakeProjectManager::QmakeProFileNode *>(node)->proFile();
            if (pro) {
                result = pro->setProVariable(BackendEditor::Constants::BACKEND_TEMPLATE_DIR,
                                              QStringList(value), scope, flags);
            }
            if (!result) {
                QMessageBox::warning(this,
                                     tr("工程文件更新失败。   "),
                                     tr("工程文件错误:\" %1\"。   ")
                                         .arg(bti.projectFilePath.toUserOutput()));
            }
//            }
        }
    } else if (m_projectType == ProjectTypeCmake) {
        // TODO: Prepare cmake file.
    } else if (m_projectType == ProjectTypeQMLProject) {
        // TODO: Prepare qmlproject file.
    } else {

    }

//    Core::EditorManager::openEditor(m_directory + QLatin1String("/AndroidManifest.xml"));
}

BuildSystem *CreateWizard::buildSystem() const
{
    return m_buildSystem;
}

void CreateWizard::accept()
{
    createBackendTemplateFiles();
    Wizard::accept();
}

} // namespace Internal
} // namespace BackendEditor

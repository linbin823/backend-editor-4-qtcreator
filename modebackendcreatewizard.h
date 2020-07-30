#ifndef MODEBACKENDCREATEWIZARD_H
#define MODEBACKENDCREATEWIZARD_H

#include <utils/fileutils.h>
#include <utils/id.h>
#include <utils/wizard.h>

namespace ProjectExplorer { class BuildSystem; }

namespace BackendEditor {
namespace Internal {

class ModeBackendCreateWizard : public Utils::Wizard
{
    Q_DECLARE_TR_FUNCTIONS(BackendEditor::ModeBackendCreateWizard)

public:
    ModeBackendCreateWizard(ProjectExplorer::BuildSystem *buildSystem);

    QString buildKey() const;
    void setBuildKey(const QString &buildKey);

    void accept() override;

    void setDirectory(const QString &directory);

    ProjectExplorer::BuildSystem *buildSystem() const;

private:
    enum CopyState {
        Ask,
        OverwriteAll,
        SkipAll
    };

    enum ProjectType {
        ProjectTypeQmake,
        ProjectTypeCmake,
        ProjectTypeQMLProject,
        ProjectTypeUnknow
    };

    bool copy(const QFileInfo &src, const QFileInfo &dst, QStringList *addedFiles);
    void createBackendTemplateFiles();

    ProjectExplorer::BuildSystem *m_buildSystem;
    QString m_buildKey;
    QString m_directory;
    ProjectType m_projectType;
    CopyState m_copyState;
};

} // namespace Internal
} // namespace BackendEditor

#endif // MODEBACKENDCREATEWIZARD_H

#include "backendeditormode.h"
#include "backendeditormodemainwidget.h"
#include "backendeditoricons.h"
#include "backendeditorconstants.h"

#include <QIcon>
#include <QSize>
#include <projectexplorer/session.h>
#include <coreplugin/modemanager.h>
#include <projectexplorer/project.h>

namespace BackendEditor {
namespace Internal {

BackendEditorMode::BackendEditorMode(QObject *parent)
{
    Q_UNUSED(parent)
    mainWidget = new BackendEditorModeMainWidget(nullptr);

    setWidget(mainWidget);

    setEnabled(ProjectExplorer::SessionManager::hasProjects());
    setDisplayName(QString("Backend"));
    setIcon(Utils::Icon::modeIcon(Icons::MODE_BACKENDEDITOR_CLASSIC,
                                  Icons::MODE_BACKENDEDITOR_FLAT,
                                  Icons::MODE_BACKENDEDITOR_FLAT_ACTIVE));
    setId(Constants::ID_MODE_BACKEND);

    connect(ProjectExplorer::SessionManager::instance(), &ProjectExplorer::SessionManager::projectAdded,
            this, [this]() {
                setEnabled(ProjectExplorer::SessionManager::hasProjects());
            });
    connect(ProjectExplorer::SessionManager::instance(), &ProjectExplorer::SessionManager::projectRemoved,
            this, [this]() {
                setEnabled(ProjectExplorer::SessionManager::hasProjects());
            });
}

BackendEditorMode::~BackendEditorMode()
{
    mainWidget->deleteLater();
    Core::ModeManager::activateMode(Constants::ID_MODE_BACKEND);
}

} // namespace Internal
} // namespace BackendEditor

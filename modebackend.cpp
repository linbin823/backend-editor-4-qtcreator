#include "modebackend.h"
#include "modebackendmainwidget.h"
#include "backendeditoricons.h"

#include <QIcon>
#include <QSize>
#include <projectexplorer/session.h>
#include <projectexplorer/project.h>

namespace BackendEditor {
namespace Internal {

ModeBackend::ModeBackend(QObject *parent)
{
    Q_UNUSED(parent)
    mainWidget = new ModeBackendMainWidget(nullptr);

    setWidget(mainWidget);
    setEnabled(ProjectExplorer::SessionManager::hasProjects());
    setDisplayName(QString("Backend"));
    setIcon(Utils::Icon::modeIcon(Icons::MODE_BACKENDEDITOR_CLASSIC,
                                  Icons::MODE_BACKENDEDITOR_FLAT,
                                  Icons::MODE_BACKENDEDITOR_FLAT_ACTIVE));


    connect(ProjectExplorer::SessionManager::instance(), &ProjectExplorer::SessionManager::projectAdded,
            this, [this]() {
                setEnabled(ProjectExplorer::SessionManager::hasProjects());
            });
    connect(ProjectExplorer::SessionManager::instance(), &ProjectExplorer::SessionManager::projectRemoved,
            this, [this]() {
                setEnabled(ProjectExplorer::SessionManager::hasProjects());
            });
}

ModeBackend::~ModeBackend()
{
    mainWidget->deleteLater();
}

} // namespace Internal
} // namespace BackendEditor

#include "modebackend.h"
#include "modebackendmainwidget.h"
#include "backendeditoricons.h"

#include <QIcon>
#include <QSize>

namespace BackendEditor {
ModeBackend::ModeBackend(QObject *parent)
{
    Q_UNUSED(parent)
    mainWidget = new ModeBackendMainWidget(nullptr);

    setWidget(mainWidget);
    setEnabled(true);
    setDisplayName(QString("Backend"));
    setIcon(Utils::Icon::modeIcon(Icons::MODE_BACKENDEDITOR_CLASSIC,
                                  Icons::MODE_BACKENDEDITOR_FLAT,
                                  Icons::MODE_BACKENDEDITOR_FLAT_ACTIVE));
}

ModeBackend::~ModeBackend()
{
    mainWidget->deleteLater();
}
} // namespace BackendEditor

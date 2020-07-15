#ifndef BACKENDEDITORPLUGIN_H
#define BACKENDEDITORPLUGIN_H

#include "backendeditor_global.h"

#include <extensionsystem/iplugin.h>

namespace BackendEditor {
namespace Internal {

class BackendEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "BackendEditor.json")

public:
    BackendEditorPlugin();
    ~BackendEditorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace BackendEditor

#endif // BACKENDEDITORPLUGIN_H

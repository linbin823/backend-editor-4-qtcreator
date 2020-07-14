#ifndef BACKENDEDITORPLUGIN_H
#define BACKENDEDITORPLUGIN_H

#include "backendeditor_global.h"

#include <extensionsystem/iplugin.h>

namespace backendEditor {
namespace Internal {

class backendEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "backendEditor.json")

public:
    backendEditorPlugin();
    ~backendEditorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace backendEditor

#endif // BACKENDEDITORPLUGIN_H

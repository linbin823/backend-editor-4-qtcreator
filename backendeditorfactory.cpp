#include "backendeditorfactory.h"
#include "backendeditormodemainwidget.h"
#include "backendeditorconstants.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/modemanager.h>
#include <projectexplorer/projectexplorerconstants.h>

#include <QCoreApplication>
#include <QDebug>

using namespace Core;
using namespace BackendEditor::Constants;

namespace BackendEditor {
namespace Internal {

BackendEditorFactory::BackendEditorFactory() : QmlJSEditor::QmlJSEditorFactory(Constants::BACKEND_EDITOR_ID)
{
    setId(BACKEND_EDITOR_ID);
    setDisplayName(QCoreApplication::translate("BackendEditor", BACKEND_EDITOR_NAME));
    addMimeType(BACKEND_CONFIG_MIME_TYPE);
    setEditorCreator([]() {
        ModeManager::activateMode(Constants::ID_MODE_BACKEND);
        return new QmlJSEditor::QmlJSEditor; // open an ordinary qml editor
    });
    FileIconProvider::registerIconOverlayForSuffix(ProjectExplorer::Constants::FILEOVERLAY_QML, "qml");
}

} // namespace Internal
} // namespace BackendEditor

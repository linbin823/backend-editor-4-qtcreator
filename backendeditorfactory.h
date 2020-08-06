#ifndef BACKENDEDITORFACTORY_H
#define BACKENDEDITORFACTORY_H

#include <coreplugin/editormanager/ieditorfactory.h>
#include <qmljseditor/qmljseditor.h>

namespace BackendEditor {
namespace Internal {

class BackendEditorFactory final : public QmlJSEditor::QmlJSEditorFactory
{
public:
    BackendEditorFactory();
};

} // namespace Internal
} // namespace Designer

#endif // BACKENDEDITORFACTORY_H

#ifndef FACTORY_H
#define FACTORY_H

#include <coreplugin/editormanager/ieditorfactory.h>
#include <qmljseditor/qmljseditor.h>

namespace BackendEditor {
namespace Internal {

class BEFactory final : public QmlJSEditor::QmlJSEditorFactory
{
public:
    BEFactory();
};

} // namespace Internal
} // namespace Designer

#endif // BACKENDEDITORFACTORY_H

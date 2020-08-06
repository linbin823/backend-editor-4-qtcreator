#ifndef MODEBACKEND_H
#define MODEBACKEND_H

#include <QObject>
#include <QWidget>
#include <coreplugin/imode.h>
#include <texteditor/autocompleter.h>


namespace BackendEditor {
namespace Internal {

class BackendEditorModeMainWidget;

class BackendEditorMode : public Core::IMode
{
    Q_OBJECT
public:
    BackendEditorMode(QObject *parent = nullptr);

    virtual ~BackendEditorMode();

private:
    BackendEditorModeMainWidget *mainWidget;
};

} // namespace Internal
} // namespace BackendEditor

#endif // MODEBACKEND_H

#ifndef MODEBACKEND_H
#define MODEBACKEND_H

#include <QObject>
#include <QWidget>
#include <coreplugin/imode.h>
#include <texteditor/autocompleter.h>
class ModeBackendMainWidget;
namespace BackendEditor {
class ModeBackend : public Core::IMode
{
    Q_OBJECT
public:
    ModeBackend(QObject *parent = nullptr);

    virtual ~ModeBackend();

private:
    ModeBackendMainWidget *mainWidget;
};
} // namespace BackendEditor
#endif // MODEBACKEND_H

#ifndef MODEBACKEND_H
#define MODEBACKEND_H

#include <QObject>
#include <QWidget>
#include <coreplugin/imode.h>
#include <texteditor/autocompleter.h>


namespace BackendEditor {
namespace Internal {

class ModeBackendMainWidget;

class ModeBackend : public Core::IMode
{
    Q_OBJECT
public:
    ModeBackend(QObject *parent = nullptr);

    virtual ~ModeBackend();

private:
    ModeBackendMainWidget *mainWidget;
};

} // namespace Internal
} // namespace BackendEditor

#endif // MODEBACKEND_H

#ifndef MODE_H
#define MODE_H

#include <QObject>
#include <QWidget>
#include <coreplugin/imode.h>
#include <texteditor/autocompleter.h>


namespace BackendEditor {
namespace Internal {

class ModeMainWidget;

class BEMode : public Core::IMode
{
    Q_OBJECT
public:
    BEMode(QObject *parent = nullptr);

    virtual ~BEMode();

private:
    ModeMainWidget *mainWidget;
};

} // namespace Internal
} // namespace BackendEditor

#endif // MODE_H

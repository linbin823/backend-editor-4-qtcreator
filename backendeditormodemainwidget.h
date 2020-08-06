#ifndef MODEBACKENDMAINWIDGET_H
#define MODEBACKENDMAINWIDGET_H

#include <QWidget>
#include <utils/id.h>

namespace Ui {
class BackendEditorModeMainWidget;
}// namespace Ui

namespace QmlDesigner {
class Model;
}// namespace QmlDesigner

namespace BackendEditor {
namespace Internal {

class BackendEditorModeMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackendEditorModeMainWidget(QWidget *parent = nullptr);
    ~BackendEditorModeMainWidget();

    void setActivePage(Utils::Id pageId);

public slots:
    void parseConfigFile(void);

private slots:
    void on_pbCreateTemplate_clicked();

private:
    Ui::BackendEditorModeMainWidget *ui;
    QmlDesigner::Model *_pDriversModel, *_pTagsModel;
};

} // namespace Internal
} // namespace BackendEditor

#endif // MODEBACKENDMAINWIDGET_H

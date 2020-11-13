#ifndef MODEBACKENDMAINWIDGET_H
#define MODEBACKENDMAINWIDGET_H

#include <QWidget>
#include <utils/id.h>

namespace Ui {
class ModeMainWidget;
}// namespace Ui

namespace QmlDesigner {
class Model;
}// namespace QmlDesigner

namespace BackendEditor {
namespace Internal {

class ModeMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModeMainWidget(QWidget *parent = nullptr);
    ~ModeMainWidget();

    void setActivePage(Utils::Id pageId);

public slots:
    void parseConfigFile(void);

private slots:
    void on_pbCreateTemplate_clicked();

private:
    Ui::ModeMainWidget *ui;
    QmlDesigner::Model *_pDriversModel, *_pTagsModel;
};

} // namespace Internal
} // namespace BackendEditor

#endif // MODEBACKENDMAINWIDGET_H

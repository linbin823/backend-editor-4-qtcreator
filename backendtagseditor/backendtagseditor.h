#ifndef MODEBACKENDTAGSEDITOR_H
#define MODEBACKENDTAGSEDITOR_H

#include "../backendeditorconstants.h"
#include <QWidget>
#include <QHideEvent>
#include "rtdatatablemodel.h"

namespace Ui {
class BackendTagsEditor;
}

namespace QmlDesigner {
class Model;
}// namespace QmlDesigner

namespace BackendEditor {

class BackendEditorModeMainWidget;

namespace Internal {

class BackendTagsEditor : public QWidget
{
    Q_OBJECT
public:

    explicit BackendTagsEditor(QAbstractTableModel *model, QWidget *parent = 0);
    virtual ~BackendTagsEditor();

private slots:
    void on_pbLoad_clicked();
    void on_pbSave_clicked();
    void on_pbDelete_clicked();
    void on_pbAdd_clicked();
    void updateDelegates();

    void on_cbInfo_toggled(bool checked);

    void on_cbScale_toggled(bool checked);

    void on_cbOnlineState_toggled(bool checked);

//    void on_pushButton_clicked();
private:
    Ui::BackendTagsEditor *ui;
    RTDataTableModel *_pModel;
//    PollGroupMgr* _pPollGroupMgr;
};

} // namespace Internal
} // namespace BackendEditor

#endif //MODEBACKENDTAGSEDITOR_H

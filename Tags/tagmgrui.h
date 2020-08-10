#ifndef TAGMGRUI_H
#define TAGMGRUI_H

#include <QWidget>
#include <QHideEvent>
#include "tagmgrtablemodel.h"

class PollGroupMgr;

namespace Ui {
class TagMgrUI;
}

class TagMgrUI : public QWidget
{
    Q_OBJECT

public:
    explicit TagMgrUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~TagMgrUI();

private slots:
    void on_pbAdd_clicked();
    void on_pbDelete_clicked();
    void on_pbLoad_clicked();
    void on_pbSave_clicked();

    void updateDelegates();

    void on_cbInfo_toggled(bool checked);
    void on_cbScale_toggled(bool checked);
    void on_cbOnlineState_toggled(bool checked);

//    void on_pushButton_clicked();

private:
    Ui::TagMgrUI *ui;
    TagMgrTableModel* _pModel;
//    PollGroupMgr* _pPollGroupMgr;
};

#endif // TAGMGRUI_H

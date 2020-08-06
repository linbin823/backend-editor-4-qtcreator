#ifndef RTDataUI_H
#define RTDataUI_H

#include <QWidget>
#include <QHideEvent>
#include "rtdatatablemodel.h"

class PollGroupMgr;

namespace Ui {
class RTDataUI;
}

namespace BackendEditor {
namespace Internal {

class RTDataUI : public QWidget
{
    Q_OBJECT

public:
    explicit RTDataUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~RTDataUI();

private slots:
    void on_pbLoad_clicked();
    void on_pbClose_clicked();
    void on_pbDelete_clicked();
    void on_pbAdd_clicked();
    void updateDelegates();

    void on_cbInfo_toggled(bool checked);

    void on_cbScale_toggled(bool checked);

    void on_cbOnlineState_toggled(bool checked);

//    void on_pushButton_clicked();
private:
    Ui::RTDataUI *ui;
    RTDataTableModel* _pModel;
//    PollGroupMgr* _pPollGroupMgr;
};

} // namespace Internal
} // namespace BackendEditor

#endif // RTDataUI_H

#ifndef POLLGROUPUI_H
#define POLLGROUPUI_H

#include <QWidget>
#include <QAbstractTableModel>

namespace Ui {
class PollGroupUI;
}

class PollGroupUI : public QWidget
{
    Q_OBJECT

public:
    explicit PollGroupUI(QAbstractTableModel *model, QWidget *parent = 0);
    ~PollGroupUI();

private slots:
    void on_pbAdd_clicked();
    void on_pbDelete_clicked();
    void on_pbClose_clicked();

private:
    Ui::PollGroupUI *ui;
    QAbstractTableModel *_pModel;
};

#endif // POLLGROUPUI_H

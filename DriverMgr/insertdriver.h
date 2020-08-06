#ifndef INSERTDRIVER_H
#define INSERTDRIVER_H

#include <QWidget>

namespace Ui {
class InsertDriver;
}

class InsertDriver : public QWidget
{
    Q_OBJECT

public:
    explicit InsertDriver(QWidget *parent = 0);
    ~InsertDriver();
    void setInsertPos(int pos);
private slots:
    void on_pbAdd_clicked();

    void on_pbClose_clicked();

    void on_lwSupportedDriverInfo_clicked(const QModelIndex &index);

private:
    Ui::InsertDriver *ui;
    int _pos;
};

#endif // INSERTDRIVER_H

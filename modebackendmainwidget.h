#ifndef MODEBACKENDMAINWIDGET_H
#define MODEBACKENDMAINWIDGET_H

#include <QDialog>

namespace Ui {
class ModeBackendMainWidget;
}

class ModeBackendMainWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ModeBackendMainWidget(QWidget *parent = nullptr);
    ~ModeBackendMainWidget();

private:
    Ui::ModeBackendMainWidget *ui;
};

#endif // MODEBACKENDMAINWIDGET_H

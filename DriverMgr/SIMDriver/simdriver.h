#ifndef SIMDRIVER_H
#define SIMDRIVER_H

#include <QObject>
#include <QTimer>
#include "iglobal.h"

class SimDriverUI;

class SimDriver : public iDriver
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.backendeditor.drivers.simDriver" FILE "simdriver.json")
    Q_INTERFACES(iDriver)
public:
    /*!
     * 构造函数
     * 功能描述：
     * 1、初始化参数
     * 2、进入停止状态
     */
    explicit SimDriver(QObject *parent = 0);
    ~SimDriver();

public slots:
    DriverInfo* driverInfo()const override;
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;
    int start() override;
    int stop() override;
    int restart() override;
    int state() override{ return _state; }
    QList<int> availableRWStrategy(void)const override;
    QList<int> availableRWStrategy(const QString& addr)const override;
    bool isAddressCorrect(const QString& addr)const override;
    QString addressErrorString(const QString& addr)const override;
    int setRelatedTagAddresses(const QList<TagAddress *>& relatedTagAddresses) override;
    QWidget *showUI(bool show=true, QWidget* parent=0) override;


public slots:

signals:

private slots:

private:
    int _state;
    /*!
     * For sorting
     * instead of operator<() to compare the TagInfo*
     */
    static bool compareTagAddress(const TagAddress* t1, const TagAddress* t2);

    SimDriverUI * _pUI;

    bool firstLoop = true;

    QList<TagAddress*> _relatedTags;
};

#endif // SIMDRIVER_H

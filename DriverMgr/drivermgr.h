#ifndef DRIVERMGR_H
#define DRIVERMGR_H
#include <QtPlugin>
#include <QPluginLoader>
#include <QDir>
#include <QThread>
#include <QAbstractTableModel>
#include "../Interface/iglobal.h"
#include "driverui.h"

class DriverMgr : public iDriverMgr
{
    Q_OBJECT
public:

    /*!
     * 单例模式
     * 功能描述：
     * 1、获取单例的指针
     */
    static DriverMgr* Instance();
    ~DriverMgr();

    /*!
     * \brief driversInit
     * clear all drivers info
     */
    void driversInit();
    /*!
     * \brief loadDrivers
     * load all extension drivers
     */
    void loadDrivers();
    /*!
     * \brief setAvailableDriver
     * append one driver loader to available driver list
     * \param pLoader
     * \return whether set is successful
     */
    bool setAvailableDriver(QPluginLoader *pLoader);
    /*!
     * \brief genDriverName
     * if testName is unique， return testName, else return a unique name
     * \param testName
     * \return
     */
    QString genDriverName(const QString& testName);
    /*!
     * \brief getSupportDriversInfo
     * get all supported drivers' infomation
     * \return
     */
    QList<DriverInfo> supportDriversInfo(void) const;

public slots:
    void startAll(void) override;
    void stopAll(void) override;
    QList<QObject*> driverList(void) override;
    QList<iDriver*> allDrivers(void) override;
    QStringList driverNames(void)override;
    QStringList driverNameRelateToType(const QString& driverType) override;
    void setDriverName(int pos, const QString& driverName) override;
    QObject* driver(const QString& driverName) override;
    bool contains(const QString& driverName) const override;
    int  setRelatedTags(const QList<iTagInfo *>& relatedTags) const override;
    QWidget* showUI(bool show=true, QWidget* parent = 0) override;
    bool insertDriver(int pos, const DriverInfo& info, const QString& driverName=QString()) override;
    bool removeDriver(const QString& driverName) override;
    bool removeDriver(int pos) override;
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;

private:
    /*!
     * 构造函数
     * 输入参数：无
     * 返回数值：无
     * 功能描述：
     * 1、测点初始化
     */
    explicit DriverMgr(QObject *parent = 0);
    static DriverMgr* _singleton;

    QDir _pluginsDir;
    QMap<QString, iDriver*> _drivers;
    QList<QThread*> _threads;

    QList<DriverInfo> _supportedDriversInfo;
    QList<QPluginLoader*> _supportedDriversLoader;

    QAbstractTableModel* _pModel;
    DriverUI* _pUI;
};

#endif // DRIVERMGR_H

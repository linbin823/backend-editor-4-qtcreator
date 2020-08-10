#ifndef DRIVERMGR_H
#define DRIVERMGR_H
#include <QDir>
#include "../Tags/tag.h"
#include "iloadsave.h"

class QAbstractTableModel;
class DriverConfigUI;
class QPluginLoader;
class DriverInfo;
class iDriverConfig;
class iDriverRuntime;

class DriverConfigMgr: public QObject, public iLoadSave
{
    Q_OBJECT
    Q_PROPERTY(QList<iDriverConfig*> driverList READ driverList NOTIFY driverListChanged)
public:
    /*!
     * 单例模式
     * 功能描述：
     * 1、获取单例的指针
     */
    static DriverConfigMgr* Instance();
    ~DriverConfigMgr();

    /*!
     * \brief initalize
     * clear all drivers info
     */
    void initalize();

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

signals:
    void driverListChanged();

public slots:
    /*!
     * \brief driverList
     * \return all drivers
     */
    QList<iDriverConfig *> driverList(void) const;

    /*!
     * \brief contains
     * \param driverName
     * \return true = driverName exists
     */
    bool contains(const QString& driverName) const;
    /*!
     * \brief showUI
     * \param show: true = show, false = hide
     * \param parent widget
     * \result return the UI QWidget*.
     */
    QWidget* showUI(bool show = true, QWidget* parent = 0);
    /*!
     * \brief names
     * \return all drivers' names
     */
    QStringList driverNames(void) const;
    QStringList driverNameRelateToType(const QString& driverType) const;
    /*!
     * \brief setDriverName
     * \param pos
     * \param driverName
     */
    void setDriverName(int pos, const QString& driverName);
    /*!
     * \brief driver
     * return a certain driver
     * \param name
     * \return
     */
    iDriverConfig* driver(const QString& driverName) const;
    /*!
     * \brief insertDriver
     * insert a certain type driver into certain pos with given driverName
     * \param pos
     * \param info
     * \param driverName
     * \return
     */
    bool insertDriver(int pos, const DriverInfo& info, const QString& driverName=QString());
    bool removeDriver(const QString& driverName);
    bool removeDriver(int pos);

    //implement of iLoadSave
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
    explicit DriverConfigMgr(QObject *parent = 0);
    static DriverConfigMgr* _singleton;

    QDir _pluginsDir;
    QMap<QString, iDriverConfig*> _drivers;

    QList<DriverInfo> _supportedDriversInfo;
    QList<QPluginLoader*> _supportedDriversLoader;

    QAbstractTableModel* _pModel;
    DriverConfigUI* _pUI;
};

class iDriverRuntimeMgr
{
public:
    explicit iDriverRuntimeMgr() {}
    virtual ~iDriverRuntimeMgr() {}

    /*!
     * \brief startAll
     * start all drivers
     */
    virtual void startAll(void)=0;
    /*!
     * \brief stopAll
     * stop all drivers
     */
    virtual void stopAll(void)=0;
    /*!
     * \brief allDrivers
     *  For C++ usage
     * \return all drivers
     */
    virtual QList<iDriverRuntime* > allDrivers(void)=0;
    /*!
     * \brief names
     * \return all drivers' names
     */
    virtual QStringList driverNames(void)=0;
    virtual QStringList driverNameRelateToType(const QString& driverType)=0;
    /*!
     * \brief driver
     * return a certain driver
     * \param name
     * \return
     */
    virtual iDriverRuntime* driver(const QString& driverName)=0;
    /*!
     * \brief 设置测点
     * 当测点表有更新，更新地址列表
     * must connect RTData's signal to this slots
     * \return 返回数值：Int 输入的测点数
     */
    virtual int setRelatedTags(const QList<Tag *>& relatedTags)const=0;

private:
    QList<QThread*> _threads;
};

#endif // DRIVERMGR_H

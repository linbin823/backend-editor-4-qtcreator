#ifndef IDRIVER_H
#define IDRIVER_H
#include <QObject>
#include "../RTData/itaginfo.h"
#include "iloadsave.h"

class DriverInfo;

class iDriver : public QObject, public iLoadSave
{
    Q_OBJECT
    Q_PROPERTY(int state  READ state  NOTIFY stateChanged)
    Q_PROPERTY(DriverInfo* driverInfo READ driverInfo WRITE setDriverInfo)

public:
    /*!
     * 构造函数
     * 功能描述：
     * 1、初始化参数
     * 2、进入停止状态
     */
    explicit iDriver(QObject *parent = 0):QObject(parent),_pInfo(nullptr){}
    /*!
     * 构造函数
     * 新增初始化DriverInfo
     */
    iDriver(DriverInfo* info, QObject *parent = 0):QObject(parent),_pInfo(info){}
    virtual ~iDriver(){}

    enum enum_states{
        STAT_ABOUT_TO_RUN,
        STAT_RUN,
        STAT_ABOUT_TO_STOP,
        STAT_STOP,
        STAT_ERR
    };
    static QString enumStatesString(int states) {
        switch(states){
        case STAT_ABOUT_TO_RUN:
            return tr("启动中.");
        case STAT_RUN:
            return tr("运行.");
        case STAT_ABOUT_TO_STOP:
            return tr("停止中.");
        case STAT_STOP:
            return tr("停止.");
        case STAT_ERR:
            return tr("故障.");
        default:
            return QString();
        }
    }

public slots:
    /*!
     * \brief getDriverInfo
     * get driverInfo
     * \return driverInfo
     */
    virtual DriverInfo* driverInfo()const {
        return _pInfo;
    }
    /*!
     * \brief setDriverInfo
     * \param pInfo setThisDriver's info when started
     */
    virtual void setDriverInfo(DriverInfo* pInfo) {
        if (pInfo) {
            _pInfo = pInfo;
        }
    }
    /*!
     * \brief start
     * 启动驱动
     * 返回数值：0：成功    其他：失败
     * \return
     */
    virtual int start()=0;
    virtual int stop()=0;
    virtual int restart()=0;
    virtual int state()=0;

    /*!
     * \brief isAddressCorrect
     * 判断测点地址是否正确
     * \param addr 测点地址
     * \return true=correct
     */
    virtual bool isAddressCorrect(const QString& addr)const=0;
    /*!
     * \brief availableRWStrategy
     * get all available RWStrategy for this driver.
     * \return
     */
    virtual QList<int> availableRWStrategy(void)const=0;
    /*!
     * \brief availableRWStrategy
     * get available RWStrategy related to a certain address
     * \param addr
     * \return
     */
    virtual QList<int> availableRWStrategy(const QString& addr)const=0;
    /*!
     * \brief addressErrorString
     * 测点地址错误提示
     * \param addr 测点地址
     * \return return QString() means no error
     * else shows the reason of the invalid address.
     */
    virtual QString addressErrorString(const QString& addr)const=0;

    /*!
     * \brief setRelatedTagAddresses
     * 设置驱动相关的测点
     * \return 返回数值：Int 输入的测点数
     */
    virtual int setRelatedTagAddresses(const QList<TagAddress *>& relatedTagAddresses)=0;
    /*!
     * 显示管理页面
     */
    virtual void showUI(bool show=true, QWidget* parent=0)=0;

signals:
    void stateChanged(int);
    void msgEventString(const QString& msg);

private:
    DriverInfo* _pInfo;
};

class DriverInfo : public iLoadSave{
public:
    QString driverType;
    QString driverManufacturer;
    QString description;
    QString majorVersion;
    QString minorVersion;
    QString microVersion;
    void save(iLoadSaveProcessor *processor){
        processor->writeValue("driverType", driverType);
        processor->writeValue("driverManufacturer", driverManufacturer);
        processor->writeValue("majorVersion", majorVersion);
        processor->writeValue("minorVersion", minorVersion);
        processor->writeValue("microVersion", microVersion);
    }
    void load(iLoadSaveProcessor *processor){
        processor->readValue("driverType", driverType);
        processor->readValue("driverManufacturer", driverManufacturer);
        processor->readValue("majorVersion", majorVersion);
        processor->readValue("minorVersion", minorVersion);
        processor->readValue("microVersion", microVersion);
    }

    bool operator==(const DriverInfo& other){
        if(driverType != other.driverType) return false;
        if(majorVersion.toInt() != other.majorVersion.toInt()) return false;
        if(minorVersion.toInt() != other.minorVersion.toInt()) return false;
        if(microVersion.toInt() != other.microVersion.toInt()) return false;
        return true;
    }
};

Q_DECLARE_INTERFACE(iDriver, "iDriver_iid")

#endif // IDRIVER_H

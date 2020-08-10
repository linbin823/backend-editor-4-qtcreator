#ifndef TAG_H
#define TAG_H
#include <QVariant>
#include <QDateTime>
#include <QDataStream>
#include <QTimer>
#include "iloadsave.h"

class QReadWriteLock;
class TagAddress;

class Tag : public QObject, public iLoadSave
{
    Q_OBJECT
    Q_PROPERTY(QString   name           READ name           NOTIFY nameChanged)
    Q_PROPERTY(QString   description    READ description    NOTIFY descriptionChanged)
    Q_PROPERTY(QString   extraInfo      READ extraInfo      NOTIFY extraInfoChanged)
    Q_PROPERTY(enumTypeCode type        READ type           WRITE  setType        NOTIFY typeChanged)
    Q_PROPERTY(QVariant  value          READ value          WRITE  setvalue       NOTIFY valueChanged)
    Q_PROPERTY(QString   unit           READ unit           NOTIFY unitChanged)
//    Q_PROPERTY(int       projectID      READ projectID      NOTIFY projectIDChanged)
    Q_PROPERTY(QString   projectName    READ projectName    NOTIFY projectNameChanged)
//    Q_PROPERTY(int       stationID      READ stationID      NOTIFY stationIDChanged)
    Q_PROPERTY(QString   stationName    READ stationName    NOTIFY stationNameChanged)
//    Q_PROPERTY(int       moduleID       READ moduleID       NOTIFY moduleIDChanged)
//    Q_PROPERTY(int       pointID        READ pointID        NOTIFY pointIDChanged)
    Q_PROPERTY(QString   systemName     READ systemName     NOTIFY systemNameChanged)
    Q_PROPERTY(QDateTime lastUpdateTime READ lastUpdateTime NOTIFY lastUpdateTimeChanged)
//    Q_PROPERTY(int       qualityCode    READ qualityCode    NOTIFY qualityCodeChanged)
    Q_PROPERTY(float     scaleRatio     READ scaleRatio     NOTIFY scaleRatioChanged)
    Q_PROPERTY(float     scaleOffset    READ scaleOffset    NOTIFY scaleOffsetChanged)

public:
    explicit Tag(const QString& name, QObject *parent = 0);
    Tag(const Tag& other);
    virtual ~Tag();

    enum enumRWStrategyCode{
        RWS_DISABLE = 0x00,
        RWS_READ_ONLY,
        RWS_WRITE_ONLY,
        RWS_READ_WRITE
    };
    static QString enumRWStrategyString(enumRWStrategyCode RWCode){
        switch(RWCode){
        case RWS_DISABLE:
            return tr("禁用.");
        case RWS_READ_ONLY:
            return tr("只读.");
        case RWS_WRITE_ONLY:
            return tr("只写.");
        case RWS_READ_WRITE:
            return tr("读写.");
        default:
            return QString();
        }
    }
    static enumRWStrategyCode enumRWStrategyValue(const QString& string){
        if(string == enumRWStrategyString(RWS_DISABLE)){
            return RWS_DISABLE;
        }else if(string == enumRWStrategyString(RWS_READ_ONLY)){
            return RWS_READ_ONLY;
        }else if(string == enumRWStrategyString(RWS_WRITE_ONLY)){
            return RWS_WRITE_ONLY;
        }else if(string == enumRWStrategyString(RWS_READ_WRITE)){
            return RWS_READ_WRITE;
        }else return RWS_DISABLE;
    }
    static QStringList enumRWStrategyStrings(){
        QStringList ret;
        ret<<enumRWStrategyString(RWS_DISABLE)<<enumRWStrategyString(RWS_READ_ONLY);
        ret<<enumRWStrategyString(RWS_WRITE_ONLY)<<enumRWStrategyString(RWS_READ_WRITE);
        return ret;
    }

    enum enumTypeCode{
        TYPE_DEFAULT = 0x00,
        TYPE_BOOL,
        TYPE_INT16,
        TYPE_INT32,
        TYPE_INT64,
        TYPE_UINT16,
        TYPE_UINT32,
        TYPE_UINT64,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_DATETIME,
        TYPE_TIME,
        TYPE_DATE,
        TYPE_UNKNOWN
    };
    static QString enumTypeString(enumTypeCode typeCode) {
        switch(typeCode){
        case TYPE_DEFAULT:
            return tr("Default");
        case TYPE_BOOL:
            return tr("BOOL");
        case TYPE_INT16:
            return tr("INT16");
        case TYPE_INT32:
            return tr("INT32");
        case TYPE_INT64:
            return tr("INT64");
        case TYPE_UINT16:
            return tr("UINT16");
        case TYPE_UINT32:
            return tr("UINT32");
        case TYPE_UINT64:
            return tr("UINT64");
        case TYPE_FLOAT:
            return tr("FLOAT");
        case TYPE_DOUBLE:
            return tr("DOUBLE");
        case TYPE_STRING:
            return tr("STRING");
        case TYPE_DATETIME:
            return tr("DATETIME");
        case TYPE_TIME:
            return tr("TIME");
        case TYPE_DATE:
            return tr("DATE");
        default:
            return tr("未知类型.");
        }
    }
    static enumTypeCode enumTypeValue(const QString& string){
        if(string == enumTypeString(TYPE_DEFAULT)){
            return TYPE_DEFAULT;
        }else if(string == enumTypeString(TYPE_BOOL)){
            return TYPE_BOOL;
        }else if(string == enumTypeString(TYPE_INT16)){
            return TYPE_INT16;
        }else if(string == enumTypeString(TYPE_INT32)){
            return TYPE_INT32;
        }else if(string == enumTypeString(TYPE_INT64)){
            return TYPE_INT64;
        }else if(string == enumTypeString(TYPE_UINT16)){
            return TYPE_UINT16;
        }else if(string == enumTypeString(TYPE_UINT32)){
            return TYPE_UINT32;
        }else if(string == enumTypeString(TYPE_UINT64)){
            return TYPE_UINT64;
        }else if(string == enumTypeString(TYPE_FLOAT)){
            return TYPE_FLOAT;
        }else if(string == enumTypeString(TYPE_DOUBLE)){
            return TYPE_DOUBLE;
        }else if(string == enumTypeString(TYPE_STRING)){
            return TYPE_STRING;
        }else if(string == enumTypeString(TYPE_DATETIME)){
            return TYPE_DATETIME;
        }else if(string == enumTypeString(TYPE_TIME)){
            return TYPE_TIME;
        }else if(string == enumTypeString(TYPE_DATE)){
            return TYPE_DATE;
        }else return TYPE_UNKNOWN;
    }
    static QStringList enumTypeStrings(){
        QStringList ret;
        ret<<enumTypeString(TYPE_DEFAULT)<<enumTypeString(TYPE_BOOL);
        ret<<enumTypeString(TYPE_INT16)<<enumTypeString(TYPE_INT32)<<enumTypeString(TYPE_INT64);
        ret<<enumTypeString(TYPE_UINT16)<<enumTypeString(TYPE_UINT32)<<enumTypeString(TYPE_UINT64);
        ret<<enumTypeString(TYPE_FLOAT)<<enumTypeString(TYPE_DOUBLE);
        ret<<enumTypeString(TYPE_STRING)<<enumTypeString(TYPE_DATETIME);
        ret<<enumTypeString(TYPE_TIME)<<enumTypeString(TYPE_DATE);
        return ret;
    }

    QString extraInfo()const;
    void setExtraInfo(const QString& extraInfo);
    QString description()const;
    void setDescription(const QString& description);
    enumTypeCode type()const;
    void setType(enumTypeCode type);

    /*!
     * \brief 读测点值
     * 输入参数：无
     * 返回数值：
     * QVariant value
     * 失败返回 null
     * 功能描述：
     * 1、执行的时候，value要进行工程量变换。。。
     */
    QVariant value()const;
    //setValue & updateValue is blow these basic functions

    QString name()const;
    void setName(const QString& name);
    QString unit()const;
    void setUnit(const QString& newUnit);
//    int projectID()const;
//    void setProjectID(int id);
    QString projectName()const;
    void setProjectName(const QString& newProjectName);
//    int stationID()const;
//    void setStationID(int id);
    QString stationName()const;
    void setStationName(const QString& newStationName);
//    int moduleID()const;
//    void setModuleID(int id);
//    int pointID()const;
//    void setPointID(int id);
    QString systemName()const;
    void setSystemName(const QString& newSystemName);
    float scaleRatio() const;
    void setScaleRatio(float ratio);
    float scaleOffset() const;
    void setScaleOffset(float offset);

    /*!
     * \brief allAddresses
     * get all address structure related to this tagInfo
     * \return
     */
    inline QList<TagAddress*> allAddresses() const { return addresses; }

    /*!
     * \brief unscaledValue
     * get unscaled value
     * for drivers who need
     * \return
     */
    QVariant unscaledValue() const;

    /*!
     * \brief 写测点值
     * 输入参数：QVariant value
     * 返回数值：
     * 0：成功
     * 其他：失败
     * 功能描述：
     * 1、专门给上位软件模块用的方法
     * 2、执行的时候，value要进行工程量变换。。。
     * 3、最后更新时间:without changing updateTime.
     */
    int setvalue(const QVariant &value);//for HMI

    /*!
     * \brief 写测点值
     * 输入参数：
     * 1、QVariant value
     * 2、QDateTime time
     * 返回数值：
     * 0：成功
     * 其他：失败
     * 功能描述：
     * 1、专门给数据驱动模块用的方法
     * 2、执行的时候，value 不需要 执行阶码缩放、工程量变换。。。
     * 3、最后更新时间按照传入参数执行更新。
     */
    int updateValue(const QVariant &value, const QDateTime &time = QDateTime::currentDateTime() );//for driver

    /*!
     * \brief set Tag's Read Write Strategy. Options include disabel/Read only/Write only/readWrite
     * \param RWS is read write strategy
     */
    void setRWStrategy(enumRWStrategyCode RWS, const QString& driverName);
    enumRWStrategyCode RWStrategy(const QString& driverName)const;

    /*!
     * \brief address
     * \param driverName
     * \return address belongs to a driver
     */
    QString address(const QString& driverName)const;
    void setAddress(const QString& address, const QString& driverName);

    /*!
     * \brief isAddressCorrect
     * \param driverName
     * \return whether the address is correct
     */
    bool isAddressCorrect(const QString& driverName)const;

    /*!
     * \brief allAvailableRWStrategy
     * get all available RWStrategy for this driver.
     * \return
     */
    QList<enumRWStrategyCode> allAvailableRWStrategy(const QString& driverName)const;

    /*!
     * \brief availableRWStrategy
     * always update available RWSStrategy after change address.
     * \param driverName
     * \return
     */
    QList<enumRWStrategyCode> availableRWStrategy(const QString& driverName)const;

    /*!
     * \brief addressErrorString
     * \param driverName
     * \return the address error string.
     * if correct, return QString()
     */
    QString addressErrorString(const QString& driverName)const;

    QDateTime lastUpdateTime() const;

    //implement of iLoadSave
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;

//    QString pollGroupName() const override;
//    void setPollGroupName(const QString& newPollGroupName);
//    bool needRead()const override;

    //    friend QDataStream& operator<<(QDataStream& out, const Tag& t);
    //    friend QDataStream& operator>>(QDataStream& in, Tag& t);
    friend bool operator< (const Tag& t1, const Tag& t2);
    friend bool operator> (const Tag& t1, const Tag& t2);
    friend bool operator==(const Tag& t1, const Tag& t2);
    friend bool operator>=(const Tag& t1, const Tag& t2);
    friend bool operator<=(const Tag& t1, const Tag& t2);
    Tag& operator= (const Tag& other);

public slots:
    /*!
     * \brief pollValue
     * pollValue only used by pollGroup
     * periodically poll value.
     * poll include:
     *  set needRead. One Tag has only one "needRead"
     *  set needWrite. Each tagAddress has one "needWrite"
     *  That means, one tag can write to several controllers(with different drivers)
     *  but only one drive can read and update the tag's value
     */
//    void pollValue();

signals:
    void nameChanged();
    void descriptionChanged();
    void extraInfoChanged();
    void typeChanged();
    void valueChanged();
    void unitChanged();
//    void projectIDChanged();
    void projectNameChanged();
//    void stationIDChanged();
    void stationNameChanged();
//    void moduleIDChanged();
//    void pointIDChanged();
    void systemNameChanged();
    void lastUpdateTimeChanged();
//    void pollGroupNameChanged();
    void scaleRatioChanged();
    void scaleOffsetChanged();
    void addressChanged();
    void RWStrategyChanged();

private:
    /*!
     * \brief _setNeedToWrite
     * \param excludeDriverName
     * Set each needToWrite of drivers to true
     */
    void _setNeedToWrite();
    /*!
     * \brief _setValue
     * for updateValue and setValue
     * set new value, emit basic signals
     * if needScale is true, then do the scale. Otherwise, skip it
     * \return
     * 1 means ok but new value equals origin value
     * 0 means ok and value updated
     * -1 means failed
     */
    int _setValue(const QVariant &value, bool needScale = false);
public:
    QList<TagAddress *> addresses;
private:
//    bool _needToRead;
//    QString _pollGroupName;
    enumTypeCode _type;
//    int _projectID,_stationID,_moduleID,_pointID;
    QString _description,_extraInfo, _name, _unit, _projectName, _stationName, _systemName;
    float _ratio, _offset;
    QVariant _value;
    QDateTime _lastUpdateTime;
    QReadWriteLock *_pLock;
};

class TagAddress: public iLoadSave
{
public:
    bool    needToWrite;
    QString address;
    QString driverName;
    Tag* tag;
    Tag::enumRWStrategyCode RWStrategy;
    void save(iLoadSaveProcessor *processor){
        processor->writeValue("address",address);
        processor->writeValue("driverName",driverName);
        processor->writeValue("RWStrategy",RWStrategy);
    }
    void load(iLoadSaveProcessor *processor){
        processor->readValue("address",address);
        processor->readValue("driverName",driverName);
        processor->readValue("RWStrategy",RWStrategy);
    }
};

#endif // TAG_H

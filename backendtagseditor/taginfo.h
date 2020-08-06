#ifndef TAGINFO_H
#define TAGINFO_H

#include <QVariant>
#include <QDateTime>
#include <QDataStream>
#include <QTimer>
#include "Interface/iglobal.h"

class QReadWriteLock;

namespace BackendEditor {
namespace Internal {

class TagInfo : public iTagInfo
{
    Q_OBJECT

public:
    explicit TagInfo(const QString& name, QObject *parent = 0);

    QString extraInfo()const override;
    void setExtraInfo(const QString& extraInfo);
    QString description()const override;
    void setDescription(const QString& description);
    int type()const override;
    void setType(int type);
    QVariant value()const override;
    //setValue & updateValue is blow these basic functions
    QString name()const override;
    void setName(const QString& name);
    QString unit()const override;
    void setUnit(const QString& newUnit);
    int projectID()const override;
    void setProjectID(int id);
    QString projectName()const override;
    void setProjectName(const QString& newProjectName);
    int stationID()const override;
    void setStationID(int id);
    QString stationName()const override;
    void setStationName(const QString& newStationName);
    int moduleID()const override;
    void setModuleID(int id);
    int pointID()const override;
    void setPointID(int id);
    QString systemName()const override;
    void setSystemName(const QString& newSystemName);
//    QString pollGroupName() const override;
//    void setPollGroupName(const QString& newPollGroupName);
    float scaleRatio() const override;
    void setScaleRatio(float ratio);
    float scaleOffset() const override;
    void setScaleOffset(float offset);

    inline QList<TagAddress*> allAddresses() const override{return addresses;}

    QVariant unscaledValue() const override;
    int setvalue(const QVariant &value) override;//for HMI
    int updatevalue(const QVariant &value, const QDateTime &time = QDateTime::currentDateTime() ) override;//for driver

    void setRWStrategy(int RWS, const QString& driverName) override;
    int RWStrategy(const QString& driverName)const override;

    QString address(const QString& driverName)const override;
    void setAddress(const QString& address, const QString& driverName)override;

    bool isAddressCorrect(const QString& driverName)const override;
    QList<int> availableRWStrategy(const QString& driverName)const override;
    QList<int> allAvailableRWStrategy(const QString& driverName)const override;
    QString addressErrorString(const QString& driverName)const override;

//    bool needRead()const override;

public slots:
//    void pollValue()override;

public:
    //implement of iLoadSave
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;


public:
    QDateTime lastUpdateTime() const override;

//    friend QDataStream& operator<<(QDataStream& out, const TagInfo& t);
//    friend QDataStream& operator>>(QDataStream& in, TagInfo& t);
    friend bool operator< (const TagInfo& t1, const TagInfo& t2);
    friend bool operator> (const TagInfo& t1, const TagInfo& t2);
    friend bool operator==(const TagInfo& t1, const TagInfo& t2);
    friend bool operator>=(const TagInfo& t1, const TagInfo& t2);
    friend bool operator<=(const TagInfo& t1, const TagInfo& t2);
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
    QList<TagAddress*> addresses;
private:
//    bool _needToRead;
//    QString _pollGroupName;
    int _type,_projectID,_stationID,_moduleID,_pointID;
    QString _description,_extraInfo, _name, _unit, _projectName, _stationName, _systemName;
    float _ratio, _offset;
    QVariant _value;
    QDateTime _lastUpdateTime;
    QReadWriteLock *_pLock;
};

} // namespace Internal
} // namespace BackendEditor

#endif // TAGINFO_H

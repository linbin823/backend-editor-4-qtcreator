#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "taginfo.h"
#include "drivermgr.h"
#include <QMutexLocker>
#include <QReadWriteLock>

//    QList<TagAddress> addresses;
//    bool _needToRead;
//    QString _pollGroupName;
//    int _type,_projectID,_stationID,_moduleID,_pointID;
//    QString _description,_extraInfo, _name, _unit, _projectName, _stationName, _systemName;
//    float _ratio, _offset;
//    QVariant _value;
//    QDateTime _lastUpdateTime;
static DriverMgr *_pDriverMgr = nullptr;

TagInfo::TagInfo(const QString &name, QObject *parent)
    :iTagInfo(parent)
{
    if(_pDriverMgr == nullptr) {
        _pDriverMgr = DriverMgr::Instance();
    }
    _pLock = new QReadWriteLock(QReadWriteLock::NonRecursive);
    addresses.clear();
//    _needToRead = false;
//    setPollGroupName(QString());
    setType(TYPE_DEFAULT);
    setProjectID(0);
    setStationID(0);
    setModuleID(0);
    setPointID(0);

    setDescription(QString());
    setExtraInfo(QString());
    setName( name );
    setUnit( QString() );
    setProjectName( QString() );
    setStationName( QString() );
    setSystemName( QString() );

    setScaleRatio(1);
    setScaleOffset(0);

    setvalue( QVariant(0) );
}

QString TagInfo::extraInfo()const{
    return _extraInfo;
}

void TagInfo::setExtraInfo(const QString& extraInfo){
    if(_extraInfo != extraInfo){
        _extraInfo = extraInfo;
        emit extraInfoChanged();
    }
}

QString TagInfo::description()const{
    return _description;
}
void TagInfo::setDescription(const QString& description){
    if(_description != description){
        _description = description;
        emit descriptionChanged();
    }
}

int TagInfo::type()const{
    return _type;
}

void TagInfo::setType(int type){
    if(_type != type){
        _type = type;
        emit typeChanged();
    }
}

QString TagInfo::name()const{
    return _name;
}

void TagInfo::setName(const QString& name){
    if(_name != name){
        _name = name;
        emit nameChanged();
    }
}

QString TagInfo::unit()const{
    return _unit;
}

void TagInfo::setUnit(const QString& newUnit){
    if(_unit != newUnit){
        _unit = newUnit;
        emit unitChanged();
    }
}

int TagInfo::projectID()const{
    return _projectID;
}

void TagInfo::setProjectID(int id){
    if(_projectID!=id){
        _projectID = id;
        emit projectIDChanged();
    }
}

QString TagInfo::projectName()const{
    return _projectName;
}

void TagInfo::setProjectName(const QString& newProjectName){
    if(_projectName != newProjectName){
        _projectName = newProjectName;
        emit projectNameChanged();
    }
}

int TagInfo::stationID()const{
    return _stationID;
}

void TagInfo::setStationID(int id){
    if(_stationID != id){
        _stationID = id;
        emit stationIDChanged();
    }
}

QString TagInfo::stationName()const{
    return _stationName;
}

void TagInfo::setStationName(const QString& newStationName){
    if(_stationName != newStationName){
        _stationName = newStationName;
        emit stationNameChanged();
    }
}

int TagInfo::moduleID()const{
    return _moduleID;
}
void TagInfo::setModuleID(int id){
    if(_moduleID != id){
        _moduleID = id;
        emit moduleIDChanged();
    }
}

int TagInfo::pointID()const{
    return _pointID;
}
void TagInfo::setPointID(int id){
    if(_pointID != id){
        _pointID = id;
        emit pointIDChanged();
    }
}

QString TagInfo::systemName()const{
    return _systemName;
}
void TagInfo::setSystemName(const QString& newSystemName){
    if(_systemName != newSystemName){
        _systemName = newSystemName;
        emit systemNameChanged();
    }
}

//QString TagInfo::pollGroupName() const{
//    return _pollGroupName;
//}

//void TagInfo::setPollGroupName(const QString& newPollGroupName){
//    if(_pollGroupName != newPollGroupName){
//        _pollGroupName = newPollGroupName;
//        emit pollGroupNameChanged();
//    }
//}

float TagInfo::scaleRatio() const{
    return _ratio;
}

void TagInfo::setScaleRatio(float ratio){
    if(ratio!=0 && _ratio!=ratio){
        _ratio = ratio;
        emit scaleRatioChanged();
    }
}

float TagInfo::scaleOffset() const{
    return _offset;
}

void TagInfo::setScaleOffset(float offset){
    if(_offset!=offset){
        _offset = offset;
        emit scaleOffsetChanged();
    }
}

QVariant TagInfo::value()const{
    _pLock->lockForRead();
    QVariant ret(_value);
    _pLock->unlock();
    return ret;
}

QVariant TagInfo::unscaledValue() const{
    _pLock->lockForRead();
    QVariant ret;
    switch (_type) {
    case TYPE_DEFAULT:
        if(_value.isValid()){
            ret = _value;
        }
        break;
    case TYPE_BOOL:
        if(_value.canConvert( QMetaType::Bool ) ){
            ret = _value;
        }
        break;
    case TYPE_DATE:
        if(_value.canConvert( QMetaType::QDate ) ){
            ret = _value;
        }
        break;
    case TYPE_TIME:
        if(_value.canConvert( QMetaType::QTime)){
            ret = _value;
        }
        break;
    case TYPE_DATETIME:
        if(_value.canConvert( QMetaType::QDateTime)){
            ret = _value;
        }
        break;
    case TYPE_STRING:
        if(_value.canConvert( QMetaType::QString)){
            ret = _value;
        }
        break;
    case TYPE_DOUBLE:
        if(_value.canConvert( QMetaType::Double)){
            double temp = (_value.toDouble() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_FLOAT:
        if(_value.canConvert( QMetaType::Float)){
            float temp = (_value.toFloat() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_INT16:
        if(_value.canConvert( QMetaType::Int)){
            qint16 temp = (_value.toInt() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_INT32:
        if(_value.canConvert( QMetaType::Int)){
            qint32 temp = (_value.toInt() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_INT64:
        if(_value.canConvert( QMetaType::LongLong)){
            qint64 temp = (_value.toLongLong() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_UINT16:
        if(_value.canConvert( QMetaType::UInt)){
            quint16 temp =(_value.toUInt() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_UINT32:
        if(_value.canConvert( QMetaType::UInt)){
            quint32 temp = (_value.toUInt() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    case TYPE_UINT64:
        if(_value.canConvert( QMetaType::ULongLong)){
            quint64 temp = (_value.toULongLong() - _offset) / _ratio;
            ret = QVariant(temp);
        }
        break;
    default:
        break;
    }
    _pLock->unlock();
    return ret;
}

int TagInfo::setvalue(const QVariant& value){
    _pLock->lockForWrite();
    int ret = _setValue(value,false);
    if(ret == 0){
        _setNeedToWrite();
    }
    _pLock->unlock();
    return ret;
}

int TagInfo::updatevalue(const QVariant& value, const QDateTime& time){
    _pLock->lockForWrite();
    int ret = _setValue(value,true);
    if(ret>=0){
        _setNeedToWrite();
        if(!time.isValid()){
            _lastUpdateTime = QDateTime::currentDateTime();
        }else
            _lastUpdateTime = time;
        emit lastUpdateTimeChanged();
    }
    _pLock->unlock();
    return ret;
}

int TagInfo::_setValue(const QVariant &value, bool needScale){
    switch (_type) {
    case TYPE_DEFAULT:
        if( value.isValid()){
            if(_value != value){
                _value = value;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_BOOL:
        if( value.canConvert( QMetaType::Bool ) ){
            if(_value != value){
                _value = value;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_DATE:
        if( value.canConvert( QMetaType::QDate ) ){
            if(_value != value){
                _value = value;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_TIME:
        if( value.canConvert( QMetaType::QTime)){
            if(_value != value){
                _value = value;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_DATETIME:
        if( value.canConvert( QMetaType::QDateTime)){
            if(_value != value){
                _value = value;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_STRING:
        if( value.canConvert( QMetaType::QString)){
            if(_value != value){
                _value = value;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_DOUBLE:
        if( value.canConvert( QMetaType::Double)){
            QVariant temp;
            if(needScale) temp = QVariant(value.toDouble() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_FLOAT:
        if( value.canConvert( QMetaType::Float)){
            QVariant temp;
            if(needScale) temp = QVariant(value.toFloat() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_INT16:
        if( value.canConvert( QMetaType::Int)){
            QVariant temp;
            if(needScale) temp =  QVariant(value.toInt() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_INT32:
        if( value.canConvert( QMetaType::Int)){
            QVariant temp;
            if(needScale) temp =  QVariant(value.toInt() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_INT64:
        if( value.canConvert( QMetaType::LongLong)){
            QVariant temp;
            if(needScale) temp =  QVariant(value.toLongLong() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_UINT16:
        if( value.canConvert( QMetaType::UInt)){
            QVariant temp;
            if(needScale) temp =  QVariant(value.toUInt() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_UINT32:
        if( value.canConvert( QMetaType::UInt)){
            QVariant temp;
            if(needScale) temp =  QVariant(value.toUInt() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    case TYPE_UINT64:
        if( value.canConvert( QMetaType::ULongLong)){
            QVariant temp;
            if(needScale) temp =  QVariant(value.toULongLong() * _ratio + _offset);
            else temp = value;
            if( _value != temp ){
                _value = temp;
                emit valueChanged();
                return 0;
            }
            return 1;
        }
        return -1;
    default:
        return -1;
    }
}

void TagInfo::setRWStrategy(int RWS, const QString &driverName){
    foreach (TagAddress* addr, addresses) {
        if(addr==0) continue;
        if(addr->driverName == driverName){
            //found
            if(addr->RWStrategy!=RWS){
                //differ
                addr->RWStrategy = RWS;
                if(addr->RWStrategy==RWS_READ_ONLY || addr->RWStrategy==RWS_READ_WRITE){
                    //read RWStrategy is exclusive!
                    foreach (TagAddress* addr2, addresses) {
                        if(addr2==0) continue;
                        if(addr2->driverName != driverName){
                            if(addr2->RWStrategy==RWS_READ_ONLY || addr2->RWStrategy==RWS_READ_WRITE){
                                //conflict!
                                addr2->RWStrategy=RWS_DISABLE;
                            }
                        }
                    }
                }
                emit RWStrategyChanged();
            }
        }
    }
}

int TagInfo::RWStrategy(const QString& driverName)const{
    foreach (TagAddress* addr, addresses) {
        if(addr==0) continue;
        if(addr->driverName == driverName){
            return addr->RWStrategy;
        }
    }
    return iTagInfo::RWS_DISABLE;
}

QString TagInfo::address(const QString& driverName)const{
    foreach (TagAddress* addr, addresses) {
        if(addr==0) continue;
        if(addr->driverName == driverName){
            return addr->address;
        }
    }
    return QString();
}

void TagInfo::setAddress(const QString& address, const QString& driverName){
    bool found = false;
    foreach (TagAddress* addr, addresses) {
        if(addr==0) continue;
        if(addr->driverName == driverName){
            found = true;
            if(address.isNull() || address.isEmpty()){
                delete addr;
                addresses.removeOne(addr);
                emit addressChanged();
            }
            else addr->address = address;
            emit addressChanged();
        }
    }
    if(!found && !address.isNull() && !address.isEmpty()){
        TagAddress* addr=new TagAddress();
        addr->driverName = driverName;
        addr->RWStrategy = iTagInfo::RWS_DISABLE;
        addr->address = address;
        addr->needToWrite = false;
        addr->tagInfo = this;
        addresses.append(addr);
        emit addressChanged();
    }
}

bool TagInfo::isAddressCorrect(const QString& driverName)const{
    //if address is empty, which means address isn't set, should also regard as correct!
    if(address(driverName).isEmpty()) return true;
    iDriver* pDriver = qobject_cast<iDriver*>(_pDriverMgr->driver(driverName) );
    if(pDriver){
        return pDriver->isAddressCorrect(address(driverName));
    }
    return false;
}

QList<int> TagInfo::availableRWStrategy(const QString& driverName)const{
    iDriver* pDriver = qobject_cast<iDriver*>(_pDriverMgr->driver(driverName) );
    if(pDriver){
        return pDriver->availableRWStrategy(address(driverName))<<RWS_DISABLE;
    }
    return QList<int>();
}

QList<int> TagInfo::allAvailableRWStrategy(const QString& driverName)const{
    iDriver* pDriver = qobject_cast<iDriver*>(_pDriverMgr->driver(driverName) );
    if(pDriver){
        return pDriver->availableRWStrategy()<<RWS_DISABLE;
    }
    return QList<int>();
}

QString TagInfo::addressErrorString(const QString& driverName)const{
    iDriver* pDriver = qobject_cast<iDriver*>(_pDriverMgr->driver(driverName) );
    if(pDriver){
        return pDriver->addressErrorString(address(driverName));
    }
    return QString();
}

//bool TagInfo::needRead()const{
//    return _needToRead;
//}

//void TagInfo::pollValue(){
//    _needToRead = true;
//}

//    QList<TagAddress> addresses;
//    bool _needToRead;
//    QString _pollGroupName;
//    int _type,_projectID,_stationID,_moduleID,_pointID;
//    QString _description,_extraInfo, _name, _unit, _projectName, _stationName, _systemName;
//    float _ratio, _offset;
//    QVariant _value;
//    QDateTime _lastUpdateTime;
void TagInfo::save(iLoadSaveProcessor* processor){
    processor->writeValue("addressesCount", addresses.size());
    for(int i=0; i<addresses.size(); ++i){
        processor->moveToInstance("TagAddress",QString::number(i));
        addresses[i]->save(processor);
        processor->moveBackToParent();
    }

//    processor->writeValue("_pollGroupName",_pollGroupName);

    processor->writeValue("_type",_type);
    processor->writeValue("_projectID",_projectID);
    processor->writeValue("_stationID",_stationID);
    processor->writeValue("_moduleID",_moduleID);
    processor->writeValue("_pointID",_pointID);

    processor->writeValue("_description",_description);
    processor->writeValue("_extraInfo",_extraInfo);
    processor->writeValue("_name",_name);
    processor->writeValue("_unit",_unit);
    processor->writeValue("_projectName",_projectName);
    processor->writeValue("_stationName",_stationName);
    processor->writeValue("_systemName",_systemName);

    processor->writeValue("_ratio",_ratio);
    processor->writeValue("_offset",_offset);

    processor->writeValue("_value",_value);

    processor->writeValue("_lastUpdateTime",_lastUpdateTime);
}

void TagInfo::load(iLoadSaveProcessor* processor){
    foreach (TagAddress* addr, addresses) {
        if(addr!=0) delete addr;
        addresses.removeOne(addr);
    }
    int addressesSize;
    processor->readValue("addressesCount", addressesSize);
    for(int i=0; i<addressesSize; ++i){
        processor->moveToInstance("TagAddress",QString::number(i));
        TagAddress* addr=new TagAddress();
        addr->load(processor);
        addr->tagInfo = this;
        addr->needToWrite = false;
        addresses.append(addr);
        processor->moveBackToParent();
    }

//    processor->readValue("_needToRead",_needToRead);

//    processor->readValue("_pollGroupName",_pollGroupName);

    processor->readValue("_type",_type);
    processor->readValue("_projectID",_projectID);
    processor->readValue("_stationID",_stationID);
    processor->readValue("_moduleID",_moduleID);
    processor->readValue("_pointID",_pointID);

    processor->readValue("_description",_description);
    processor->readValue("_extraInfo",_extraInfo);
    processor->readValue("_name",_name);
    processor->readValue("_unit",_unit);
    processor->readValue("_projectName",_projectName);
    processor->readValue("_stationName",_stationName);
    processor->readValue("_systemName",_systemName);

    processor->readValue("_ratio",_ratio);
    if(_ratio==0.) _ratio = 0.0;
    processor->readValue("_offset",_offset);

    processor->readValue("_value",_value);

    processor->readValue("_lastUpdateTime",_lastUpdateTime);
}


QDateTime TagInfo::lastUpdateTime()const{
    return _lastUpdateTime;
}

//QDataStream& operator<<(QDataStream& out, const TagInfo& t){
//    out<<t._address<<t._rankCode<<t._type<<t._RWStrategy<<t._projectID<<t._stationID<<t._moduleID<<t._pointID;
//    out<<t._description<<t._extraInfo<<t._name<<t._unit<<t._projectName<<t._stationName<<t._systemName;
//    out<<t._value;
//    out<<t._lastUpdateTime;
//    return out;
//}
//QDataStream& operator>>(QDataStream& in, TagInfo& t){
//    in>>t._address>>t._type>>t._RWStrategy>>t._projectID>>t._stationID>>t._moduleID>>t._pointID;
//    in>>t._description>>t._extraInfo>>t._name>>t._unit>>t._projectName>>t._stationName>>t._systemName;
//    in>>t._value;
//    in>>t._lastUpdateTime;
//    return in;
//}
bool operator<(const TagInfo& t1, const TagInfo& t2){
    return t1._name < t2._name;
}
bool operator>(const TagInfo& t1, const TagInfo& t2){
    return t1._name > t2._name;
}
bool operator==(const TagInfo& t1, const TagInfo& t2){
    return t1._name == t2._name;
}
bool operator>=(const TagInfo& t1, const TagInfo& t2){
    return t1._name >= t2._name;
}
bool operator<=(const TagInfo& t1, const TagInfo& t2){
    return t1._name <= t2._name;
}

void TagInfo::_setNeedToWrite(){
    foreach (TagAddress* addr, addresses) {
        if(addr->RWStrategy == iTagInfo::RWS_READ_WRITE || addr->RWStrategy == iTagInfo::RWS_WRITE_ONLY )
            addr->needToWrite = true;
    }
}

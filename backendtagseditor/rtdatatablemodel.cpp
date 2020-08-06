#include "rtdatatablemodel.h"
#include "rtdata.h"
#include "taginfo.h"
#include "drivermgr.h"
//#include "PollGroups/pollgroupmgr.h"
#include <QBrush>
#include <QString>
#include <QMap>

static RTData *_pData = nullptr;
static iDriverMgr* _pDriverMgr = nullptr;
//static PollGroupMgr* _pPollGroupMgr = nullptr;

int RTDataTableModel::column(int itemName, const QString& driverName)const{
    int accPos=0;
    int itemNo=0;
    int length = 0;
    if(itemName<0) return -1;
    if(partVisible(GENERAL)){
        length = partLength(GENERAL);
        itemNo = itemName-StartOfGeneral-1;
        if( itemNo>= 0 && itemNo< length )
            return itemNo;
        accPos += length;
    }
    if(partVisible(INFO)){
        length = partLength(INFO);
        itemNo = itemName-StartOfInfo-1;
        if( itemNo>= 0 && itemNo< length )
            return itemNo + accPos;
        accPos += length;
    }
    if(partVisible(DRIVERS)){
        length = partLength(DRIVERS);
        itemNo = itemName-StartOfDrivers-1;
        if( itemNo>= 0 && itemNo< length ){
            int driverNo = driverNames.indexOf(driverName);
            if(driverNo<0) return -1;
            return itemNo + length*driverNo + accPos;
        }
        accPos += partLength(DRIVERS)*driverNames.size();
    }
    if(partVisible(SCALE)){
        length = partLength(SCALE);
        itemNo = itemName-StartOfScale-1;
        if( itemNo>= 0 && itemNo< length )
            return itemNo + accPos;
        accPos += length;
    }
    if(partVisible(ONLINESTATE)){
        length = partLength(ONLINESTATE);
        itemNo = itemName-StartOfOnlineState-1;
        if( itemNo>= 0 && itemNo< length )
            return itemNo + accPos;
    }
    return -1;

}
bool RTDataTableModel::setValue(TagInfo* t, int column, const QVariant& value){
    int partName, itemName;
    QString driverName;
    bool ok=true;
    getNames(column, &partName, &itemName, &driverName);
    if(partName<0||itemName<0) return false;
    if(!partVisible(partName)) return false;
    switch(itemName){
    case Name:
        t->setName( value.toString() );
        return ok;
    case Type:
        t->setType( value.toInt(&ok) );
        return ok;
    case Description:
        t->setDescription( value.toString() );
        return ok;
    case Unit:
        t->setUnit( value.toString() );
        return ok;
    case ExtraInfo:
        t->setExtraInfo( value.toString() );
        return ok;
    case SubSystemName:
        t->setSystemName( value.toString() );
        return ok;
    case ProjectID:
        t->setProjectID( value.toInt(&ok) );
        return ok;
    case ProjecttName:
        t->setProjectName( value.toString() );
        return ok;
    case StationID:
        t->setStationID( value.toInt(&ok) );
        return ok;
    case StationName:
        t->setStationName( value.toString() );
        return ok;
    case ModuleID:
        t->setModuleID( value.toInt(&ok) );
        return ok;
    case PointID:
        t->setPointID( value.toInt(&ok) );
        return ok;
    case Address:
        t->setAddress( value.toString(), driverName );
        return ok;
    case RWStrategy:
        t->setRWStrategy( value.toInt(&ok), driverName );
        return ok;
//    case PollGroup:
//        t->setPollGroupName( value.toString() );
//        return ok;
    case Ratio:
        t->setScaleRatio( value.toFloat() );
        return ok;
    case Offset:
        t->setScaleOffset( value.toFloat() );
        return ok;
    case Value:
    case LastUpdateTimes:
        return false;
    }
    return false;
}

QVariant RTDataTableModel::value(TagInfo *t, int column)const{
    int partName, itemName;
    QString driverName;
    getNames(column, &partName, &itemName, &driverName);
    if(partName<0||itemName<0) return false;
    if(!partVisible(partName)) return false;
    switch(itemName){
    case Name:
        return t->name();
    case Type:
        return iTagInfo::enumTypeString(t->type());
    case Description:
        return t->description();
    case Unit:
        return t->unit();
    case ExtraInfo:
        return t->extraInfo();
    case SubSystemName:
        return t->systemName();
    case ProjectID:
        return t->projectID();
    case ProjecttName:
        return t->projectName();
    case StationID:
        return t->stationID();
    case StationName:
        return t->stationName();
    case ModuleID:
        return t->moduleID();
    case PointID:
        return t->pointID();
    case Address:
        return t->address(driverName);
    case RWStrategy:
        return iTagInfo::enumRWStrategyString(t->RWStrategy(driverName));
//    case PollGroup:
//        return t->pollGroupName();
    case Ratio:
        return t->scaleRatio();
    case Offset:
        return t->scaleOffset();
    case Value:
        return t->value();
    case LastUpdateTimes:
        return t->lastUpdateTime();
    }
    return QVariant();
}
QVariant RTDataTableModel::columnName(int column)const{
    int partName, itemName;
    QString driverName;
    getNames(column, &partName, &itemName, &driverName);
    if(partName<0||itemName<0) return QString();
    if(!partVisible(partName)) return QString();
    switch(itemName){
    case Name:
        return tr("名称");
    case Type:
        return tr("数据类型");
    case Description:
        return tr("描述");
    case Unit:
        return tr("单位");
    case ExtraInfo:
        return tr("备注");
    case SubSystemName:
        return tr("子系统名称");
    case ProjectID:
        return tr("项目ID");
    case ProjecttName:
        return tr("项目名称");
    case StationID:
        return tr("站点ID");
    case StationName:
        return tr("站点名称");
    case ModuleID:
        return tr("模块ID");
    case PointID:
        return tr("通道ID");
    case Address:
        return tr("%1：地址").arg(driverName);
    case RWStrategy:
        return tr("%1：读写权限").arg(driverName);
//    case PollGroup:
//        return tr("轮询组");
    case Ratio:
        return tr("比例");
    case Offset:
        return tr("偏移");
    case Value:
        return tr("值");
    case LastUpdateTimes:
        return tr("最后更新时间");
    }
    return QVariant();
}

int RTDataTableModel::partLength(int partName)const{
    switch (partName) {
    case GENERAL:
        if(partVisible(GENERAL))
            return 2;
        else return 0;
    case INFO:
        if(partVisible(INFO))
            return 10;
        else return 0;
    case DRIVERS:
        if(partVisible(DRIVERS))
            return 2;
        else return 0;
    case SCALE:
        if(partVisible(SCALE))
            return 2;
        else return 0;
    case ONLINESTATE:
        if(partVisible(ONLINESTATE))
            return 2;
        else return 0;
    default:
        return 0;
    }
}

void RTDataTableModel::setPartVisible(int partName, bool visible){
    if(partName<0 || partName>=_partsVisibleState.size()) return;
    if(_partsVisibleState[partName] != visible){
        emit layoutAboutToBeChanged();
        _partsVisibleState[partName]=visible;
        emit updateDelegates();
        emit layoutChanged();
    }
}

bool RTDataTableModel::partVisible(int partName)const{
    if(partName<0 || partName>=_partsVisibleState.size()) return false;
    return _partsVisibleState[partName];
}

void RTDataTableModel::getNames(int column, int* partName, int* itemName, QString* driverName)const{
    int accPos = 0;
    int itemNo=0;
    int length = 0;
    if(partVisible(GENERAL)){
        length = partLength(GENERAL);
        itemNo = column - accPos;
        if(itemNo>=0 && itemNo<length){
            if(partName) *partName = GENERAL;
            if(itemName) *itemName = StartOfGeneral + itemNo + 1;
            if(driverName) *driverName = QString();
            return;
        }
        accPos += length;
    }
    if(partVisible(INFO)){
        length = partLength(INFO);
        itemNo = column - accPos;
        if(itemNo>=0 && itemNo<length ){
            if(partName) *partName = INFO;
            if(itemName) *itemName = StartOfInfo + itemNo + 1;
            if(driverName) *driverName = QString();
            return;
        }
        accPos += length;
    }
    if(partVisible(DRIVERS)){
        length = partLength(DRIVERS);
        int totalLength = length * driverNames.size();
        itemNo = column - accPos;
        if(itemNo>=0 && itemNo<totalLength){
            if(partName) *partName = DRIVERS;
            if(itemName) *itemName = StartOfDrivers + (itemNo % length) + 1;
            if(driverName) *driverName = driverNames.at(itemNo / length);
            return;
        }
        accPos += totalLength;
    }
    if(partVisible(SCALE)){
        length = partLength(SCALE);
        itemNo = column - accPos;
        if(itemNo>=0 && itemNo<length ){
            if(partName) *partName = SCALE;
            if(itemName) *itemName = StartOfScale + itemNo + 1;
            if(driverName) *driverName = QString();
            return;
        }
        accPos += length;
    }
    if(partVisible(ONLINESTATE)){
        length = partLength(ONLINESTATE);
        itemNo = column - accPos;
        if(itemNo>=0 && itemNo<length ){
            if(partName) *partName = ONLINESTATE;
            if(itemName) *itemName = StartOfOnlineState + itemNo + 1;
            if(driverName) *driverName = QString();
            return;
        }
    }
    return;
}

void RTDataTableModel::driverUpdated(){
    driverNames = _pDriverMgr->driverNames();
    emit updateDelegates();
}


RTDataTableModel::RTDataTableModel(QObject *parent):QAbstractTableModel(parent)
{
    _pData = RTData::Instance();
    connect(_pData,&RTData::beginResetModel, this, &RTDataTableModel::beginResetModel);
    connect(_pData,&RTData::endResetModel,   this, &RTDataTableModel::endResetModel);

    _pDriverMgr = DriverMgr::Instance();
    connect(_pDriverMgr,&iDriverMgr::driverListChanged, this, &RTDataTableModel::driverUpdated);
    driverNames = _pDriverMgr->driverNames();

//    _pPollGroupMgr = PollGroupMgr::Instance();

    _partsVisibleState<<true<<true<<true<<true<<true<<true;
}


int RTDataTableModel::rowCount(const QModelIndex &parent) const{
    if (parent.isValid())
        return 0;
    return _pData->_tags.size();
}

int RTDataTableModel::columnCount(const QModelIndex &parent) const{
    if (parent.isValid())
        return 0;
    return partLength(GENERAL)+
            partLength(INFO)+
            partLength(DRIVERS)* driverNames.size()+
            partLength(SCALE)+
            partLength(ONLINESTATE);
}

QVariant RTDataTableModel::data(const QModelIndex &index, int role) const{
    if(!index.isValid()){
        return QVariant();
    }
    if(index.row()<0 || index.row()>=_pData->_tags.size() ) return QVariant();

    int partName, itemName;
    QString driverName;
    int row = index.row();
    int column = index.column();
    getNames(column, &partName, &itemName, &driverName);
    TagInfo* t = _pData->_tags.at(row);
    if(t == nullptr) return QVariant();

    if(role == Qt::ToolTipRole){
        //judge name conflict
        if( itemName == Name){
            if(_pData->contains( t->name(), t ) ){
                return tr("Name conflict!");
            }
        }
//        //judge pollGroupName existence;
//        if( itemName == PollGroup){
//            if( !_pPollGroupMgr->contains( t->pollGroupName()) ){
//                return tr("PollGroup name does NOT exist!");
//            }
//        }
        //judge whether RWStrategy is supported
        if( itemName == RWStrategy){
            if( !t->availableRWStrategy(driverName)
                    .contains(t->RWStrategy(driverName)) ){
                return tr("Read write strategy is NOT supported!");
            }
        }
        //judge whether Address is supported
        if( itemName == Address){
            if( !t->isAddressCorrect(driverName) ){
                return t->addressErrorString(driverName);
            }
        }
    }else if(role == Qt::BackgroundRole){
        QBrush brush;
        brush.setStyle( Qt::SolidPattern);
        if(row % 2){
            brush.setColor( QColor(225,225,225,100) );
        }else{
            brush.setColor( QColor(255,255,255,100) );
        }
        //judge name conflict
        if( itemName == Name){
            if(_pData->contains( t->name(), t ) ){
                brush.setColor( QColor(255,69,0,100) );
            }
        }
//        //judge pollGroupName existence;
//        if( itemName == PollGroup){
//            if( !_pPollGroupMgr->contains( t->pollGroupName()) ){
//                brush.setColor( QColor(255,0,0,100) );
//            }
//        }
        //judge whether RWStrategy is supported
        if( itemName == RWStrategy){
            if( !t->availableRWStrategy(driverName)
                    .contains(t->RWStrategy(driverName)) ){
                brush.setColor( QColor(255,0,0,100) );
            }
        }
        //judge whether Address is supported
        if( itemName == Address){
            if( !t->isAddressCorrect(driverName) ){
                brush.setColor( QColor(255,0,0,100) );
            }
        }
        return QVariant(brush);

    }else if(role == Qt::TextAlignmentRole){
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }else if(role == Qt::DisplayRole || role ==Qt::EditRole ){
        return value(t,column);
    }else if(role == Qt::UserRole){
        //store tagName in first column as user role
        //let related delegates be easily informed.
        if(column == 0){
            return QVariant(t->name());
        }
    }
    return QVariant();
}

QVariant RTDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if(role == Qt::TextAlignmentRole){
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }else if(role == Qt::DisplayRole ){
        if(orientation == Qt::Horizontal){
            return columnName(section);
        }else{
            return QVariant::fromValue(section+1);
        }
    }
    return QVariant();
}

bool RTDataTableModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!index.isValid()){
        return false;
    }
    if (data(index, role) != value) {
        if(index.row()<0 || index.row()>=_pData->_tags.size() ) return false;
        TagInfo* t =  _pData->_tags.at( index.row() );
        if(t == nullptr) return false;
        return setValue(t,index.column(),value);
    }
    return false;
}

Qt::ItemFlags RTDataTableModel::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return 0;
    if( column(Value) == index.column() || column(LastUpdateTimes) == index.column())
        return Qt::ItemIsEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

bool RTDataTableModel::insertRows(int row, int count, const QModelIndex &parent){
    if(row<0) row=0;
    beginInsertRows(parent, row, row + count - 1);
    for(int i=0; i<count&&i<=_pData->_tags.length(); ++i){
        _pData->insertTagInfo(row);
    }
    endInsertRows();
    return true;
}

bool RTDataTableModel::removeRows(int row, int count, const QModelIndex &parent){
    if(row<0) row=0;
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=0; i<count&&i<_pData->_tags.length(); ++i){
        _pData->removeTagInfo(row);
    }
    endRemoveRows();
    return true;
}

typedef bool (*LessThan)(const QPair<QVariant,int> &left,const QPair<QVariant,int> &right);
bool itemLessThan(const QPair<QVariant,int> &left,
                  const QPair<QVariant,int> &right)
{
    //try to compare as number
    bool isNum;
    float leftNum = left.first.toFloat(&isNum);
    if(isNum){
        float rightNum = right.first.toFloat(&isNum);
        if(isNum)
            return leftNum < rightNum;
    }
    return left.first < right.first;
}

bool itemGreaterThan(const QPair<QVariant,int> &left,
                     const QPair<QVariant,int> &right)
{
    //try to compare as number
    bool isNum;
    float leftNum = left.first.toFloat(&isNum);
    if(isNum){
        float rightNum = right.first.toFloat(&isNum);
        if(isNum)
            return leftNum > rightNum;
    }
    return left.first > right.first;
}

void RTDataTableModel::sort(int column, Qt::SortOrder order){
    QVector<QPair<QVariant, int> > sortable;
    QVector<int> unsortable;

    sortable.reserve(rowCount());
    unsortable.reserve(rowCount());

    for (int row = 0; row < rowCount(); ++row)
    {
        QVariant itm = this->data(this->index(row,column));
        if (!itm.isNull())
        {
            sortable.append(QPair<QVariant,int>(itm, row));
        }
        else
        {
            unsortable.append(row);
        }
    }
    LessThan compare = (order == Qt::AscendingOrder ? &itemLessThan : &itemGreaterThan);
    std::stable_sort(sortable.begin(), sortable.end(), compare);

    QList<TagInfo*>  temp_data;
    temp_data.reserve( rowCount() );
    emit layoutAboutToBeChanged();
    int nSwapCount=rowCount();
    for(int i=0;i<nSwapCount;i++){
        int r = (i < sortable.count()?
                     sortable.at(i).second : unsortable.at(i - sortable.count()));
        temp_data.append( _pData->_tags.at(r) );
    }
    _pData->_tags.clear();
    _pData->_tags = temp_data;
    emit layoutChanged();
}

QList<int> RTDataTableModel::errorRows(){
    QList<int> ret;
    for( int i=0; i<_pData->_tags.size(); i++){
        iTagInfo* t = _pData->_tags.at(i);
        //judge name conflict
        if(_pData->contains(t->name(), t)){
            ret<<i;
            continue;
        }
//        //judge pollGroupName exists
//        else if(!_pPollGroupMgr->contains(t->pollGroupName())){
//            ret<<i;
//        }
        foreach (TagAddress* a, t->allAddresses()) {
            QString driverName = a->driverName;
            //judge whether RWStrategy is supported
            if( !t->availableRWStrategy(driverName)
                    .contains(t->RWStrategy(driverName)) ){
                ret<<i;
                continue;
            }
            //judge whether Address is supported
            else if( !t->isAddressCorrect(driverName) ){
                ret<<i;
                continue;
            }
        }
    }
    return ret;
}




////////////////////////////////data general part
//static const QStringList headDataGeneralPart = (QStringList()<<QObject::tr("名称")<<QObject::tr("描述")
//                                                <<QObject::tr("备注")<<QObject::tr("数据类型"));
//static QVariant dataGeneralPart(TagInfo* t, int pos){
//    switch(pos){
//    case 0:
//        return QVariant(t->name());
//    case 1:
//        return QVariant(t->description());
//    case 2:
//        return QVariant(t->extraInfo());
//    case 3:
//        return QVariant(TagInfo::enumTypeString(t->type() ));
//    default:
//        return QVariant();
//    }
//}
//static void setDataGeneralPart(TagInfo* t, int pos, const QVariant& data){
//    switch(pos){
//    case 0:
//        t->setName(data.toString());
//        break;
//    case 1:
//        t->setDescription(data.toString());
//        break;
//    case 2:
//        t->setExtraInfo(data.toString());
//        break;
//    case 3:
//        t->setType(data.toInt());
//        break;
//    }
//}
///////////////////////////////data info part
//static const QStringList headDataInfoPart = (QStringList()<<QObject::tr("子系统名称")<<QObject::tr("单位")
//                                                <<QObject::tr("项目ID")<<QObject::tr("项目名称")
//                                                <<QObject::tr("站点ID")<<QObject::tr("站点名称")
//                                                <<QObject::tr("模块ID")<<QObject::tr("通道ID"));
//static QVariant dataInfoPart(TagInfo* t, int pos){
//    switch(pos){
//    case 0:
//        return QVariant(t->systemName());
//    case 1:
//        return QVariant(t->unit());
//    case 2:
//        return QVariant(t->projectID());
//    case 3:
//        return QVariant(t->projectName() );
//    case 4:
//        return QVariant(t->stationID());
//    case 5:
//        return QVariant(t->stationName() );
//    case 6:
//        return QVariant(t->moduleID() );
//    case 7:
//        return QVariant(t->pointID() );
//    default:
//        return QVariant();
//    }
//}
//static void setDataInfoPart(TagInfo* t, int pos, const QVariant& data){
//    switch(pos){
//    case 0:
//        t->setSystemName(data.toString());
//        break;
//    case 1:
//        t->setUnit(data.toString());
//        break;
//    case 2:
//        t->setProjectID(data.toInt());
//        break;
//    case 3:
//        t->setProjectName(data.toString());
//        break;
//    case 4:
//        t->setStationID(data.toInt());
//        break;
//    case 5:
//        t->setStationName(data.toString());
//        break;
//    case 6:
//        t->setModuleID(data.toInt());
//        break;
//    case 7:
//        t->setPointID(data.toInt());
//        break;
//    }
//}
///////////////////////////////data process part
//static const QStringList headDataDataProcessPart = (QStringList()<<QObject::tr("轮询组")<<QObject::tr("阶码"));
//static QVariant dataDataProcessPart(TagInfo* t, int pos){
//    switch(pos){
//    case 0:
//        return QVariant(t->pollGroupName());
//    case 1:
//        return QVariant(t->rankCode());
//    default:
//        return QVariant();
//    }
//}
//static void setDataDataProcessPart(TagInfo* t, int pos, const QVariant& data){
//    switch(pos){
//    case 0:
//        t->setPollGroupName(data.toString());
//        break;
//    case 1:
//        t->setRankCode(data.toInt());
//        break;
//    }
//}
////////////////////////data online state part
//static const QStringList headDataOnlineStatePart = (QStringList()<<QObject::tr("值")<<QObject::tr("最后更新时间"));
//static QVariant dataOnlineStatePart(TagInfo* t, int pos){
//    switch(pos){
//    case 0:
//        return t->value();
//    case 1:
//        return QVariant(t->lastUpdateTime());
//    default:
//        return QVariant();
//    }
//}
//static void setDataOnlineStatePart(TagInfo* t, int pos, const QVariant& data){
//    Q_UNUSED(t)
//    Q_UNUSED(pos)
//    Q_UNUSED(data)
//}
//////////////////////data address part
//static const QStringList headDataAddressPart = (QStringList()<<QObject::tr("地址")<<QObject::tr("数据类型"));
//static QVariant dataAddressPart(TagInfo* t, const QString& driverName, int pos){
//    switch(pos){
//    case 0:
//        return QVariant(t->address(driverName) );
//    case 1:
//        return QVariant(TagInfo::enumTypeString(t->RWStrategy(driverName)));
//    default:
//        return QVariant();
//    }
//}
//static void setDataAddressPart(TagInfo* t, const QString& driverName, int pos, const QVariant& data){
//    switch(pos){
//    case 0:
//        t->setAddress(data.toString(), driverName);
//        break;
//    case 1:
//        t->setRWStrategy(data.toInt(), driverName);
//        break;
//    }
//}

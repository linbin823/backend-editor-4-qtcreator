#include "drivermodel.h"
#include "drivermgr.h"
#include "insertdriver.h"

static DriverMgr* _pMgr=nullptr;
static InsertDriver* _pInserter=nullptr;

DriverModel::DriverModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    _pMgr = DriverMgr::Instance();
}

QVariant DriverModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::TextAlignmentRole){
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }else if(role == Qt::DisplayRole ){
        if(orientation == Qt::Horizontal){
            switch (section) {
            case 0:
                return tr("名称.");
            case 1:
                return tr("状态.");
            case 2:
                return tr("类型.");
            case 3:
                return tr("配置.");
            }
        }else{
            return QString::number(section+1);
        }
    }
    return QVariant();
}

int DriverModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return _pMgr->allDrivers().size();
}

int DriverModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4;
}

QVariant DriverModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    if(index.row()<0 || index.row()>=_pMgr->allDrivers().size() ) return QVariant();
    if(role == Qt::BackgroundRole){
        QBrush brush;
        brush.setStyle( Qt::SolidPattern);
        brush.setColor( QColor(255,255,255,100) );
        QStringList names = _pMgr->driverNames();
        for(int i=0; i<names.size(); i++){
            for(int j=i+1; j<names.size(); j++){
                if(names.at(i) == names.at(j)){
                    brush.setColor( QColor(255,0,0,100) );
                }
            }
        }
        return QVariant(brush);

    }else if(role == Qt::TextAlignmentRole){
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }else if(role == Qt::DisplayRole || role ==Qt::EditRole ){
        int row = index.row();
        if(row<0 || row>=_pMgr->allDrivers().size()) return QVariant();
        iDriver* d = _pMgr->allDrivers().at(row);
        if(d == nullptr) return QVariant();
        switch(index.column()){
        case 0:
            return _pMgr->driverNames().at(row);
        case 1:
            return iDriver::enumStatesString( d->state() );
        case 2:
            return d->driverInfo()->driverType;
        case 3:
            return tr("配置");
        }
    }
    return QVariant();
}

bool DriverModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if (data(index, role) != value) {
        if(index.row()<0 || index.row()>=_pMgr->allDrivers().size() ) return false;
        iDriver* d = _pMgr->allDrivers().at(index.row());
        if(d == nullptr) return false;
        switch(index.column()){
        case 0:
            _pMgr->setDriverName( index.row(), value.toString() );
            emit dataChanged(index, index, QVector<int>() << role);
            return true;
        default:
            break;
        }
    }
    return false;
}

Qt::ItemFlags DriverModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if(index.column()==0){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }else{
        return Qt::ItemIsEnabled;
    }
}

bool DriverModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(row<0) row=0;
    if(count > 1) count = 1;
    beginInsertRows(parent, row, row + count - 1);

    if(_pInserter==nullptr){
        _pInserter = new InsertDriver();
    }
    _pInserter->setInsertPos(row);
    _pInserter->show();

    endInsertRows();
    return true;
}

bool DriverModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row<0) row=0;
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=0; i<count&&i<_pMgr->allDrivers().size(); ++i){
        _pMgr->removeDriver(row);
    }
    endRemoveRows();
    return true;
}

QList<int> DriverModel::errorRows(){
    QList<int> ret;
    QStringList names = _pMgr->driverNames();
    for(int i=0; i<names.size(); i++){
        for(int j=i+1; j<names.size(); j++){
            if(names.at(i) == names.at(j)){
                ret<< i;
            }
        }
    }
    return ret;
}

int DriverModel::startDriver(int row){
    if(row<0 || row>=_pMgr->allDrivers().size() ) return -1;
    iDriver* d = _pMgr->allDrivers().at(row);
    QMetaObject::invokeMethod(d, "start");
    QModelIndex index =  createIndex(row, 1);
    emit dataChanged( index, index, QVector<int>() << Qt::DisplayRole );
    return 0;
}

int DriverModel::stopDriver(int row){
    if(row<0 || row>=_pMgr->allDrivers().size() ) return -1;
    iDriver* d = _pMgr->allDrivers().at(row);
    QMetaObject::invokeMethod(d, "stop");
    QModelIndex index =  createIndex(row, 1);
    emit dataChanged( index, index, QVector<int>() << Qt::DisplayRole );
    return 0;
}

void DriverModel::startAllDriver(void){
    _pMgr->startAll();
    QModelIndex startIndex = createIndex(0,1);
    QModelIndex stopIndex = createIndex(rowCount(),1);
    emit dataChanged( startIndex, stopIndex, QVector<int>() << Qt::DisplayRole );
}

void DriverModel::stopAllDriver(void){
    _pMgr->stopAll();
    QModelIndex startIndex = createIndex(0,1);
    QModelIndex stopIndex = createIndex(rowCount(),1);
    emit dataChanged( startIndex, stopIndex, QVector<int>() << Qt::DisplayRole );
}

void DriverModel::showConfigUI(QModelIndex index){
    if(!index.isValid()) return;
    if(index.column() ==3){
        _pMgr->allDrivers().at(index.row())->showUI(true);
    }
}

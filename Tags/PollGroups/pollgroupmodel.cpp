#include "pollgroupmodel.h"
#include "pollgroupmgr.h"

static PollGroupMgr* _pPollGroupMgr = nullptr;

PollGroupModel::PollGroupModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    _pPollGroupMgr = PollGroupMgr::Instance();
}

QVariant PollGroupModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::TextAlignmentRole){
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }else if(role == Qt::DisplayRole ){
        if(orientation == Qt::Horizontal){
            switch (section) {
            case 0:
                return tr("名称");
            case 1:
                return tr("轮询间隔（毫秒）");
            }
        }else{
            return QString::number(section+1);
        }
    }
    return QVariant();
}

int PollGroupModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return _pPollGroupMgr->pollGroupList().size();
}

int PollGroupModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant PollGroupModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    if(index.row()<0 || index.row()>=_pPollGroupMgr->pollGroupList().size() ) return QVariant();
    if(role == Qt::TextAlignmentRole){
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }else if(role == Qt::DisplayRole || role ==Qt::EditRole ){
        int row = index.row();
        PollGroup* p = static_cast<PollGroup*>(_pPollGroupMgr->pollGroupList().at(row));
        if(p == nullptr) return QVariant();
        switch(index.column()){
        case 0:
            return p->name;
        case 1:
            return p->intervMs;
        }
    }
    return QVariant();
}

bool PollGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if (data(index, role) != value) {
        if(index.row()<0 || index.row()>=_pPollGroupMgr->pollGroupList().size() ) return false;
        PollGroup* p = static_cast<PollGroup*>(_pPollGroupMgr->pollGroupList().at(index.row()));
        if(p == nullptr) return false;
        switch(index.column()){
        case 0:
            p->name = value.toString();
            break;
        case 1:
            p->intervMs = value.toInt();
            p->start();
            break;
        }
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags PollGroupModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool PollGroupModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(row<0) row=0;
    beginInsertRows(parent, row, row + count - 1);
    for(int i=0; i<count&&i<_pPollGroupMgr->pollGroupList().size(); ++i){
        _pPollGroupMgr->insertPollGroup(row);
    }
    endInsertRows();
    return true;
}

bool PollGroupModel::removeRows(int row, int count, const QModelIndex &parent)
{
    //not allowed to remove the last PollGroup
    if(rowCount()<=1) return false;
    if(row<0) row=0;
    beginRemoveRows(parent, row, row + count - 1);
    for(int i=0; i<count&&i<_pPollGroupMgr->pollGroupList().size(); ++i){
        _pPollGroupMgr->removePollGroup(row);
    }
    endRemoveRows();
    return true;
}

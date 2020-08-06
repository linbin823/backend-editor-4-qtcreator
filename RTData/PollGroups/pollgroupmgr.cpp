#include "pollgroupmgr.h"
#include "pollgroupmodel.h"
#include "pollgroupui.h"

PollGroupMgr* PollGroupMgr::_singleton=nullptr;
PollGroupMgr* PollGroupMgr::Instance(){
    if(_singleton == nullptr){
        _singleton = new PollGroupMgr();
    }
    return _singleton;
}
PollGroupMgr::PollGroupMgr(QObject *parent) : QObject(parent){
    _pModel = nullptr;
    _pUI = nullptr;
    insertPollGroup(0, tr("默认"),1000);
}
PollGroupMgr::~PollGroupMgr(){
    foreach(PollGroup* p, _pollGroups){
        p->deleteLater();
        _pollGroups.removeOne(p);
    }
    if(_pUI != nullptr){
        _pModel->deleteLater();
        _pUI->deleteLater();
    }
}
void PollGroupMgr::save(iLoadSaveProcessor* processor){
    int number=_pollGroups.size();
    processor->writeValue("PollGroupNumber", number );
    for(int i=0; i<_pollGroups.size(); i++){
        processor->moveToInstance("PollGroup",QString::number(i) );
        _pollGroups.at(i)->save(processor);
        processor->moveBackToParent();
    }
}

void PollGroupMgr::load(iLoadSaveProcessor* processor){
    int number=0;
    PollGroup* p;
    foreach(PollGroup* p, _pollGroups){
        p->deleteLater();
        _pollGroups.removeOne(p);
    }
    processor->readValue("PollGroupNumber",number);
    for(int i=0; i<number; i++){
        processor->moveToInstance("PollGroup",QString::number(i) );
        p = new PollGroup();
        p->load(processor);
        processor->moveBackToParent();
        if( contains(p->name ) ){
            p->name = genPollGroupName( p->name );
        }
        _pollGroups.append(p);
    }
    if(_pollGroups.size() ==0)
        insertPollGroup(0, tr("默认"),1000);
    emit pollGroupListChanged();
}

bool PollGroupMgr::contains(const QString& pollGroupName , PollGroup* ignore)const{
    foreach (PollGroup* p, _pollGroups) {
        if(p->name == pollGroupName){
            if(ignore != p){
                return true;
            }
        }
    }
    return false;
}

void PollGroupMgr::showUI(bool show, QWidget* parent){
    if(_pUI == nullptr){
        _pModel = new PollGroupModel(this);
        _pUI = new PollGroupUI(_pModel, parent);
    }
    if(show){
        _pUI->show();
    }else{
        _pUI->hide();
    }
}

QStringList PollGroupMgr::names(void)const{
    QStringList ret;
    foreach(PollGroup* p, _pollGroups){
        ret<<p->name;
    }
    return ret;
}

QObject* PollGroupMgr::pollGroup(const QString& name)const{
    foreach(PollGroup* p, _pollGroups){
        if(p->name == name){
            return static_cast<QObject* >(p);
        }
    }
    return nullptr;
}

QList<QObject*> PollGroupMgr::pollGroupList(void)const{
    QList<QObject*> ret;
    foreach (PollGroup* p, _pollGroups) {
        ret<<p;
    }
    return ret;
}

QString PollGroupMgr::genPollGroupName(const QString& testName){
    int i=0;
    QString retName = testName;
    while( contains( retName) ){
        i++;
        retName = testName + QString("_%1");
        retName = retName.arg( QString::number(i) );
    }
    return retName;
}

bool PollGroupMgr::insertPollGroup(int pos, const QString& groupName, int pollInterv){
    QString name;
    if(groupName.isNull() || groupName.isEmpty()){
        name = tr("新轮询组");
    }else{
        name = groupName;
    }
    PollGroup* p = new PollGroup();
    p->name = genPollGroupName( name );
    p->intervMs = pollInterv;
    p->start();
    _pollGroups.insert(pos, p);
    emit pollGroupListChanged();
    return true;
}

bool PollGroupMgr::removePollGroup(const QString& groupName){
    foreach (PollGroup* p, _pollGroups) {
        if(p->name == groupName){
            p->deleteLater();
            _pollGroups.removeOne(p);
            emit pollGroupListChanged();
            return true;
        }
    }
    return false;
}

bool PollGroupMgr::removePollGroup(int pos){
    if(pos<0 || pos >=_pollGroups.size()) return false;
    PollGroup* p = _pollGroups.at(pos);
    _pollGroups.removeOne(p);
    p->deleteLater();
    emit pollGroupListChanged();
    return true;
}

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QtCore/qmath.h>
#include "taginfo.h"
#include "rtdata.h"
#include "rtdataui.h"
//#include "PollGroups/pollgroupmgr.h"
//static PollGroupMgr* _pPollGroupMgr=nullptr;

RTData* RTData::_singleton = nullptr;
RTData* RTData::Instance(){
    if(_singleton == nullptr){
        _singleton = new RTData();
    }
    return _singleton;
}

RTData::RTData(QObject *parent) : iRTData(parent)
{
    _pModel = nullptr;
    _pUI = nullptr;
//    _pPollGroupMgr = PollGroupMgr::Instance();
//    //attach each pollgroups to tags when pollgroups changed!
//    connect(_pPollGroupMgr,&PollGroupMgr::pollGroupListChanged,[this](){
//        foreach (QObject* o, _pPollGroupMgr->pollGroupList()) {
//            PollGroup* p = static_cast<PollGroup*>(o);
//            if(!p) continue;
//            attachPollGroup(p->name);
//        }
//    });
    dataInit();
}

RTData::~RTData(){
    foreach(TagInfo* t, _tags){
        t->deleteLater();
        _tags.removeOne(t);
    }
    if(_pUI != nullptr){
        _pModel->deleteLater();
        _pUI->deleteLater();
    }
}

QList<QObject*> RTData::tagList(void) const {
    QList<QObject*> tagList;
    foreach (TagInfo* it, _tags) {
        tagList << it;
    }
    return tagList;
}

QList<iTagInfo*> RTData::allTags(void) const {
    QList<iTagInfo*> ret;
    foreach(TagInfo* t, _tags){
        ret<<t;
    }
    return ret;
}

QList<TagAddress*> RTData::allAddresses(void)const{
    QList<TagAddress*> ret;
    foreach(TagInfo* t, _tags){
        foreach(TagAddress* a, t->addresses){
            ret<<a;
        }
    }
    return ret;
}

QStringList RTData::names(void)const{
    QStringList ret;
    foreach(TagInfo* t, _tags){
        ret<< t->name();
    }
    return ret;
}

QObject* RTData::tagInfo(const QString& name)const{
    foreach(TagInfo* t ,_tags){
        if(t->name() == name){
            return static_cast<QObject* >(t);
        }
    }
    return nullptr;
}

bool RTData::contains(const QString& name, iTagInfo *ignore)const{
    foreach (TagInfo* t, _tags) {
        if(t->name() == name){
            if(ignore != t){
                return true;
            }
        }
    }
    return false;
}

void RTData::showUI(bool show, QWidget *parent){
    if(_pUI == nullptr){
        _pModel = new RTDataTableModel(this);
        _pUI = new RTDataUI(_pModel, parent);
    }
    if(show){
        _pUI->show();
    }else{
        _pUI->hide();
    }
}

void RTData::save(iLoadSaveProcessor* processor){
    int number=_tags.size();
    processor->writeValue("TagsNumber", number );
    for(int i=0; i<_tags.size(); i++){
        processor->moveToInstance("TagInfo",QString::number(i) );
        _tags.at(i)->save(processor);
        processor->moveBackToParent();
    }
}

void RTData::load(iLoadSaveProcessor* processor){
    int number=0;
    TagInfo* t;
    dataInit();
    processor->readValue("TagsNumber",number);
    for(int i=0; i<number; i++){
        processor->moveToInstance("TagInfo",QString::number(i) );
        t = new TagInfo("");
        t->load(processor);
        processor->moveBackToParent();
        if( contains(t->name() ) ){
            //name conflict!
            continue;
        }
        _tags.append(t);
        connect(t,&TagInfo::addressChanged, this, [this](){
            emit tagListChanged(allTags());
        });
        connect(t,&TagInfo::RWStrategyChanged, this, [this](){
            emit tagListChanged(allTags());
        });
        connect(t,&TagInfo::typeChanged, this, [this](){
            emit tagListChanged(allTags());
        });
    }
    emit tagListChanged(allTags());
}

bool RTData::insertTagInfo(int pos, const QString& tagName){
    QString name;
    if(tagName.isNull() || tagName.isEmpty()){
        name = tr("新测点");
    }else{
        name = tagName;
    }
    TagInfo* t = new TagInfo( genTagName(name) );
//    t->setPollGroupName( _pPollGroupMgr->names().at(0) );
//    attachPollGroup(_pPollGroupMgr->names().at(0),t);
//    connect(t, &TagInfo::pollGroupNameChanged, [t,this](){
//        attachPollGroup(t->pollGroupName(),t);
//    });

    _tags.insert(pos, t);
    connect(t,&TagInfo::addressChanged, this, [this](){
        emit tagListChanged(allTags());
    });
    connect(t,&TagInfo::RWStrategyChanged, this, [this](){
        emit tagListChanged(allTags());
    });
    connect(t,&TagInfo::typeChanged, this, [this](){
        emit tagListChanged(allTags());
    });
    emit tagListChanged(allTags());
    return true;
}

bool RTData::removeTagInfo(const QString& tagName){
    foreach (TagInfo* t, _tags) {
        if(t->name() == tagName){
            _tags.removeOne(t);
            t->deleteLater();
            emit tagListChanged(allTags());
            return true;
        }
    }
    return false;
}

bool RTData::removeTagInfo(int pos){
    if(pos<0 || pos >=_tags.size()) return false;
    TagInfo* t = _tags.at(pos);
    _tags.removeOne(t);
    t->deleteLater();
    emit tagListChanged(allTags());
    return true;
}

//void RTData::attachPollGroup(const QString& pollGroupName, iTagInfo* t)const{
//    PollGroup* p;
//    p = static_cast<PollGroup*>(_pPollGroupMgr->pollGroup(pollGroupName));
//    if(!p) return;
//    if(t){
//        //attach certain tag
//        disconnect(p,&PollGroup::pollUpdate,t,&iTagInfo::pollValue);
//        connect(p, &PollGroup::pollUpdate, t, &iTagInfo::pollValue);
//    }else{
//        //attach all tags
//        foreach (TagInfo* tag, _tags) {
//            if(tag->pollGroupName() == p->name){
//                disconnect(p,&PollGroup::pollUpdate,tag,&TagInfo::pollValue);
//                connect(p, &PollGroup::pollUpdate, tag, &TagInfo::pollValue);
//            }
//        }
//    }
//}

void RTData::dataInit(){
    foreach (TagInfo* it, _tags) {
        delete it;
    }
    _tags.clear();
}

QString RTData::genTagName(const QString& testName){
    int i=0;
    QString retName = testName;
    while( contains( retName) ){
        i++;
        retName = testName + QString("_%1");
        retName = retName.arg( QString::number(i) );
    }
    return retName;
}

void RTData::load_CSIC711_ConfigDB(const QString& fileName){

    int iConnIdx = 0;
    QString _strConnName;
    QSqlDatabase* _pDb;
    while(1)
    {
        _strConnName = QString("MyDBProcessConn%1").arg(++iConnIdx);
        QSqlDatabase dbConn = QSqlDatabase::database(_strConnName, false);
        //to avoid connName confliction
        if(dbConn.isValid()){//存在连接
            continue;
        }
        _pDb = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", _strConnName));
        break;
    }
    QString msg;
    QMessageBox box;
    if( !QFile::exists( fileName ) ){
        msg = tr("读取错误！\n文件不存在！");
        box.setText(msg);
        box.exec();
        return;
    }
    _pDb->setDatabaseName( fileName );
    if (!_pDb->open()){
        _pDb->close();
        msg = tr("读取错误！\n无法打开数据库文件，请确认格式是否正确！");
        box.setText(msg);
        box.exec();
        return;
    }

    QSqlQuery* _pquery = new QSqlQuery(*_pDb);

    int totalNumber, successNumber, failNumber;
    totalNumber = successNumber = failNumber = 0;

    emit beginResetModel();

    dataInit();

    msg = tr("成功读取记录！成功%1条，失败%2条。\n");

    _pquery->exec("select TAG_ID,TESTPOINTNAME,UNIT,RANKCODE,PRJID,STATIONID,MODULEID,POINTID from TESTPOIT_CONFIG");
    while(_pquery->next()){

        totalNumber++;
        if( !_pquery->isValid() ) continue;

        //name判断
        QString name = genTagName( _pquery->value(0).toString() );
        //address判断
        //address从project、station、module、point中计算得到
        int address,projectID,stationID,moduleID,pointID;
        bool ok;
        projectID = _pquery->value(4).toInt(&ok);
        if(!ok){
            msg += tr("读取失败，projectID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
            continue;
        }
        stationID = _pquery->value(5).toInt(&ok);
        if(!ok){
            msg += tr("读取失败，stationID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
            continue;
        }
        moduleID = _pquery->value(6).toInt(&ok);
        if(!ok){
            msg += tr("读取失败，moduleID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
            continue;
        }
        pointID = _pquery->value(7).toInt(&ok);
        if(!ok){
            msg += tr("读取失败，pointID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
            continue;
        }
        address = (projectID << 16) + (stationID << 9) + (moduleID << 5) + pointID;

        //success!
        successNumber ++;

        TagInfo* tag = new TagInfo( name );
        tag->setDescription( _pquery->value(1).toString() );
////////////        tag->setAddress( QString::number(address ) );
        tag->setUnit( _pquery->value(2).toString() );
        int temp = _pquery->value(3).toInt(&ok);
        if(ok){
            tag->setScaleRatio( qPow(10.,(-1)*temp) );
        }
        tag->setProjectID( projectID );
        tag->setStationID( stationID );
        tag->setModuleID( moduleID );
        tag->setPointID( pointID );
//        tag->setPollGroupName( _pPollGroupMgr->names().at(0) );
        if(_pquery->value(2).toString().isEmpty())
            tag->setType(TagInfo::TYPE_INT32);
        else
            tag->setType(TagInfo::TYPE_FLOAT);
        _tags.append(tag);
        connect(tag,&TagInfo::addressChanged, this, [this](){
            emit tagListChanged(allTags());
        });
        connect(tag,&TagInfo::RWStrategyChanged, this, [this](){
            emit tagListChanged(allTags());
        });
        connect(tag,&TagInfo::typeChanged, this, [this](){
            emit tagListChanged(allTags());
        });
    }
    failNumber = totalNumber - successNumber;
    _pquery->clear();
    _pquery->exec("select TAG_ID, PAGENM from CONSOLE_PAGE");
    while(_pquery->next()){
        QString name = _pquery->value(0).toString();
        QString sysName = _pquery->value(1).toString();
        for(int j=0; j<_tags.size(); j++){
            TagInfo* currentItem = _tags.at(j);
            if(name == currentItem->name() ){
                currentItem->setSystemName( sysName );
            }
        }
    }
    delete _pquery;
    _pDb->close();
    emit endResetModel();
    emit tagListChanged(allTags());

    //show message box
    msg = msg.arg(successNumber).arg(failNumber);
    box.setText(msg);
    box.exec();
}

bool RTData::_compareTagInfo(const TagInfo* t1, const TagInfo* t2){
    return t1->name() < t2->name();
}

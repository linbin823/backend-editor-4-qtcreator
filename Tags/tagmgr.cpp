#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QFile>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QtCore/qmath.h>
#include "../Interface/Tags/tag.h"
#include "../Interface/Tags/tagmgr.h"
#include "tagmgrui.h"
//#include "PollGroups/pollgroupmgr.h"
//static PollGroupMgr* _pPollGroupMgr=nullptr;

TagMgr* TagMgr::_singleton = nullptr;
TagMgr* TagMgr::Instance() {
    if(_singleton == nullptr) {
        _singleton = new TagMgr();
    }
    return _singleton;
}

TagMgr::TagMgr(QObject *parent) : QObject(parent)
{
    _pModel = nullptr;
    _pUI = nullptr;
//    _pPollGroupMgr = PollGroupMgr::Instance();
//    //attach each pollgroups to tags when pollgroups changed!
//    connect(_pPollGroupMgr,&PollGroupMgr::pollGroupListChanged,[this]() {
//        foreach (QObject* o, _pPollGroupMgr->pollGroupList()) {
//            PollGroup* p = static_cast<PollGroup*>(o);
//            if(!p) continue;
//            attachPollGroup(p->name);
//        }
//    });
    dataInit();
}

TagMgr::~TagMgr() {
    foreach(Tag* t, _tagList) {
        t->deleteLater();
        _tagList.removeOne(t);
    }
    if(_pUI != nullptr) {
        _pModel->deleteLater();
        _pUI->deleteLater();
    }
}

QList<Tag*> TagMgr::tagList(void) const {
    return _tagList;
}

QList<TagAddress*> TagMgr::allAddresses(void) const {
    QList<TagAddress*> ret;
    foreach(Tag* t, _tagList) {
        foreach(TagAddress* a, t->addresses) {
            ret<<a;
        }
    }
    return ret;
}

QStringList TagMgr::tagNames(void) const {
    QStringList ret;
    foreach(Tag* t, _tagList) {
        ret<< t->name();
    }
    return ret;
}

Tag* TagMgr::tag(const QString& name) const {
    foreach(Tag* t ,_tagList) {
        if(t->name() == name) {
            return t;
        }
    }
    return nullptr;
}

bool TagMgr::contains(const QString& name, Tag *ignore) const {
    foreach (Tag* t, _tagList) {
        if(t->name() == name) {
            if(ignore != t) {
                return true;
            }
        }
    }
    return false;
}

QWidget *TagMgr::showUI(bool show, QWidget *parent) {
    if(_pUI == nullptr) {
        _pModel = new TagMgrTableModel(this);
        _pUI = new TagMgrUI(_pModel, parent);
    }
    if(show) {
        _pUI->show();
    }else{
        _pUI->hide();
    }
    return _pUI;
}

void TagMgr::save(iLoadSaveProcessor* processor) {
    int number=_tagList.size();
    processor->writeValue("TagsNumber", number );
    for(int i=0; i<_tagList.size(); i++) {
        processor->moveToInstance("Tag",QString::number(i) );
        _tagList.at(i)->save(processor);
        processor->moveBackToParent();
    }
}

void TagMgr::load(iLoadSaveProcessor* processor) {
    int number=0;
    Tag* t;
    dataInit();
    processor->readValue("TagsNumber",number);
    for(int i=0; i<number; i++) {
        processor->moveToInstance("Tag",QString::number(i) );
        t = new Tag("");
        t->load(processor);
        processor->moveBackToParent();
        if( contains(t->name() ) ) {
            //name conflict!
            continue;
        }
        _tagList.append(t);
        connect(t,&Tag::addressChanged, this, [this]() {
            emit tagListChanged();
        });
        connect(t,&Tag::RWStrategyChanged, this, [this]() {
            emit tagListChanged();
        });
        connect(t,&Tag::typeChanged, this, [this]() {
            emit tagListChanged();
        });
    }
    emit tagListChanged();
}

bool TagMgr::insertTag(const QString& tagName, int pos) {
    QString name;
    if(tagName.isNull() || tagName.isEmpty()) {
        name = tr("新测点.");
    }else{
        name = tagName;
    }

    if (pos <0 || pos > _tagList.size()) {
        pos = _tagList.size();
    }

    Tag* t = new Tag( genTagName(name) );
    //    t->setPollGroupName( _pPollGroupMgr->names().at(0) );
    //    attachPollGroup(_pPollGroupMgr->names().at(0),t);
    //    connect(t, &Tag::pollGroupNameChanged, [t,this]() {
    //        attachPollGroup(t->pollGroupName(),t);
    //    });
    _tagList.insert(pos, t);
    connect(t,&Tag::addressChanged, this, [this]() {
        emit tagListChanged();
    });
    connect(t,&Tag::RWStrategyChanged, this, [this]() {
        emit tagListChanged();
    });
    connect(t,&Tag::typeChanged, this, [this]() {
        emit tagListChanged();
    });
    emit tagListChanged();
    return true;
}

bool TagMgr::removeTag(const QString& tagName) {
    foreach (Tag* t, _tagList) {
        if(t->name() == tagName) {
            _tagList.removeOne(t);
            t->deleteLater();
            emit tagListChanged();
            return true;
        }
    }
    return false;
}

bool TagMgr::removeTag(int pos) {
    if(pos<0 || pos >=_tagList.size()) return false;
    Tag* t = _tagList.at(pos);
    _tagList.removeOne(t);
    t->deleteLater();
    emit tagListChanged();
    return true;
}

//void TagMgr::attachPollGroup(const QString& pollGroupName, iTag* t)const{
//    PollGroup* p;
//    p = static_cast<PollGroup*>(_pPollGroupMgr->pollGroup(pollGroupName));
//    if(!p) return;
//    if(t) {
//        //attach certain tag
//        disconnect(p,&PollGroup::pollUpdate,t,&iTag::pollValue);
//        connect(p, &PollGroup::pollUpdate, t, &iTag::pollValue);
//    }else{
//        //attach all tags
//        foreach (Tag* tag, _tagList) {
//            if(tag->pollGroupName() == p->name) {
//                disconnect(p,&PollGroup::pollUpdate,tag,&Tag::pollValue);
//                connect(p, &PollGroup::pollUpdate, tag, &Tag::pollValue);
//            }
//        }
//    }
//}

void TagMgr::dataInit() {
    foreach (Tag* it, _tagList) {
        delete it;
    }
    _tagList.clear();
}

QString TagMgr::genTagName(const QString& testName) {
    int i=0;
    QString retName = testName;
    while( contains( retName) ) {
        i++;
        retName = testName + QString("_%1");
        retName = retName.arg( QString::number(i) );
    }
    return retName;
}

//void TagMgr::load_CSIC711_ConfigDB(const QString& fileName) {

//    int iConnIdx = 0;
//    QString _strConnName;
//    QSqlDatabase* _pDb;
//    while(1)
//    {
//        _strConnName = QString("MyDBProcessConn%1").arg(++iConnIdx);
//        QSqlDatabase dbConn = QSqlDatabase::database(_strConnName, false);
//        //to avoid connName confliction
//        if(dbConn.isValid()) {//存在连接
//            continue;
//        }
//        _pDb = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", _strConnName));
//        break;
//    }
//    QString msg;
//    QMessageBox box;
//    if( !QFile::exists( fileName ) ) {
//        msg = tr("读取错误！\n文件不存在！");
//        box.setText(msg);
//        box.exec();
//        return;
//    }
//    _pDb->setDatabaseName( fileName );
//    if (!_pDb->open()) {
//        _pDb->close();
//        msg = tr("读取错误！\n无法打开数据库文件，请确认格式是否正确！");
//        box.setText(msg);
//        box.exec();
//        return;
//    }

//    QSqlQuery* _pquery = new QSqlQuery(*_pDb);

//    int totalNumber, successNumber, failNumber;
//    totalNumber = successNumber = failNumber = 0;

//    emit beginResetModel();

//    dataInit();

//    msg = tr("成功读取记录！成功%1条，失败%2条。\n");

//    _pquery->exec("select TAG_ID,TESTPOINTNAME,UNIT,RANKCODE,PRJID,STATIONID,MODULEID,POINTID from TESTPOIT_CONFIG");
//    while(_pquery->next()) {

//        totalNumber++;
//        if( !_pquery->isValid() ) continue;

//        //name判断
//        QString name = genTagName( _pquery->value(0).toString() );
//        //address判断
//        //address从project、station、module、point中计算得到

//        int address,projectID,stationID,moduleID,pointID;
//        bool ok;
//        projectID = _pquery->value(4).toInt(&ok);
//        if(!ok) {
//            msg += tr("读取失败，projectID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
//            continue;
//        }
//        stationID = _pquery->value(5).toInt(&ok);
//        if(!ok) {
//            msg += tr("读取失败，stationID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
//            continue;
//        }
//        moduleID = _pquery->value(6).toInt(&ok);
//        if(!ok) {
//            msg += tr("读取失败，moduleID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
//            continue;
//        }
//        pointID = _pquery->value(7).toInt(&ok);
//        if(!ok) {
//            msg += tr("读取失败，pointID不正确。  ") + _pquery->value(0).toString() + "  " + _pquery->value(1).toString() + tr("\n");
//            continue;
//        }
//        address = (projectID << 16) + (stationID << 9) + (moduleID << 5) + pointID;

//        //success!
//        successNumber ++;

//        Tag* tag = new Tag( name );
//        tag->setDescription( _pquery->value(1).toString() );
//////////////        tag->setAddress( QString::number(address ) );
//        tag->setUnit( _pquery->value(2).toString() );
//        int temp = _pquery->value(3).toInt(&ok);
//        if(ok) {
//            tag->setScaleRatio( qPow(10.,(-1)*temp) );
//        }
//        tag->setProjectID( projectID );
//        tag->setStationID( stationID );
//        tag->setModuleID( moduleID );
//        tag->setPointID( pointID );
////        tag->setPollGroupName( _pPollGroupMgr->names().at(0) );
//        if(_pquery->value(2).toString().isEmpty())
//            tag->setType(Tag::TYPE_INT32);
//        else
//            tag->setType(Tag::TYPE_FLOAT);
//        _tagList.append(tag);
//        connect(tag,&Tag::addressChanged, this, [this]() {
//            emit tagListChanged();
//        });
//        connect(tag,&Tag::RWStrategyChanged, this, [this]() {
//            emit tagListChanged();
//        });
//        connect(tag,&Tag::typeChanged, this, [this]() {
//            emit tagListChanged();
//        });
//    }
//    failNumber = totalNumber - successNumber;
//    _pquery->clear();
//    _pquery->exec("select TAG_ID, PAGENM from CONSOLE_PAGE");
//    while(_pquery->next()) {
//        QString name = _pquery->value(0).toString();
//        QString sysName = _pquery->value(1).toString();
//        for(int j=0; j<_tagList.size(); j++) {
//            Tag* currentItem = _tagList.at(j);
//            if(name == currentItem->name() ) {
//                currentItem->setSystemName( sysName );
//            }
//        }
//    }
//    delete _pquery;
//    _pDb->close();
//    emit endResetModel();
//    emit tagListChanged();

//    //show message box
//    msg = msg.arg(successNumber).arg(failNumber);
//    box.setText(msg);
//    box.exec();
//}

bool TagMgr::_compareTag(const Tag* t1, const Tag* t2) {
    return t1->name() < t2->name();
}

#include "drivermgr.h"
#include "drivermodel.h"
#include <QPluginLoader>
#include <QMessageBox>
#include <QApplication>
#include <QDir>

DriverMgr* DriverMgr::_singleton = nullptr;
DriverMgr* DriverMgr::Instance(){
    if(_singleton == nullptr){
        _singleton = new DriverMgr();
    }
    return _singleton;
}

DriverMgr::DriverMgr(QObject *parent) : iDriverMgr(parent)
{
    _pModel= nullptr;
    _pUI = nullptr;
    driversInit();
    loadDrivers();
}

DriverMgr::~DriverMgr(){
    driversInit();
}

void DriverMgr::driversInit(){
    QList<iDriver*> drivers = _drivers.values();
    foreach(iDriver* d, drivers){
        d->stop();
        d->deleteLater();
    }
    foreach (QThread* thread, _threads) {
        thread->deleteLater();
    }
    _drivers.clear();
    _threads.clear();
}

void DriverMgr::loadDrivers(){
    _supportedDriversInfo.clear();
    foreach(QPluginLoader* loader, _supportedDriversLoader){
        loader->deleteLater();
        _supportedDriversLoader.removeOne(loader);
    }

    QDir _pluginsDir = QDir(QCoreApplication::instance()->applicationDirPath());

    #if defined(Q_OS_WIN)
    if (_pluginsDir.dirName().toLower() == "debug" || _pluginsDir.dirName().toLower() == "release")
        _pluginsDir.cdUp();
    #elif defined(Q_OS_MAC)
        if (_pluginsDir.dirName() == "MacOS") {
            _pluginsDir.cdUp();
            _pluginsDir.cdUp();
            _pluginsDir.cdUp();
        }
    #endif
    _pluginsDir.cd("Drivers");

    foreach (QString fileName, _pluginsDir.entryList(QDir::Files)) {
        QPluginLoader* pLoader = new QPluginLoader(_pluginsDir.absoluteFilePath(fileName));
        if(!setAvailableDriver(pLoader)){
            pLoader->deleteLater();
        }
    }
}

bool DriverMgr::setAvailableDriver(QPluginLoader* pLoader){
    iDriver *driver = qobject_cast<iDriver*>(pLoader->instance());
    if(driver){
        _supportedDriversInfo.append( *(driver->driverInfo()) );
        _supportedDriversLoader.append(pLoader);
        driver->deleteLater();
        //success
        return true;
    }
    //failed
    return false;
}

QString DriverMgr::genDriverName(const QString& testName){
    int i=0;
    QString retName = testName;
    while( contains( retName) ){
        i++;
        retName = testName + QString("_%1");
        retName = retName.arg( QString::number(i) );
    }
    return retName;
}

QList<DriverInfo> DriverMgr::supportDriversInfo(void)const{
    return _supportedDriversInfo;
}

void DriverMgr::startAll(void){
    QList<iDriver*> drivers = _drivers.values();
    foreach(iDriver* d, drivers){
        QMetaObject::invokeMethod(d,"start");
    }
}

void DriverMgr::stopAll(void){
    QList<iDriver*> drivers = _drivers.values();
    foreach(iDriver* d, drivers){
        QMetaObject::invokeMethod(d,"stop");
    }
}

QList<QObject*> DriverMgr::driverList(void){
    QList<QObject*> ret;
    QList<iDriver*> drivers = _drivers.values();
    foreach(iDriver* d, drivers){
        ret<<d;
    }
    return ret;
}

QList<iDriver*> DriverMgr::allDrivers(void){
    return _drivers.values();
}

QStringList DriverMgr::driverNames(void){
    return _drivers.keys();
}

QStringList DriverMgr::driverNameRelateToType(const QString& driverType){
    QStringList ret;
    foreach(iDriver* d, _drivers.values()){
        if(driverType == d->driverInfo()->driverType){
            ret<<_drivers.key(d);
        }
    }
    return ret;
}
void DriverMgr::setDriverName(int pos, const QString& driverName){
    if(pos<0 || pos>=_drivers.size()) return;
    QMap<QString, iDriver*>::Iterator it;
    it = _drivers.begin();
    it += pos;
    QString oldKey = it.key();
    iDriver* d = it.value();
    _drivers.remove(oldKey);
    _drivers.insert(genDriverName( driverName ), d);
    emit driverListChanged();
}

QObject* DriverMgr::driver(const QString& driverName){
    if(!_drivers.keys().contains(driverName) )
        return nullptr;
    return _drivers[driverName];
}

bool DriverMgr::contains(const QString& driverName)const{
    foreach (QString n, _drivers.keys()) {
        if(driverName == n){
            return true;
        }
    }
    return false;
}

int DriverMgr::setRelatedTags(const QList<iTagInfo *>& relatedTags)const{
    int count=0;
    QList< QList<TagAddress *> > sepList;
    for(int i=0;i<_drivers.count();++i){
        sepList.append( QList<TagAddress *>() );
    }
    QList<TagAddress *> relatedTagAddresses;
    foreach (iTagInfo* t, relatedTags) {
        relatedTagAddresses<<t->allAddresses();
    }
    foreach (TagAddress* t, relatedTagAddresses) {
        for(int i=0;i<_drivers.count();++i){
            if(t->driverName == _drivers.keys().at(i)){
                sepList[i].append(t);
                count++;
            }
        }
    }
    for(int i=0;i<_drivers.count();++i){
        _drivers.values().at(i)->setRelatedTagAddresses( sepList.at(i) );
    }
    return count;
}

QWidget *DriverMgr::showUI(bool show, QWidget* parent){
    if(_pUI == nullptr){
        _pModel = new DriverModel(this);
        _pUI = new DriverUI(_pModel, parent);
    }
    if(show){
        _pUI->show();
    }else{
        _pUI->hide();
    }
    return _pUI;
}

bool DriverMgr::insertDriver(int pos, const DriverInfo& info, const QString& driverName){
    if(pos <0 || pos >_drivers.size()) return false;
    QString name = driverName;
    if(name.isEmpty()) name = tr("新驱动 ");
    QMap<QString, iDriver*>::Iterator it;
    it = _drivers.begin();
    it += pos;

    for(int i=0;i<_supportedDriversInfo.count();++i){
        if(_supportedDriversInfo[i] == info){
            iDriver* d = qobject_cast<iDriver*>(_supportedDriversLoader[i]->instance() );
            if(d){
                //do some connections here
                _drivers.insert(it,genDriverName( name ),d);
                QThread* thread = new QThread(this);
                d->moveToThread(thread);
                thread->start();
                _threads.append(thread);
                emit driverListChanged();
                return true;
            }
        }
    }
    return false;
}

bool DriverMgr::removeDriver(const QString& driverName){
    iDriver* d = _drivers.take( driverName );
    if(d){
        d->deleteLater();
        emit driverListChanged();
        return true;
    }
    //not found
    return false;
}

bool DriverMgr::removeDriver(int pos){
    if(pos <0 || pos >=_drivers.size()) return false;
    return removeDriver( _drivers.keys().at(pos) );
}

void DriverMgr::save(iLoadSaveProcessor* processor){
    int num =_drivers.count();
    QString name;
    iDriver* pDriver;
    QMap<QString, iDriver*>::const_iterator it;
    processor->writeValue("driversNumber", num );
    int i=0;
    for(it = _drivers.begin(); it != _drivers.end(); ++it,i++){
        processor->moveToInstance("driver",QString::number(i));
        name = it.key();
        pDriver=it.value();
        processor->writeValue("name", name );

        processor->moveToInstance("driverInfo","");
        pDriver->driverInfo()->save(processor);
        processor->moveBackToParent();

        processor->moveToInstance("content", "");
        pDriver->save(processor);
        processor->moveBackToParent();

        processor->moveBackToParent();
    }
}

void DriverMgr::load(iLoadSaveProcessor* processor){
    int ret=0,num =0;
    DriverInfo info;
    QString name;
    iDriver* pDriver;
    ret = processor->readValue("driversNumber", num);
    if(ret !=0 ) return;//not found num
    for(int i=0; i<num; i++){
        ret = processor->moveToInstance("driver",QString::number(i));
        processor->readValue("name",name);
        processor->moveToInstance("driverInfo","");
        info.load(processor);
        processor->moveBackToParent();

        bool found = false;
        //first searching existence drivers
        foreach(QString n, _drivers.keys() ){
            if(n != name) continue;
            pDriver = _drivers[n];
            if(pDriver==0) continue;
            if(pDriver->driverInfo() == &info){
                found = true;
                processor->moveToInstance("content", "");
                pDriver->load(processor);
                processor->moveBackToParent();
                break;
            }
        }
        //second searching supported drivers
        if(!found){
            bool res = insertDriver(0, info, name);
            if(res){
                found = true;
                processor->moveToInstance("content", "");
                _drivers.values().at(0)->load(processor);
                processor->moveBackToParent();
            }
        }
        if(!found){
            QMessageBox::warning(NULL, tr("load error"),tr("can't support the driver %1!").arg(name),QMessageBox::Ok, QMessageBox::Cancel);
        }
        processor->moveBackToParent();
    }
}

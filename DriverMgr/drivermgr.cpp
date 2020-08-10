#include "../Interface/Drivers/drivermgr.h"
#include "../Interface/Drivers/idriver.h"
#include "driverconfigmodel.h"
#include "driverconfigui.h"

#include <QThread>
#include <QPluginLoader>
#include <QMessageBox>
#include <QApplication>
#include <QDir>

DriverConfigMgr* DriverConfigMgr::_singleton = nullptr;
DriverConfigMgr* DriverConfigMgr::Instance(){
    if(_singleton == nullptr){
        _singleton = new DriverConfigMgr();
    }
    return _singleton;
}

DriverConfigMgr::DriverConfigMgr(QObject *parent) : QObject(parent)
{
    _pModel= nullptr;
    _pUI = nullptr;

    loadDrivers();
}

DriverConfigMgr::~DriverConfigMgr(){
    initalize();

    if(_pModel) {
        _pModel->deleteLater();
        _pModel = nullptr;
    }
    if(_pUI) {
        _pUI->deleteLater();
        _pUI = nullptr;
    }
}

void DriverConfigMgr::initalize(){
    _drivers.clear();
    foreach(iDriverConfig* d, _drivers.values()) {
        delete d;
    }
    _drivers.clear();

    _supportedDriversInfo.clear();
    foreach(QPluginLoader* loader, _supportedDriversLoader){
        loader->deleteLater();
        _supportedDriversLoader.removeOne(loader);
    }

}

void DriverConfigMgr::loadDrivers(){

    initalize();
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

bool DriverConfigMgr::setAvailableDriver(QPluginLoader* pLoader){
    auto driver = dynamic_cast<iDriverConfig * >(pLoader->instance());
    if(driver){
        _supportedDriversInfo.append( *(driver->driverInfo()) );
        _supportedDriversLoader.append(pLoader);
        delete driver;
        //success
        return true;
    }
    //failed
    return false;
}

QString DriverConfigMgr::genDriverName(const QString& testName) {
    int i=0;
    QString retName = testName;
    while( contains( retName) ){
        i++;
        retName = testName + QString("_%1");
        retName = retName.arg( QString::number(i) );
    }
    return retName;
}

QList<DriverInfo> DriverConfigMgr::supportDriversInfo(void) const {
    return _supportedDriversInfo;
}

QList<iDriverConfig*> DriverConfigMgr::driverList(void) const {
    return _drivers.values();
}

QStringList DriverConfigMgr::driverNames(void) const {
    return _drivers.keys();
}

QStringList DriverConfigMgr::driverNameRelateToType(const QString& driverType) const {
    QStringList ret;
    foreach(auto d, _drivers.values()){
        if(driverType == d->driverInfo()->driverType){
            ret<<_drivers.key(d);
        }
    }
    return ret;
}
void DriverConfigMgr::setDriverName(int pos, const QString& driverName) {
    if(pos<0 || pos>=_drivers.size()) return;
    QMap<QString, iDriverConfig*>::Iterator it;
    it = _drivers.begin();
    it += pos;
    QString oldKey = it.key();
    auto d = it.value();
    _drivers.remove(oldKey);
    _drivers.insert(genDriverName( driverName ), d);
    emit driverListChanged();
}

iDriverConfig* DriverConfigMgr::driver(const QString& driverName) const {
    if(!_drivers.keys().contains(driverName) )
        return nullptr;
    return _drivers[driverName];
}

bool DriverConfigMgr::contains(const QString& driverName) const {
    foreach (QString n, _drivers.keys()) {
        if(driverName == n){
            return true;
        }
    }
    return false;
}

QWidget *DriverConfigMgr::showUI(bool show, QWidget* parent) {
    if(_pUI == nullptr){
        _pModel = new DriverConfigModel();
        _pUI = new DriverConfigUI(_pModel, parent);
    }
    if(show){
        _pUI->show();
    }else{
        _pUI->hide();
    }
    return _pUI;
}

bool DriverConfigMgr::insertDriver(int pos, const DriverInfo& info, const QString& driverName){
    if(pos <0 || pos >_drivers.size()) return false;
    QString name = driverName;
    if(name.isEmpty()) name = tr("新驱动 ");
    QMap<QString, iDriverConfig *>::Iterator it;
    it = _drivers.begin();
    it += pos;

    for(int i=0;i<_supportedDriversInfo.count();++i){
        if(_supportedDriversInfo[i] == info){
            iDriverConfig* d = dynamic_cast<iDriverConfig*>(_supportedDriversLoader[i]->instance() );
            if(d){
                //do some connections here
                _drivers.insert(it,genDriverName( name ),d);
                emit driverListChanged();
                return true;
            }
        }
    }
    return false;
}

bool DriverConfigMgr::removeDriver(const QString& driverName){
    iDriverConfig* d = _drivers.take( driverName );
    if(d){
        delete d;
        emit driverListChanged();
        return true;
    }
    //not found
    return false;
}

bool DriverConfigMgr::removeDriver(int pos){
    if(pos <0 || pos >=_drivers.size()) return false;
    return removeDriver( _drivers.keys().at(pos) );
}

void DriverConfigMgr::save(iLoadSaveProcessor* processor){
    int num =_drivers.count();
    QString name;
    iDriverConfig* pDriver;
    QMap<QString, iDriverConfig *>::const_iterator it;
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

void DriverConfigMgr::load(iLoadSaveProcessor* processor){
    int ret=0,num =0;
    DriverInfo info;
    QString name;
    iDriverConfig* pDriver;
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

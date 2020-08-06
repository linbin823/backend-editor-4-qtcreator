#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include <QDateTime>
#include <QHostAddress>
#include "simdriverui.h"
#include "simdriver.h"
#include "loadsaveprocessorxml.h"

SimDriver::SimDriver(QObject *parent) : iDriver(parent),
    _udpdatabaseAddress( QHostAddress("127.0.0.1")),
    _stationID(0x05),
    _port(5656),
    _pUdpSocket(nullptr),
    _pUI(nullptr),
    _pWatchDogTimer(nullptr),
    _scheduleTimer(nullptr)
{
    //default
    if(QNetworkInterface::allInterfaces().size() == 0){
        emit msgEventString(tr("电脑上找不到网卡") );
    }
    else{
        _networkInterface = QNetworkInterface::allInterfaces().at(0);
    }


    _setState(STAT_STOP);
}
SimDriver::~SimDriver(){
    stop();
    if(_pUdpSocket!=nullptr){
        _pUdpSocket->deleteLater();
    }
    if(_pUI != nullptr){
        _pUI->deleteLater();
    }
}

DriverInfo* SimDriver::driverInfo()const{
    DriverInfo* info = new DriverInfo();
    info->driverType = "Simulation Driver";
    info->driverManufacturer = "Richard Lin";
    info->description = "Driver simulation\nDate:2020.8";
    info->majorVersion = QString::number(0);
    info->minorVersion = QString::number(0);
    info->microVersion = QString::number(1);
    return info;
}

void SimDriver::save(iLoadSaveProcessor* processor){
    processor->writeValue("udpdatabaseAddress",_udpdatabaseAddress);
    processor->writeValue("port",_port);
    QString networkInterfaceName = _networkInterface.name();
    processor->writeValue("networkInterface", networkInterfaceName );
}

void SimDriver::load(iLoadSaveProcessor* processor){
    processor->readValue("udpdatabaseAddress",_udpdatabaseAddress);
    processor->readValue("port",_port);
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
    if(allInterfaces.size() == 0){
        emit msgEventString(tr("电脑上找不到网卡") );
        return;
    }
    QString networkInterfaceName;
    processor->readValue("networkInterface", networkInterfaceName );
    bool foundInterface = false;
    foreach (QNetworkInterface iface, allInterfaces) {
        if(iface.name() == networkInterfaceName){
            foundInterface = true;
            _networkInterface = iface;
            break;
        }
    }
    if(!foundInterface){
        emit msgEventString(tr("电脑上找不到名为“%1”的网卡").arg(networkInterfaceName) );
        _networkInterface = allInterfaces.at(0);
    }
    if(state() == STAT_RUN){
        restart();
    }
}

int SimDriver::stop(){
    if(_scheduleTimer!=nullptr){
        _scheduleTimer->stop();
        _scheduleTimer->deleteLater();
        _scheduleTimer=nullptr;
    }
    if(_pWatchDogTimer!=nullptr){
        _pWatchDogTimer->stop();
        _pWatchDogTimer->deleteLater();
        _pWatchDogTimer=nullptr;
    }
    if(_pUdpSocket != nullptr){
        _pUdpSocket->close();
        _pUdpSocket->deleteLater();
        _pUdpSocket=nullptr;
        _setState(STAT_STOP);
        emit msgEventString(tr("停止成功！") );
    }
    return 0;
}

int SimDriver::restart(){
    stop();
    return start();
}

QList<int> SimDriver::availableRWStrategy(void)const{
    QList<int> ret;
    ret<<iTagInfo::RWS_DISABLE<<iTagInfo::RWS_WRITE_ONLY;
    return ret;
}

QList<int> SimDriver::availableRWStrategy(const QString& addr)const{
    QList<int> ret;
    ret<<iTagInfo::RWS_DISABLE;
    if(isAddressCorrect(addr))
        ret<<iTagInfo::RWS_WRITE_ONLY;
    return ret;
}

bool SimDriver::isAddressCorrect(const QString& addr)const{
    bool ret;
    addr.toUInt(&ret,16);
    return ret;
}

QString SimDriver::addressErrorString(const QString& addr) const{
    if(isAddressCorrect(addr)){
        return QString();
    }
    return tr("地址必须是十进制数字！");
}

int SimDriver::setRelatedTagAddresses(const QList<TagAddress *>& relatedTagAddresses){
    int count =0;
    _relatedTags.clear();
    foreach (TagAddress* it, relatedTagAddresses) {
        if(it->RWStrategy == iTagInfo::RWS_WRITE_ONLY){
            _relatedTags<<it;
            count++;
        }
    }
    qSort(_relatedTags.begin(), _relatedTags.end(), compareTagAddress);
    return count;
}

QWidget* SimDriver::showUI(bool show, QWidget *parent){
    if(_pUI == nullptr){
        _pUI = new SimDriverUI(this, parent);
    }
    if(show){
        _pUI->show();
    }else{
        _pUI->hide();
    }
    return _pUI;
}


bool SimDriver::compareTagAddress(const TagAddress *t1, const TagAddress *t2){
    return t1->address < t2->address;
}

#ifndef IGLOBAL_H
#define IGLOBAL_H

#include "AlarmData/ialarmdata.h"
#include "Drivers/drivermgr.h"
#include "Tags/tagmgr.h"

class iGlobal{

public:
    explicit iGlobal(){}
    virtual ~iGlobal(){}

    virtual TagMgr* tagMgr()const = 0;
    virtual iDriverRuntimeMgr* driverRuntimeMgr()const = 0;
    virtual DriverConfigMgr* driverConfigMgr()const = 0;

#ifdef APPSERVER_WITH_ALARMDATA
    virtual iAlarmData* AlarmData()const=0;
#endif

#ifdef APPSERVER_WITH_WEBSERVER
//    iAppWebServer* AppWebServer()const=0;
#endif
};
#endif // IGLOBAL_H

import QtQml 2.0

QtObject {

    property string driverName: ""
    property DriverInfo driverinfo: DriverInfo{}

    enum EnumStates{
        STAT_ABOUT_TO_RUN,
        STAT_RUN,
        STAT_ABOUT_TO_STOP,
        STAT_STOP,
        STAT_ERR
    }

    property int state: Driver.STAT_STOP

    /*
      Internal usage
    */
    property list<QtObject> _tagAddresses
}

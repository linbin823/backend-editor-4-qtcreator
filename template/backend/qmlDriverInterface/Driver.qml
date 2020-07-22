import QtQml 2.0
import "../qmlTagInterface"

QtObject {
    property string driverName;
    property string driverType;
    property string driverManufacturer;
    property string description;
    property string majorVersion;
    property string minorVersion;
    property string microVersion;

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

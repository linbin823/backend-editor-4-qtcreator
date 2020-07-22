import QtQml 2.0

QtObject {

    enum EnumRWStrategyCode{
        RWS_DISABLE,
        RWS_READ_ONLY,
        RWS_WRITE_ONLY,
        RWS_READ_WRITE
    }
    /*!
        1. DriverName(Data Driver's name). Will automatically connect to related driver during runtime according to this name.
        2. Address. Special name, defined by specific driver.
        3. RWStrategy.
    */
    property string driverName: ""
    property string address: ""
    //TODO: not implemented
    property int rwstrategy: TagAddress.RWS_READ_WRITE;


    /*!
      Internal usage.
    */
    property Tag _tag
    property bool _newValuetoWrite: false

    property int _sourceId: 0
    function _binding(sources) {
        _tag.rawValue =  Qt.binding(function(){ return sources[_sourceId].value })
    }
}

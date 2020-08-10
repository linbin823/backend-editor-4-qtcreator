import QtQuick 2.0

Item {

    property var availDriversType: ["SimDriver"]

    function initiatedDriversNames() {
        var ret = []
        for (var i = 0; i < _initiatedDrivers.length; i++) {
            ret.push(_initiatedDrivers[i].driverName)
        }
        return ret
    }

    /*!
      Internal usage
    */
    default property list<QtObject> _initiatedDrivers


    function init(tags) {
        for (var i = 0; i < _initiatedDrivers.length; i++) {
            var addresses = tags._getTagAddressesbyDriverName(_initiatedDrivers[i].driverName)
//            console.log("DriverMgr::init()  " + _initiatedDrivers[i].driverName + "'s addresses: " + addresses.length)
            _initiatedDrivers[i]._tagAddresses = addresses
            _initiatedDrivers[i].start();
        }
    }
}
import QtQml 2.0
import "../DriversInterface"

DriverRuntime {
    driverinfo.driverType: "SimDriver"
    driverinfo.driverManufacturer: "Richard Lin"
    driverinfo.description: "2020.8.11 ver 1.0.0"
    driverinfo.majorVersion: "1"
    driverinfo.minorVersion: "0"
    driverinfo.microVersion: "0"

    /*!
      should implement start() stop() restart() functions for each drive.
    */
    function start() {
        if (state === DriverRuntime.STAT_RUN) return
        for (var i = 0; i < _sources.length; i++) {
            var sourceAddress = _sources[i].sourceAddress
            for (var j = 0; j < _tagAddresses.length; j++) {
                var tagAddress = _tagAddresses[j].address
                if (sourceAddress === tagAddress) {
//                    console.log("SimDriver::start " + "j="+ j + ": " + _tagAddresses[j]._tag.rawValue + "; i=" + i + ": " + _sources[i].value)
                    _tagAddresses[j]._sourceId = i
                    _tagAddresses[j]._binding(_sources)
                }
            }
            _sources[i].running = true
        }
        state = DriverRuntime.STAT_RUN
    }

    function stop() {
        if (state === DriverRuntime.STAT_STOP) return
        for (var i = 0; i < _sources.length; i++) {
            _sources.running = false
        }
        state = DriverRuntime.STAT_STOP
    }

    function restart() {
        start()
        stop()
    }

    /*!
      Internal usage
    */
    default property list<QtObject> _sources

}

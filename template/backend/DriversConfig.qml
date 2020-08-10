pragma Singleton
import QtQml 2.0
import QtQuick 2.0
import "TagsInterface"
import "DriversInterface"

import "SimDriverRuntime"

DriversRuntimeMgr {

    SimDriver {
        driverName: "Sim"

        Counter {
            sourceAddress: "C1"
            maxValue: 10
            minValue: 0
            interval: 1000
            type: Counter.COUNT_CYCLE
        }

        Counter {
            sourceAddress: "C2"
            maxValue: 1000
            minValue: 0
            interval: 100
            type: Counter.COUNT_DOWN
        }
    }
}

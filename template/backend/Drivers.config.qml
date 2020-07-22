pragma Singleton
import QtQuick 2.0
import "qmlTagInterface"
import "qmlDriverInterface"
import "simDriver"

DriversMgr {

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

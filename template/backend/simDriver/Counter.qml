import QtQml 2.0

Timer {
    repeat: true
    interval: 1000
    running: false

    enum EnumCounterType {
        COUNT_UP,
        COUNT_DOWN,
        COUNT_CYCLE
    }
    property string sourceAddress: ""
    property int maxValue: 100
    property int minValue: 0
    property int value: 0

    property int type: Counter.COUNT_UP

    /*!
      Internal usage
    */
    property int direction: Counter.COUNT_UP

    onTypeChanged: {
        if (type === Counter.COUNT_UP)
            direction = Counter.COUNT_UP
        else if (type === Counter.COUNT_DOWN)
            direction = Counter.COUNT_DOWN
        else
            direction = Counter.COUNT_UP
    }

    onTriggered: {
        if (value >= maxValue && direction === Counter.COUNT_UP) {
            if (type === Counter.COUNT_UP) {
                value = minValue
            } else if (type === Counter.COUNT_CYCLE){
                value = maxValue - 1
                direction = Counter.COUNT_DOWN
            } else {
                value = maxValue - 1
            }
        }
        else if (value <= minValue && direction === Counter.COUNT_DOWN) {
            if (type === Counter.COUNT_DOWN) {
                value = maxValue
            } else if (type === Counter.COUNT_CYCLE) {
                value = minValue + 1
                direction = Counter.COUNT_UP
            } else {
                value = minValue + 1
            }
        }
        else {
            if (direction === Counter.COUNT_UP) {
                value ++
            } else {
                value --
            }
        }
    }
}

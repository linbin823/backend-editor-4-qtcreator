import QtQml 2.0

QtObject {
    /*!
      Tag information collection
    */
    property string tagName: ""
    property string description: ""
    property string extraInfo: ""
    property string projectName: ""
    property string systemName: ""
    property string zoneName: ""

    enum EnumTypeCode{
        TYPE_DEFAULT=0x00,
        TYPE_BOOL,
        TYPE_INT16,
        TYPE_INT32,
        TYPE_INT64,
        TYPE_UINT16,
        TYPE_UINT32,
        TYPE_UINT64,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_DATETIME,
        TYPE_TIME,
        TYPE_DATE
    }

    property int tagType: Tag.TYPE_DEFAULT
    property string unit: ""

    /*!
        value updated by drivers, value set by UI components, value quality
    */
    property real scaleRatio: 1.0
    property real scaleOffset: 0.0
    property real deadband: 0.01
    //drivers modify "rawValue" property.
    property variant rawValue: 0
    onRawValueChanged: {
        //caculate actual value
        var newValue = rawValue * scaleRatio + scaleOffset
        if (Math.abs(value - newValue) > deadband) {
            value = newValue;

            //onValueChanged will be called
            //which will notificate all drivers that this has been changed

            //caculate lastUpdateTime
            lastUpdateTime = new Date()
        }
    }

    //UI components modify the "value" property
    property variant value: 0
    onValueChanged: {
        //caculate actual value
        var newRawValue = (value - scaleOffset) / scaleRatio
        if (Math.abs(rawValue - newRawValue) > deadband) {
            rawValue = newRawValue;

            //notificate all drivers that this rawValue changed
            for (var address in _addresses) {
                if (address.rwstrategy === TagAddress.RWS_WRITE_ONLY || address.rwstrategy === TagAddress.RWS_READ_WRITE)
                    address._newValuetoWrite = true;
            }
        }
    }

    enum EnumQualityCode{
        QUALITY_OK,
        QUALITY_TIMEOUT,
        QUALITY_BAD
    }

    property date lastUpdateTime: new Date()
    property int quality: Tag.QUALITY_OK

    /*!
      Internal usage.
      Tag addresses. Each tag could connect with multipule drivers and hence contains multiple address.
    */
    default property list<QtObject> _addresses

    on_AddressesChanged: {
        for (var i = 0; i < _addresses.length; i++) {
            _addresses[i]._tag = this
        }
    }

    function getTagAddressbyDriverName(driverName) {
        for (var i = 0; i < _addresses.length; i++) {
            if (_addresses[i].driverName === driverName)
                return _addresses[i]
        }
    }
}

pragma Singleton
import QtQml 2.0
import QtQuick 2.0
import "TagsInterface"

TagsMgr {
    Tag {
        id: t0001
        tagName: "testListedTag1"
        TagAddress {
            driverName: "Sim"
            address: "C1"
        }
    }

    Tag {
        id: t0002
        tagName: "testListedTag2"
        TagAddress {
            driverName: "Sim"
            address: "C2"
        }
    }

    Tag {
        id: t0003
        tagName: "testListedTag3"
    }

    Tag {
        id: t0004
        tagName: "testListedTag4"
    }

    property Tag testListedTag1: t0001
    property Tag testListedTag2: t0002
    property Tag testListedTag3: t0003
    property Tag testListedTag4: t0004

    Component.onCompleted: {
        Drivers.init(this)
    }
}

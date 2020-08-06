pragma Singleton

import QtQuick 2.15
import "qmlTagInterface"

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

    property QtObject testListedTag1: t0001
    property QtObject testListedTag2: t0002
    property QtObject testListedTag3: t0003
    property QtObject testListedTag4: t0004
}

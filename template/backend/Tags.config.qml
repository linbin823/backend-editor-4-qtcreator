pragma Singleton

import QtQuick 2.15
import "qmlTagInterface"

TagsMgr {
    Tag {
        tagName: "testListedTag1"
        TagAddress {
            driverName: "Sim"
            address: "C1"
        }
    }

    Tag {
        tagName: "testListedTag2"
        TagAddress {
            driverName: "Sim"
            address: "C2"
        }
    }

    Tag {
        tagName: "testListedTag3"
    }

    Tag {
        tagName: "testListedTag4"
    }
}

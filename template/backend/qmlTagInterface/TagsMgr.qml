import QtQml 2.0

QtObject {

    /*!
      fetch tag by name
    */
    function getTagbyName(name) {
        for (var i = 0; i < _tagsList.length; i++) {
//            console.log(tagsList.length + " + " + name + " + " + tagsList[0].tagName)
            if (_tagsList[i].tagName === name)
                return _tagsList[i];
        }
    }

    /*!
      Internal usage.
    */
    default property list<QtObject> _tagsList

    function _allTagNames() {
        var ret = []
        for (var i = 0; i < _tagsList.length; i++) {
            ret.push(_tagsList[i].tagName)
        }
        return ret
    }

    function _getTagAddressesbyDriverName(driverName) {
        var ret = []
        for (var i = 0; i < _tagsList.length; i++) {
//            console.log(_tagsList.length + " + " + driverName + " + " + _tagsList[0].tagName)
            var tagAddress = _tagsList[i].getTagAddressbyDriverName(driverName)
            if(tagAddress !== undefined) {
                ret.push(tagAddress)
            }
        }
        return ret
    }
}

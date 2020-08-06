#include "modbusslaveprotocoldriver.h"
#include "modbusslaveprotocoldriverui.h"
#include <QMap>
#include <QTimer>
#include <QVariant>
#include <QHostAddress>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QCoreApplication>
#include <QCoreApplication>

#ifdef Q_OS_LINUX
#endif
ModbusSlaveProtocolDriver::ModbusSlaveProtocolDriver(QObject *parent) :
    iDriver(parent),
    _pModbusDevice(nullptr),
    _pDataMapping(nullptr),
    _pUI(nullptr)
{
    _supportedType<<Coils <<DiscreteInputs <<InputRegisters <<HoldingRegisters;
    _supportedRWS<< iTagInfo::RWS_READ_ONLY<< iTagInfo::RWS_WRITE_ONLY<< iTagInfo::RWS_READ_WRITE;
    _initParameters();
    _initDataUnites();
    _retryCount = 0;
    _setState(STAT_STOP);
}

ModbusSlaveProtocolDriver::~ModbusSlaveProtocolDriver() {
    stop();
}

void ModbusSlaveProtocolDriver::_initParameters() {
    _parameters[ConnectionType] = Serial;
    _parameters[SlaveStationID] = 1;
    _parameters[SerialModel] = Half485;

    QList<QSerialPortInfo> all = QSerialPortInfo::availablePorts();
    if(all.size() != 0){
        _parameters[SerialPortNameParameter] = all.at(0).systemLocation();
    }else{
        _parameters[SerialPortNameParameter] = NO_AVAILABLE_PORT_NAME;
    }
    _parameters[SerialParityParameter] = NoParity;
    _parameters[SerialBaudRateParameter] = Baud9600;
    _parameters[SerialDataBitsParameter] = Data8;
    _parameters[SerialStopBitsParameter] = OneStop;
    _parameters[NetworkPort] = 502;
    _parameters[NetworkMaxConnections] = 10;
    _parameters[ResponseTimeOutMS] = 1000;
    _parameters[NumberOfRetries] = 3;
    _parameters[SerialInterFrameDelayMS] = 50;
    _parameters[DataAddressing] = ZeroBased;
    _parameters[Endianness] = BigEnd;
    _parameters[CoilStartAddress] = 0;
    _parameters[CoilNumber] = 16;
    _parameters[DiscreteInputsStartAddress] = 10000;
    _parameters[DiscreteInputsNumber] = 16;
    _parameters[InputRegistersStartAddress] = 20000;
    _parameters[InputRegistersNumber] = 16;
    _parameters[HoldingRegistersStartAddress] = 40000;
    _parameters[HoldingRegistersNumber] = 16;
    /* Modbus_Application_Protocol_V1_1b.pdf (chapter 6 section 1 page 12)
     * Quantity of Coils to read (2 bytes): 1 to 2000 (0x7D0)
     * (chapter 6 section 11 page 29)
     * Quantity of Coils to write (2 bytes): 1 to 1968 (0x7B0)
     */
//    _parameters[MaxReadInputPerMessage] = 1000;//<=2000
//    _parameters[MaxWriteCoilPerMessage] = 1000;//<=1968
    /* Modbus_Application_Protocol_V1_1b.pdf (chapter 6 section 3 page 15)
     * Quantity of Registers to read (2 bytes): 1 to 125 (0x7D)
     * (chapter 6 section 12 page 31)
     * Quantity of Registers to write (2 bytes) 1 to 123 (0x7B)
     * (chapter 6 section 17 page 38)
     * Quantity of Registers to write in R/W registers (2 bytes) 1 to 121 (0x79)
     */
//    _parameters[MaxReadRegistersPerMessage] = 100;//<=125
//    _parameters[MaxWriteRegistersPerMessage] = 100;//<=123
//    _parameters[MaxReadWriteRegistersPerMessage] = 100;//<=121
}

void ModbusSlaveProtocolDriver::_initDataUnites() {
    _tagAddresses.clear();
    _dataUnits.clear();
}

DriverInfo ModbusSlaveProtocolDriver::driverInfo()const {
    DriverInfo info;
    info.driverType = "Modbus Slave/Server";
    info.driverManufacturer = "Modicon";
    info.description = "Modbus Serial Slave \nModbus Tcp Server \nDate:2018.4";
    info.majorVersion = QString::number(0);
    info.minorVersion = QString::number(0);
    info.microVersion = QString::number(1);
    return info;
}

void ModbusSlaveProtocolDriver::save(iLoadSaveProcessor* processor) {
    int num =_parameters.count();
    int key;
    QString value;
    QMap<int,QVariant>::const_iterator it;
    processor->writeValue("parametersNumber", num );
    int i=0;
    for(it = _parameters.constBegin(); it != _parameters.constEnd(); ++it,i++){
        processor->moveToInstance("parameters",QString::number(i));
        key = it.key();
        value=it.value().toString();
        processor->writeValue("key",key );
        processor->writeValue("value",value );
        processor->moveBackToParent();
    }
}

void ModbusSlaveProtocolDriver::load(iLoadSaveProcessor* processor) {
    int ret=0,num =0;
    int key;
    QString value;
    ret = processor->readValue("parametersNumber", num);
    if(ret !=0 ) return;//not found num
    for(int i=0; i<num; i++){
        ret = processor->moveToInstance("parameters",QString::number(i));
        //not found Instance
        if(ret !=0 ) {
            processor->moveBackToParent();
            continue;
        }
        ret = processor->readValue("key",key);
        ret = processor->readValue("value",value);
        _parameters.insert(key, QVariant(value));
        processor->moveBackToParent();
    }

    //verify the port name
    QList<QSerialPortInfo> all = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo i, all) {
        if(i.systemLocation() == _parameters[SerialPortNameParameter].toString()) {
            return;
        }
    }
    if(all.size() != 0){
        _parameters[SerialPortNameParameter] = all.at(0).systemLocation();
    }else{
        _parameters[SerialPortNameParameter] = NO_AVAILABLE_PORT_NAME;
    }
}

int ModbusSlaveProtocolDriver::start() {
    if(_state != STAT_STOP || _pModbusDevice != nullptr) {
        return -1;
    }

    //creat device
    int type = _parameters[ConnectionType].toInt();
    if(type == Serial){
        if (_parameters[SerialPortNameParameter] == NO_AVAILABLE_PORT_NAME) {
            emit msgEventString(tr("启动失败。没有串口设备"));
            _setState(STAT_ERR);
            return -2;
        }
        _pModbusDevice = modbus_new_rtu(_parameters[SerialPortNameParameter].toString().toUtf8().data(),
                                        _parameters[SerialBaudRateParameter].toInt(),
                                        _parameters[SerialParityParameter].toInt(),
                                        _parameters[SerialDataBitsParameter].toInt(),
                                        _parameters[SerialStopBitsParameter].toInt() );
    }else if(type == Tcp){
        _pModbusDevice = modbus_new_tcp(NULL, _parameters[NetworkPort].toInt());
    }

    if (_pModbusDevice == nullptr) {
        emit msgEventString(tr("启动失败。无法建立端口对象！%1").arg(modbus_strerror(errno)));
        _setState(STAT_ERR);
        return -2;
    }

    //common setup
    modbus_set_debug(_pModbusDevice, FALSE);
    modbus_set_slave(_pModbusDevice, _parameters[SlaveStationID].toInt());
    modbus_rtu_set_serial_mode (_pModbusDevice, _parameters[SerialModel].toInt());

    uint32_t sec, usec;
    sec = _parameters[SerialInterFrameDelayMS].toUInt() / 1000;
    usec = _parameters[SerialInterFrameDelayMS].toUInt() % 1000 * 1000;
    modbus_set_byte_timeout (_pModbusDevice, sec, usec );
    sec = _parameters[ResponseTimeOutMS].toUInt() / 1000;
    usec = _parameters[ResponseTimeOutMS].toUInt() % 1000 * 1000;
    modbus_set_response_timeout(_pModbusDevice, sec, usec );

    //we handle error in our side
//    modbus_set_error_recovery(_pModbusDevice, modbus_error_recovery_mode(MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL));

    //start server
    if (_parameters[ConnectionType] == Tcp) {
            int s = modbus_tcp_listen(_pModbusDevice, _parameters[NetworkMaxConnections].toInt());
            if ( s == -1 ) {
                emit msgEventString(tr("启动失败。监听端口失败！%1。请检查权限").arg(modbus_strerror(errno)));
                _setState(STAT_ERR);
                modbus_free(_pModbusDevice);
                return -3;
            }
    } else if (_parameters[ConnectionType] == Serial) {
        if (modbus_connect(_pModbusDevice) == -1) {
            emit msgEventString(tr("启动失败。连接串口失败！%1。请检查权限").arg(modbus_strerror(errno)));
            _setState(STAT_ERR);
            modbus_free(_pModbusDevice);
            return -4;
        }
    }

    //init dataMap
    _pDataMapping = modbus_mapping_new_start_address(_parameters[CoilStartAddress].toInt(), _parameters[CoilNumber].toInt(),
                                                       _parameters[DiscreteInputsStartAddress].toInt(), _parameters[DiscreteInputsNumber].toInt(),
                                                       _parameters[HoldingRegistersStartAddress].toInt(), _parameters[HoldingRegistersNumber].toInt(),
                                                       _parameters[InputRegistersStartAddress].toInt(), _parameters[InputRegistersNumber].toInt());

    if (_pDataMapping == nullptr) {
        emit msgEventString(tr("启动失败。无法初始化内存！%1。").arg(modbus_strerror(errno)));
        _setState(STAT_ERR);
        modbus_free(_pModbusDevice);
        return -5;
    }

    _retryCount = 0;
    _setState(STAT_RUN);

    if (_parameters[ConnectionType] == Tcp) {
        QTimer::singleShot(0, Qt::CoarseTimer, this, SLOT(_tcpProcess()));
    } else if (_parameters[ConnectionType] == Serial) {
        QTimer::singleShot(0, Qt::CoarseTimer, this, SLOT(_serialProcess()));
    }

    return 0;
}

int ModbusSlaveProtocolDriver::stop() {
//    clean up should be done in process funcions.
//    if (_pModbusDevice) {
//        modbus_close(_pModbusDevice);
//        modbus_free(_pModbusDevice);
//        _pModbusDevice = nullptr;
//    }
//    if(_pDataMapping){
//        modbus_mapping_free(_pDataMapping);
//        _pDataMapping = nullptr;
//    }
    _setState(STAT_ABOUT_TO_STOP);

    QTime maxWaitTime;
    maxWaitTime.start();
    while ( state() != STAT_STOP && maxWaitTime.elapsed() <= 1000){
        QCoreApplication::processEvents();
    }
    if (state() != STAT_STOP) return -1;
    return 0;
}

int ModbusSlaveProtocolDriver::restart() {
    if (stop() == 0) return start();
    return -1;
}

QList<int> ModbusSlaveProtocolDriver::availableRWStrategy(void)const {
    QList<int> ret;
    ret<<iTagInfo::RWS_DISABLE<<iTagInfo::RWS_READ_ONLY
        <<iTagInfo::RWS_READ_WRITE<<iTagInfo::RWS_WRITE_ONLY;
    return ret;
}

QList<int> ModbusSlaveProtocolDriver::availableRWStrategy(const QString& addr)const {
    QList<int> ret;
    ret<<iTagInfo::RWS_DISABLE;
    if(isAddressCorrect(addr)){
        switch(_addressArea(addr)){
        case Coils:
        case HoldingRegisters:
            ret<<iTagInfo::RWS_READ_ONLY<<iTagInfo::RWS_READ_WRITE<<iTagInfo::RWS_WRITE_ONLY;
            break;
        case DiscreteInputs:
        case InputRegisters:
            ret<<iTagInfo::RWS_READ_ONLY;
            break;
        default:
            break;
        }
    }
    return ret;
}

bool ModbusSlaveProtocolDriver::isAddressCorrect(const QString& addr) const{
    int area = _addressArea(addr);
    if(_supportedType.contains(enumDataType(area))){
        int offset = _addressOffset(addr);
        if(_parameters.value(DataAddressing) == OneBased){
            if(offset>=1 && offset<=65536 )
                return true;
        }else if(_parameters.value(DataAddressing) == ZeroBased){
            if(offset>=0 && offset<=65535 )
                return true;
        }
    }
    return false;
}

QString ModbusSlaveProtocolDriver::addressErrorString(const QString& addr)const{
    int area = _addressArea(addr);
    if(!_supportedType.contains(enumDataType(area)))
        return tr("Area address should start with 0, 1, 3, 4!");
    int offset = _addressOffset(addr);
    if(offset < 0 && offset >= 65535 )
        return tr("Offset address is not decimal or is out of range!");
    return QString("No error");
}

int ModbusSlaveProtocolDriver::setRelatedTagAddresses(const QList<TagAddress *>& relatedTagAddresses){
    int lastState = state();
    if(lastState != STAT_STOP) {
        QMetaObject::invokeMethod(this, "stop");
    }
    int count =0;
    _initDataUnites();

    //sort tagAddress
    foreach (TagAddress* ta, relatedTagAddresses) {
        //check for correct address
        if(!isAddressCorrect(ta->address)) continue;
        if(ta->RWStrategy == iTagInfo::RWS_DISABLE) continue;

        _tagAddresses[enumDataType(_addressArea(ta->address))][iTagInfo::enumRWStrategyCode(ta->RWStrategy)]<<ta;
        count++;
    }

    //prepare dataUnits
    foreach (enumDataType type, _supportedType) {
        foreach (iTagInfo::enumRWStrategyCode strategy, _supportedRWS) {
            if(_tagAddresses.contains(type) && _tagAddresses[type].contains(strategy) && _tagAddresses[type][strategy].size() != 0) {
                //must sorted by raising address before using
                qSort(_tagAddresses[type][strategy].begin(), _tagAddresses[type][strategy].end(), compareTagAddress);
                _dataUnits[type][strategy] = _prepareDataUnit(_tagAddresses[type][strategy], type);
            }
        }
    }

    if(lastState == STAT_RUN) {
        QMetaObject::invokeMethod(this, "start");
    }
    return count;
}

bool ModbusSlaveProtocolDriver::compareTagAddress(const TagAddress* t1, const TagAddress* t2){
    QString offset1 = t1->address;
    QString offset2 = t2->address;
    return offset1.remove(0,1).toInt() < offset2.remove(0,1).toInt();
}

QList<ModbusDataUnit> ModbusSlaveProtocolDriver::_prepareDataUnit(const QList<TagAddress *> &list, int dataType) const {
    QList<ModbusDataUnit> ret;
    if (list.size()<=0) return ret;
    //init ModbusDataUnit
    ModbusDataUnit unit;

    //compute maxNumberPerMessage
    int maxNumberPerMessage = 10;
    switch (dataType) {
    case Coils:
        maxNumberPerMessage = qMin<int>(MODBUS_MAX_READ_BITS, MODBUS_MAX_WRITE_BITS);
        break;
    case DiscreteInputs:
        maxNumberPerMessage = MODBUS_MAX_READ_BITS;
        break;
    case InputRegisters:
        maxNumberPerMessage = MODBUS_MAX_READ_REGISTERS;
        break;
    case HoldingRegisters:
        maxNumberPerMessage = qMin<int>(MODBUS_MAX_WR_WRITE_REGISTERS, MODBUS_MAX_WR_READ_REGISTERS);
        break;
    default:
        break;
    }

    //get start offset(aka address) and lastOffset(aka address)
    int startOffset = _addressOffset( list.at(0)->address );
    int lastOffset = startOffset;
    //start main function
    switch (dataType) {
    case Coils:
    case DiscreteInputs:{
        int currentOffset;
        for (int i = 1; i<list.size(); ++i){
            currentOffset = _addressOffset( list.at(i)->address );
            if( currentOffset <= (lastOffset +1) &&
                    maxNumberPerMessage >= (lastOffset - startOffset + 1) ){
                //condition 1: address continueous
                //condition 2: not reach max number per message
                lastOffset = currentOffset;
                continue;
            }else{
                //seperate unit beacuse:
                //address not continueous or
                //max package length reached!
                unit.setStartAddress(startOffset);
                unit.setBitCount(lastOffset - startOffset + 1);
                ret.append(unit);
                startOffset = currentOffset;
                lastOffset = startOffset;
                continue;
            }
        }
        //finish last unit
        unit.setStartAddress(startOffset);
        unit.setBitCount(lastOffset - startOffset + 1);
        ret.append(unit);
        return ret;
    }
    case InputRegisters:
    case HoldingRegisters:{
        int currentOffset;
        bool doubleRegs = _isDWord(list.at(0));
        for (int i = 1; i<list.size(); ++i){
            currentOffset = _addressOffset( list.at(i)->address );
            if( //condition 1: address continueous
                (((!doubleRegs) && currentOffset<=(lastOffset +1)) || (doubleRegs && currentOffset<=(lastOffset +2)))
                    &&
                //condition 2: not reach max number per message
                maxNumberPerMessage >= (lastOffset - startOffset + 1) ){

                lastOffset = currentOffset;

                //feature for registers: 32bit length data could combine with two continueous 16bit registers!
                //update last tagAddress's condition
                doubleRegs = _isDWord(list.at(i));
                continue;
            }else{
                //seperate unit beacuse:
                //address not continueous or
                //max package length reached!
                unit.setStartAddress(startOffset);
                if(doubleRegs){
                    unit.setRegCount(lastOffset - startOffset + 2);
                }else{
                    unit.setRegCount(lastOffset - startOffset + 1);
                }
                ret.append(unit);
                startOffset = currentOffset;
                lastOffset = startOffset;

                //feature for registers: 32bit length data could combine with two continueous 16bit registers!
                //update last tagAddress's condition
                doubleRegs = _isDWord(list.at(i));
                continue;
            }
        }
        //finish last unit
        unit.setStartAddress(startOffset);
        if(doubleRegs){
            unit.setRegCount(lastOffset - startOffset + 2);
        }else{
            unit.setRegCount(lastOffset - startOffset + 1);
        }
        ret.append(unit);
        return ret;
    }
    default:
        return ret;
    }
}

bool ModbusSlaveProtocolDriver::_isDWord(TagAddress* t)const {
    if ( (t->tagInfo->type() == iTagInfo::TYPE_INT32) ||
        (t->tagInfo->type() == iTagInfo::TYPE_UINT32) ||
        (t->tagInfo->type() == iTagInfo::TYPE_FLOAT) ){
        return true;
    } else {
        return false;
    }
}

void ModbusSlaveProtocolDriver::showUI(bool show, QWidget* parent){
    if(_pUI == nullptr){
        _pUI = new ModbusSlaveProtocolDriverUI(this, parent);
    }
    if(show){
        _pUI->show();
    }else{
        _pUI->hide();
    }
}

void ModbusSlaveProtocolDriver::_setState(enum_states state){
    if(_state != state){
        _state = state;
        emit stateChanged(state);
    }
}
void ModbusSlaveProtocolDriver::_serialProcess() {
    while (state() == STAT_RUN) {




        QCoreApplication::processEvents(QEventLoop::AllEvents,1000);
    }
}

void ModbusSlaveProtocolDriver::_tcpProcess() {
    while (state() == STAT_RUN) {

    }
}

void ModbusSlaveProtocolDriver::_scheduleProcess() {
    if (_polling) return;
    if (!_pModbusDevice) return;
    if (_state != STAT_RUN) return;

    //start polling
    _polling = true;
    QVariant type = _parameters[ConnectionType];
    foreach (enumDataType dataType, _supportedType) {
        foreach (iTagInfo::enumRWStrategyCode strategy, _supportedRWS) {
            if( _dataUnits.contains(dataType) && _dataUnits[dataType].contains(strategy) && !_dataUnits[dataType][strategy].isEmpty()) {
                foreach(ModbusDataUnit unit, _dataUnits[dataType][strategy]) {
                    _unitProcesss(unit, dataType, strategy);
                }
            }
        }
    }
    //finish polling
    _polling = false;
}

void ModbusSlaveProtocolDriver::_unitProcesss(ModbusDataUnit unit, int dataType, int RWStrategy) {
    if (!_pModbusDevice) return;
    if (_state != STAT_RUN) return;

    int ret=0;
    switch(dataType) {
    case Coils:
        switch(RWStrategy) {
        case iTagInfo::RWS_READ_ONLY:
            ret = modbus_read_bits(_pModbusDevice, unit.startAddress() , unit.bitCount() , unit.bitData());
            if (ret < 0) {
                _errorProcess(tr("读取失败。%1").arg(modbus_strerror(errno)));
                break;
            } else {
                _retryCount = 0;
            }
            _readValues(_tagAddresses[Coils][iTagInfo::RWS_READ_ONLY], unit, dataType);
            break;
        case iTagInfo::RWS_WRITE_ONLY:
            _writeValues(_tagAddresses[Coils][iTagInfo::RWS_WRITE_ONLY], unit, dataType);
            ret = modbus_write_bits(_pModbusDevice, unit.startAddress(), unit.bitCount(), unit.bitData());
            if (ret < 0) {
                _errorProcess(tr("写入失败。%1").arg(modbus_strerror(errno)));
            } else {
                _retryCount = 0;
            }
            break;
        case iTagInfo::RWS_READ_WRITE:
            //Step 1: read field device's newValue to buffer(unit)
            ret = modbus_read_bits(_pModbusDevice, unit.startAddress() , unit.bitCount() , unit.bitData());
            if (ret < 0) {
                _errorProcess(tr("读取失败。%1").arg(modbus_strerror(errno)));
                break;
            } else {
                _retryCount = 0;
            }
            //Step 2: write value, which need to write to field device, to buffer
            _writeValues(_tagAddresses[Coils][iTagInfo::RWS_WRITE_ONLY], unit, dataType);
            //Step 3: read value, which is readable from buffer,
            _readValues(_tagAddresses[Coils][iTagInfo::RWS_READ_ONLY], unit, dataType);
            //Step 4: write buffer to field device
            ret = modbus_write_bits(_pModbusDevice, unit.startAddress(), unit.bitCount(), unit.bitData());
            if (ret < 0) {
                _errorProcess(tr("写入失败。%1").arg(modbus_strerror(errno)));
            } else {
                _retryCount = 0;
            }
            break;
        }
        break;
    case DiscreteInputs:
        //only RO for these inputs
        if (RWStrategy == iTagInfo::RWS_READ_ONLY) {
            ret = modbus_read_input_bits(_pModbusDevice, unit.startAddress(), unit.bitCount(), unit.bitData());
            if (ret < 0) {
                _errorProcess(tr("读取失败。%1").arg(modbus_strerror(errno)));
                break;
            } else {
                _retryCount = 0;
            }
            _readValues(_tagAddresses[DiscreteInputs][iTagInfo::RWS_READ_ONLY], unit, dataType);
        }
        break;
    case InputRegisters:
        //only RO for these inputs
        if (RWStrategy == iTagInfo::RWS_READ_ONLY) {
            ret = modbus_read_input_registers(_pModbusDevice, unit.startAddress(), unit.regCount(), unit.regData());
            if (ret < 0) {
                _errorProcess(tr("读取失败。%1").arg(modbus_strerror(errno)));
                break;
            } else {
                _retryCount = 0;
            }
            _readValues(_tagAddresses[InputRegisters][iTagInfo::RWS_READ_ONLY], unit, dataType);
        }
        break;
    case HoldingRegisters:
        switch(RWStrategy) {
        case iTagInfo::RWS_READ_ONLY:
            ret = modbus_read_registers(_pModbusDevice, unit.startAddress() , unit.regCount() , unit.regData());
            if (ret < 0) {
                _errorProcess(tr("读取失败。%1`").arg(modbus_strerror(errno)));
                break;
            } else {
                _retryCount = 0;
            }
            _readValues(_tagAddresses[HoldingRegisters][iTagInfo::RWS_READ_ONLY], unit, dataType);
            break;
        case iTagInfo::RWS_WRITE_ONLY:
            _writeValues(_tagAddresses[HoldingRegisters][iTagInfo::RWS_WRITE_ONLY], unit, dataType);
            ret = modbus_write_registers(_pModbusDevice, unit.startAddress(), unit.regCount(), unit.regData());
            if (ret < 0) {
                _errorProcess(tr("写入失败。%1").arg(modbus_strerror(errno)));
            } else {
                _retryCount = 0;
            }
            break;
        case iTagInfo::RWS_READ_WRITE:
            //Step 1: read field device's newValue to buffer(unit)
            ret = modbus_read_registers(_pModbusDevice, unit.startAddress() , unit.regCount() , unit.regData());
            if (ret < 0) {
                _errorProcess(tr("读取失败。%1").arg(modbus_strerror(errno)));
                break;
            } else {
                _retryCount = 0;
            }
            //Step 2: write value, which need to write to field device, to buffer
            _writeValues(_tagAddresses[Coils][iTagInfo::RWS_WRITE_ONLY], unit, dataType);
            //Step 3: read value, which is readable from buffer,
            _readValues(_tagAddresses[Coils][iTagInfo::RWS_READ_ONLY], unit, dataType);
            //Step 4: write buffer to field device
            ret = modbus_write_registers(_pModbusDevice, unit.startAddress(), unit.regCount(), unit.regData());
            if (ret < 0) {
                _errorProcess(tr("写入失败。%1").arg(modbus_strerror(errno)));
            } else {
                _retryCount = 0;
            }
            break;
        }
        break;
    default:
        break;
    }
}

int  ModbusSlaveProtocolDriver::_addressArea(const QString& addr)const {
    if(addr.isEmpty() || addr.isNull()) return -1;
    return addr.at(0).digitValue();
}

int  ModbusSlaveProtocolDriver::_addressOffset(const QString& addr)const {
    QString offsetString = addr;
    offsetString.remove(0,1);
    int offset = offsetString.toInt();
    if(_parameters.value(DataAddressing) == OneBased) offset--;
    return offset;
}

void ModbusSlaveProtocolDriver::_readValues(const QList<TagAddress *> &list, const ModbusDataUnit& unit, const int type) {
    int startAddress = unit.startAddress();
    int unitDataCount;
    switch (type) {
    case Coils:
    case DiscreteInputs:
        unitDataCount = unit.bitCount();
        break;
    case InputRegisters:
    case HoldingRegisters:
        unitDataCount = unit.regCount();
        break;
    default:
        return;
    }

    for(int index=0; index<unitDataCount; index++){
        int lastSpan = list.size() /2;
        int pos = list.size() / 2;
        while(1){
            TagAddress * t = list.at(pos);
            if( _addressOffset(t->address) == (startAddress+index) ){
                //found!
                _readValue(t, unit, index, type);
                //look forward for same address
                for(int i=1; (pos+i)<list.size();i++){
                    t = list.at(pos+i);
                    if( _addressOffset(t->address) == (startAddress+index) ){
                        _readValue(t, unit, index, type);
                        continue;
                    }
                    break;
                }
                //look backward for same address
                for(int i=1; (pos-i)>=0  ;i++){
                    t = list.at(pos-i);
                    if( _addressOffset(t->address) == (startAddress+index) ){
                        _readValue(t, unit, index, type);
                        continue;
                    }
                    break;
                }
                break;
            }else if( _addressOffset(t->address) > (startAddress+index) ){
                if(lastSpan == 0) break;
                lastSpan /= 2;
                if(lastSpan == 0) pos -= 1;
                else pos -= lastSpan;
            }else if( _addressOffset(t->address) < (startAddress+index)){
                if(lastSpan == 0) break;
                lastSpan /= 2;
                if(lastSpan == 0) pos += 1;
                else pos += lastSpan;
            }
        }
    }
}

void ModbusSlaveProtocolDriver::_readValue(TagAddress *t, const ModbusDataUnit& unit, uint index, const int type) {
    quint16 valueSmallAddr;
    quint16 valueBigAddr;
    quint32 uint32Value;
    float* floatValue;

    switch (type) {
    case Coils:
    case DiscreteInputs:
        t->tagInfo->updatevalue( unit.bitValue(index) );
        break;
    case InputRegisters:
    case HoldingRegisters:
        if( t->tagInfo->type() != iTagInfo::TYPE_INT32 &&
                t->tagInfo->type() != iTagInfo::TYPE_UINT32 &&
                t->tagInfo->type() != iTagInfo::TYPE_FLOAT) {
            t->tagInfo->updatevalue( unit.regValue(index) );
            break;
        } else {
            valueSmallAddr = unit.regValue(index);
            if ((index+1) >= unit.regCount())
                valueBigAddr = 0;
            else
                valueBigAddr = unit.regValue(index+1);

            if(t->tagInfo->type() == iTagInfo::TYPE_INT32 || t->tagInfo->type() == iTagInfo::TYPE_UINT32) {
                if (_parameters[Endianness] == BigEnd)
                    t->tagInfo->updatevalue( (quint32)((valueSmallAddr&0xFFFF)<<16) + (quint32)(valueBigAddr&0xFFFF) );
                else
                    t->tagInfo->updatevalue( (quint32)((valueBigAddr&0xFFFF)<<16) + (quint32)(valueSmallAddr&0xFFFF) );
            } else if (t->tagInfo->type() == iTagInfo::TYPE_FLOAT) {
                if(_parameters[Endianness] == BigEnd) {
                    uint32Value = (quint32)((valueSmallAddr&0xFFFF)<<16) + (quint32)(valueBigAddr&0xFFFF);
                    floatValue = (float*)&uint32Value;
                    t->tagInfo->updatevalue( *floatValue );
                } else {
                    uint32Value = (quint32)((valueBigAddr&0xFFFF)<<16) + (quint32)(valueSmallAddr&0xFFFF);
                    floatValue = (float*)&uint32Value;
                    t->tagInfo->updatevalue( *floatValue );
                }
            }
        }
        break;
    default:
        return;
    }
}

void ModbusSlaveProtocolDriver::_writeValues(const QList<TagAddress *>& list, ModbusDataUnit& unit, const int type) {
    int startAddress = unit.startAddress();
    int unitDataCount;
    switch (type) {
    case Coils:
    case DiscreteInputs:
        unitDataCount = unit.bitCount();
        break;
    case InputRegisters:
    case HoldingRegisters:
        unitDataCount = unit.regCount();
        break;
    default:
        return;
    }

    for(int index=0; index<unitDataCount; index++){
        int lastSpan = list.size() /2;
        int pos = list.size() / 2;
        while(1){
            TagAddress * t = list.at(pos);
            if( _addressOffset(t->address) == (startAddress+index) ){
                //found!
                _writeValue(t, unit, index, type);
                //look forward for same address
                for(int i=1; (pos+i)<list.size();i++){
                    t = list.at(pos+i);
                    if( _addressOffset(t->address) == (startAddress+index) ){
                        _writeValue(t, unit, index, type);
                        continue;
                    }
                    break;
                }
                //look backward for same address
                for(int i=1; (pos-i)>=0  ;i++){
                    t = list.at(pos-i);
                    if( _addressOffset(t->address) == (startAddress+index) ){
                        _writeValue(t, unit, index, type);
                        continue;
                    }
                    break;
                }
                break;
            }else if( _addressOffset(t->address) > (startAddress+index) ){
                if(lastSpan == 0) break;
                lastSpan /= 2;
                if(lastSpan == 0) pos -= 1;
                else pos -= lastSpan;
            }else if( _addressOffset(t->address) < (startAddress+index)){
                if(lastSpan == 0) break;
                lastSpan /= 2;
                if(lastSpan == 0) pos += 1;
                else pos += lastSpan;
            }
        }
    }
}

void ModbusSlaveProtocolDriver::_writeValue(TagAddress *t, ModbusDataUnit& unit, uint index, const int type) {
    if(!t->needToWrite) return;

    QVariant value;
    if(t->RWStrategy == iTagInfo::RWS_READ_WRITE)
        //because in R&W Strategy, read value is always scaled.
        //If scaled value is written, some data tremblling will occured.
        value = t->tagInfo->unscaledValue();
    else
        value = t->tagInfo->value();

    quint32 uintValue;
    float floatValue;
    quint32 *pValue;

    switch (type) {
    case Coils:
    case DiscreteInputs:
        unit.setBitValue(index, value.toBool());
        break;
    case InputRegisters:
    case HoldingRegisters:
        if (t->tagInfo->type() != iTagInfo::TYPE_INT32 &&
                t->tagInfo->type() != iTagInfo::TYPE_UINT32 &&
                t->tagInfo->type() != iTagInfo::TYPE_FLOAT) {
            unit.setRegValue(index, (quint16)(value.toUInt()));
        } else {
            if (t->tagInfo->type() == iTagInfo::TYPE_INT32 || t->tagInfo->type() == iTagInfo::TYPE_UINT32) {
                uintValue = value.toUInt();
                if (_parameters[Endianness] == BigEnd){
                    unit.setRegValue(index,   (quint16)((uintValue&0xFF00)>>16));
                    unit.setRegValue(index+1, (quint16)(uintValue&0xFF));
                } else {
                    unit.setRegValue(index,   (quint16)(uintValue&0xFF));
                    unit.setRegValue(index+1, (quint16)((uintValue&0xFF00)>>16));
                }
            } else if (t->tagInfo->type() == iTagInfo::TYPE_FLOAT) {
                floatValue = value.toFloat();
                pValue = (quint32*)&floatValue;
                if (_parameters[Endianness] == BigEnd){
                    unit.setRegValue(index,   (quint16)((*pValue&0xFF00)>>16));
                    unit.setRegValue(index+1, (quint16)(*pValue&0xFF));
                } else {
                    unit.setRegValue(index,   (quint16)(*pValue&0xFF));
                    unit.setRegValue(index+1, (quint16)((*pValue&0xFF00)>>16));
                }
            }
        }
        break;
    default:
        return;
    }
}

//switch (errnum) {
//case EMBXILFUN:
//    return "Illegal function";
//case EMBXILADD:
//    return "Illegal data address";
//case EMBXILVAL:
//    return "Illegal data value";
//case EMBXSFAIL:
//    return "Slave device or server failure";
//case EMBXACK:
//    return "Acknowledge";
//case EMBXSBUSY:
//    return "Slave device or server is busy";
//case EMBXNACK:
//    return "Negative acknowledge";
//case EMBXMEMPAR:
//    return "Memory parity error";
//case EMBXGPATH:
//    return "Gateway path unavailable";
//case EMBXGTAR:
//    return "Target device failed to respond";
//case EMBBADCRC:
//    return "Invalid CRC";
//case EMBBADDATA:
//    return "Invalid data";
//case EMBBADEXC:
//    return "Invalid exception code";
//case EMBMDATA:
//    return "Too many data";
//case EMBBADSLAVE:
//    return "Response not from requested slave";
//default:
//    return strerror(errnum);
//}

void ModbusSlaveProtocolDriver::_errorProcess(const QString& msg) {
    emit msgEventString(msg);
    qDebug()<<msg<<errno;

    //need restart
    if (errno == EMBXSFAIL || errno == 5 || errno == ECONNREFUSED) {
        if (_retryCount < _parameters[NumberOfRetries].toInt()) {
            int lastRetryCount = _retryCount + 1;
            restart();
            _retryCount = lastRetryCount;
        } else {
            qDebug()<<"to many retries";
            _setState(STAT_ERR);
        }
    }
    //need flush
    else if (errno == EMBBADCRC || errno == EMBXMEMPAR || errno == EMBMDATA) {
        int ret = modbus_flush(_pModbusDevice);
    }
}

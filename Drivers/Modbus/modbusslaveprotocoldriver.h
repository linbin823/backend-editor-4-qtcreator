#ifndef MODBUSMASTERPROTOCOLDRIVER_H
#define MODBUSMASTERPROTOCOLDRIVER_H
#include "iglobal.h"
#include "libmodbus/modbus.h"
#include "modbusdataunit.h"
#include <QObject>
#include <QMap>
#include <QTimer>

class QTimer;
class QVariant;
class QHostAddress;
class ModbusSlaveProtocolDriverUI;

#define NO_AVAILABLE_PORT_NAME tr("no available port")

class ModbusSlaveProtocolDriver : public iDriver
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "com.appServer.drivers.modbusSlave" FILE "modbusSlave.json")
    Q_INTERFACES(iDriver)

    friend class ModbusMasterParametersModel;
    friend class ModbusMasterParametersDelegate;
public:
    enum enumParameters{
        ConnectionType,
        SlaveStationID,
        SerialModel,
        SerialPortNameParameter,
        SerialParityParameter,
        SerialBaudRateParameter,
        SerialDataBitsParameter,
        SerialStopBitsParameter,
        NetworkPort,
        NetworkMaxConnections,
        ResponseTimeOutMS,
        NumberOfRetries,
        SerialInterFrameDelayMS,
        DataAddressing,
        Endianness,
        CoilStartAddress,
        CoilNumber,
        DiscreteInputsStartAddress,
        DiscreteInputsNumber,
        InputRegistersStartAddress,
        InputRegistersNumber,
        HoldingRegistersStartAddress,
        HoldingRegistersNumber
        /*,
        MaxReadInputPerMessage,
        MaxWriteCoilPerMessage,
        MaxReadRegistersPerMessage,
        MaxWriteRegistersPerMessage,
        MaxReadWriteRegistersPerMessage*/
    };
    static QString enumParametersString(int type){
        switch(type){
        case ConnectionType:
            return tr("通信类型");
        case SlaveStationID:
            return tr("从站ID");
        case SerialModel:
            return tr("串口模式");
        case SerialPortNameParameter:
            return tr("串口名称");
        case SerialParityParameter:
            return tr("串口校验");
        case SerialBaudRateParameter:
            return tr("串口波特率");
        case SerialDataBitsParameter:
            return tr("串口数据位");
        case SerialStopBitsParameter:
            return tr("串口停止位");
        case NetworkPort:
            return tr("服务器端口");
        case NetworkMaxConnections:
            return tr("网络最大连接数");
        case ResponseTimeOutMS:
            return tr("超时时间(ms)");
        case NumberOfRetries:
            return tr("重试次数");
        case SerialInterFrameDelayMS:
            return tr("串口数据帧延时(ms)");
        case DataAddressing:
            return tr("地址格式");
        case Endianness:
            return tr("双字(32bits)拼字模式");
        case CoilStartAddress:
            return tr("开关量起始地址");
        case CoilNumber:
            return tr("开关量数量");
        case DiscreteInputsStartAddress:
            return tr("只读开关量起始地址");
        case DiscreteInputsNumber:
            return tr("只读开关量数量");
        case InputRegistersStartAddress:
            return tr("只读寄存器起始地址");
        case InputRegistersNumber:
            return tr("只读寄存器数量");
        case HoldingRegistersStartAddress:
            return tr("保持型寄存器起始地址");
        case HoldingRegistersNumber:
            return tr("保持型寄存器数量");
//        case MaxReadInputPerMessage:
//            return tr("单个数据包最大读取bit数(<=2000)");
//        case MaxWriteCoilPerMessage:
//            return tr("单个数据包最大写入bit数(<=1968)");
//        case MaxReadRegistersPerMessage:
//            return tr("单个数据包最大读取寄存器数(<=125)");
//        case MaxWriteRegistersPerMessage:
//            return tr("单个数据包最大写入寄存器数(<=123)");
//        case MaxReadWriteRegistersPerMessage:
//            return tr("单个数据包最大读写寄存器数(<=121)");
        default:
            return QString();
        }
    }
    enum enumConnectionType {
        Serial,
        Tcp
    };
    static QString enumConnectionTypeString(int type){
        switch(type){
        case Serial:
            return tr("串口");
        case Tcp:
            return tr("以太网");
        default:
            return QString();
        }
    }
    enum enumSerialModel {
        Half485 = MODBUS_RTU_RS485,
        Full232 = MODBUS_RTU_RS232
    };
    static QString enumSerialModelString(int type){
        switch(type){
        case Half485:
            return tr("485半双工");
        case Full232:
            return tr("232全双工");
        default:
            return tr("unknown modle");
        }
    }
    enum enumSerialParity{
        NoParity = 'N',
        OddParity = 'O',
        EvenParity = 'E'
    };
    static QString enumSerialParityString(int type){
        switch(type){
        case NoParity:
            return tr("无校验");
        case OddParity:
            return tr("奇校验");
        case EvenParity:
            return tr("偶校验");
        default:
            return tr("Unknow Parity");
        }
    }
    enum enumSerialBaud{
        Baud1200 = 1200,
        Baud2400 = 2400,
        Baud4800 = 4800,
        Baud9600 = 9600,
        Baud19200 = 19200,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200
    };
    static QString enumSerialBaudString(int baud){
        return tr("%1.%2 kbps").arg(baud / 1000).arg(baud % 1000);
    }
    enum enumSerialDataBit{
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8
    };
    static QString enumSerialDataBitString(int type){
        switch(type){
        case Data5:
            return tr("5bits");
        case Data6:
            return tr("6bits");
        case Data7:
            return tr("7bits");
        case Data8:
            return tr("8bits");
        default:
            return tr("Unkonw Data Bit");
        }
    }
    enum enumSerialStopBit{
        OneStop = 1,
        TwoStop = 2
    };
    static QString enumSerialStopBitString(int type){
        switch(type){
        case OneStop:
            return tr("1bit");
        case TwoStop:
            return tr("2bits");
        default:
            return tr("Unkonw Stop Bit");
        }
    }
    enum enumDataType {
        Invalid = -1,
        Coils = 0,
        DiscreteInputs = 1,
        InputRegisters = 3,
        HoldingRegisters =4
    };
    static QString enumDataTypeString(int type){
        switch(type){
        case Invalid:
            return tr("Invalid Type");
        case Coils:
            return tr("开关量0x0");
        case DiscreteInputs:
            return tr("只读开关量0x1");
        case InputRegisters:
            return tr("只读寄存器0x3");
        case HoldingRegisters:
            return tr("保持型寄存器0x4");
        default:
            return QString();
        }
    }
    enum enumDataAddressing{
        ZeroBased,
        OneBased
    };
    static QString enumDataAddressingString(int type){
        switch(type){
        case ZeroBased:
            return tr("从0开始");
        case OneBased:
            return tr("从1开始");
        default:
            return QString();
        }
    }
    enum enumEndianness{
        BigEnd,
        SmallEnd
    };
    static QString enumEndiannessString(int type){
        switch(type){
        case BigEnd:
            return QObject::tr("大端");
        case SmallEnd:
            return QObject::tr("小端");
        default:
            return QString();
        }
    }

public:
    ModbusSlaveProtocolDriver(QObject *parent = 0);
    ~ModbusSlaveProtocolDriver();

public slots:
    DriverInfo driverInfo()const override;
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;
    int start() override;
    int stop() override;
    int restart() override;
    inline int state() override{return _state;}
    QList<int> availableRWStrategy(void)const override;
    QList<int> availableRWStrategy(const QString& addr)const override;
    bool isAddressCorrect(const QString& addr)const override;
    QString addressErrorString(const QString& addr)const override;
    int setRelatedTagAddresses(const QList<TagAddress *>& relatedTagAddresses) override;
    void showUI(bool show=true, QWidget* parent=0) override;

private:
    modbus_t* _pModbusDevice;
    modbus_mapping_t* _pDataMapping;
    //all parameters.
    QMap<int,QVariant> _parameters;

    int _state;
    int _retryCount;
    ModbusSlaveProtocolDriverUI * _pUI;

    QList<iTagInfo::enumRWStrategyCode> _supportedRWS;
    QList<ModbusSlaveProtocolDriver::enumDataType> _supportedType;
    /*!
     * \brief compareTagAddress
     * For sorting
     * instead of operator<() to compare the TagInfo*
     * \param t1
     * \param t2
     * \return
     */
    static bool compareTagAddress(const TagAddress* t1, const TagAddress* t2);

    QMap<ModbusSlaveProtocolDriver::enumDataType, QMap<iTagInfo::enumRWStrategyCode,QList<TagAddress*> > > _tagAddresses;
    QMap<ModbusSlaveProtocolDriver::enumDataType, QMap<iTagInfo::enumRWStrategyCode, QList<ModbusDataUnit> > > _dataUnits;

private slots:
    void _initDataUnites();
    void _initParameters();
    void _setState(enum_states state);

    void _serialProcess();
    void _tcpProcess();
    void _unitProcesss(ModbusDataUnit unit, int dataType, int RWStrategy);
    /*!
     * \brief _addressArea
     * get area part of a address
     * \param addr
     * \return
     */
    int _addressArea(const QString& addr)const;
    /*!
     * \brief _addressOffset
     * get offset part of a address
     * \param addr
     * \return
     */
    int _addressOffset(const QString& addr)const;
    /*!
     * \brief _prepareDataUnit
     * generate DataUnit from TagAddress
     * \param dataType
     * \param list
     * \return
     */
    QList<ModbusDataUnit> _prepareDataUnit(const QList<TagAddress *>& list, int dataType) const;
    /*!
     * \brief _readValues
     * read data from DataUnit unit to TagAddress t
     * \param unit
     * \param list
     * \param enumDataType
     */
    void _readValues(const QList<TagAddress *>& list, const ModbusDataUnit& unit, const int type);
    /*!
     * \brief _readValue
     * read value from ModbusDataUnit unit to TagAddress t
     * \param t
     * \param unit
     * \param index
     * \param enumDataType
     */
    void _readValue(TagAddress *t, const ModbusDataUnit& unit, uint index, const int type);
    /*!
     * \brief _writeValues
     * \param list
     * \param unit
     * \param type
     */
    void _writeValues(const QList<TagAddress *>& list, ModbusDataUnit &unit, const int type);
    /*!
     * \brief _writeValue
     * \param t
     * \param unit
     * \param index
     * \param type
     */
    void _writeValue(TagAddress *t, ModbusDataUnit& unit, uint index, const int type);
    /*!
     * \brief _isDWord(Double Word)
     * \param t
     * \return if t is 32bit, then return true
     */
    bool _isDWord(TagAddress *t)const;
    /*!
     * \brief _errorProcess
     */
    void _errorProcess(const QString& msg);
};
#endif // MODBUSMASTERPROTOCOLDRIVER_H

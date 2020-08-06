#ifndef ModbusDataUnit_H
#define ModbusDataUnit_H

#include <QObject>
#include <QVector>
#include <QString>

class ModbusDataUnit
{
public:
    ModbusDataUnit() = default;

    int startAddress() const { return _startAddress; }
    void setStartAddress(int newAddress) {
        if (newAddress >=0 ) {
            _startAddress = newAddress;
        }
    }

    QVector<quint16> values() const { return _values; }
    void setValues(const QVector<quint16> &newValues)
    {
        _values = newValues;
        _bitCount = newValues.size() * 16;
        _regCount = newValues.size();
    }

    int regCount() const { return _regCount; }

    int bitCount() const { return _bitCount; }

    void setRegCount(int newCount) {
        if (newCount >=0) {
            _regCount = newCount;
            _bitCount = newCount * 16;
            _values.resize(_regCount);
        }
    }

    void setBitCount(int newCount) {
        if (newCount >=0) {
            _regCount = (newCount / 16) + 1;
            _bitCount = newCount;
            _values.resize(_regCount);
        }
    }

    void setRegValue(int index, quint16 newValue) {
        if (index < 0 || _regCount <= index)
            return;
        _values[index] = newValue;
    }

    void setBitValue(int index, bool newValue) {
        if (index < 0 || (_regCount*16) <= index)
            return;
        int regNum = index / 16;
        int bitNum = index % 16;
        quint16 mask = 1<<bitNum;
        if(newValue) {
            _values[regNum] = _values[regNum] | mask;
        } else {
            _values[regNum] = _values[regNum] & (~mask);
        }
    }

    quint16 regValue(int index) const {
        if (index < 0 || _regCount <= index)
            return 0;
        return _values[index];
    }

    bool bitValue(int index) const {
        if (index < 0 || (_regCount*16) <= index)
            return false;
        int regNum = index / 16;
        int bitNum = index % 16;
        quint16 mask = 1<<bitNum;
        return (_values[regNum] & mask);
    }

    quint16* regData() {
        return _values.data();
    }

    quint8* bitData() {
        return (quint8*)_values.data();
    }
private:
    int _startAddress = 0;
    QVector<quint16> _values;
    int _bitCount = 0;
    int _regCount = 0;
};

#endif // ModbusDataUnit_H

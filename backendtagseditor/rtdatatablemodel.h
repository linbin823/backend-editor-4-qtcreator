#ifndef RTDATATABLEMODEL_H
#define RTDATATABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>

namespace BackendEditor {
namespace Internal {

class iDriver;
class TagInfo;

class RTDataTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RTDataTableModel(QObject* parent =0);

    enum enumPartNames{
        GENERAL = 0,
        INFO,
        DRIVERS,
        SCALE,
        ONLINESTATE,
    };

    enum enumItemNames{
        StartOfGeneral = 0,
        Name,
        Type,
//        PollGroup,

        StartOfInfo,
        Description,
        Unit,
        ExtraInfo,
        SubSystemName,
        ProjectID,
        ProjecttName,
        StationID,
        StationName,
        ModuleID,
        PointID,

        StartOfDrivers,
        Address,
        RWStrategy,

        StartOfScale,
        Ratio,
        Offset,

        StartOfOnlineState,
        Value,
        LastUpdateTimes,
    };
    /*!
     * \brief column
     * get column number from a certain combine of partName, itemName and driverName.
     * \param itemName
     * \param driverName
     * \return
     */
    int column(int itemName, const QString& driverName=QString())const;
    /*!
     * \brief setValue
     * set value to certain column's item
     * \param t
     * \param column
     * \param value
     * \return
     */
    bool setValue(TagInfo *t, int column, const QVariant& value);
    /*!
     * \brief value
     * fetch value from certain column's item
     * \param column
     * \return
     */
    QVariant value(TagInfo *t,int column)const;
    /*!
     * \brief columnName
     * get certain Name of column
     * \param column
     * \return
     */
    QVariant columnName(int column)const;
    /*!
     * \brief partLength
     * get part length
     * \param partName
     * \return
     */
    int partLength(int partName)const;

    void setPartVisible(int partName, bool visible);
    bool partVisible(int partName)const;
    /*!
     * \brief getNames
     * private/protected function
     * get combine of partName, itemName and driverName from column
     * \param column
     * \param partName
     * \param itemName
     * \param driverName
     */
    void getNames(int column, int* partName, int* itemName, QString* driverName)const;
public slots:
    void driverUpdated();
signals:
    void updateDelegates();
    //implement of QAbstractTableModel
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role= Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

public:
    QList<int> errorRows();
    QStringList driverNames;
private:
    QList<bool> _partsVisibleState;
};

} // namespace Internal
} // namespace BackendEditor

#endif // RTDATATABLEMODEL_H

#ifndef DRIVERMODEL_H
#define DRIVERMODEL_H

#include <QAbstractTableModel>

class DriverModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DriverModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public:
    //return error rows' index;
    QList<int> errorRows();
    int startDriver(int row);
    int stopDriver(int row);
    void startAllDriver(void);
    void stopAllDriver(void);
    void showConfigUI(QModelIndex index);
private:
};

#endif // DRIVERMODEL_H

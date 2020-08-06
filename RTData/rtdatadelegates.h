#ifndef ALARMDATADELEGATES_H
#define ALARMDATADELEGATES_H
#include <QItemDelegate>
#include <QComboBox>
#include <QCheckBox>
#include "taginfo.h"
#include "rtdata.h"
#include "rtdatatablemodel.h"

//remove PollGroup 2017.11
//#include "PollGroups/pollgroupmgr.h"

class TagInfoRWStrategyDelegate: public QItemDelegate{
    Q_OBJECT
public:
    TagInfoRWStrategyDelegate(RTDataTableModel* model, QObject* parent =0): QItemDelegate(parent), _pModel(model){}
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override{
        Q_UNUSED(option)
        QComboBox* editor = new QComboBox(parent);
        QString tagName = _pModel->data( index.sibling(index.row(),0), Qt::UserRole ).toString();
        TagInfo* t = static_cast<TagInfo*>( RTData::Instance()->tagInfo(tagName) );
        int partName, itemName;
        QString driverName;
        _pModel->getNames(index.column(), &partName, &itemName, &driverName);
        if(!t){
            editor->addItems( TagInfo::enumRWStrategyStrings() );
            return editor;
        }
        foreach (int rws, t->availableRWStrategy( driverName )) {
            editor->addItem( TagInfo::enumRWStrategyString(rws),rws);
        }
        editor->setCurrentText( TagInfo::enumRWStrategyString(t->RWStrategy( driverName )) );
        return editor;
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        QComboBox* combobox = static_cast<QComboBox*>(editor);
        if(!combobox) return;
        model->setData(index, TagInfo::enumRWStrategyValue( combobox->currentText() ), Qt::EditRole);
    }
private:
    RTDataTableModel* _pModel;
};

class TagInfoTypeDelegate: public QItemDelegate{
    Q_OBJECT
public:
    TagInfoTypeDelegate(RTDataTableModel* model, QObject* parent =0): QItemDelegate(parent), _pModel(model){}
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override{
        Q_UNUSED(option)
        QComboBox* editor = new QComboBox(parent);
        editor->addItems( TagInfo::enumTypeStrings() );
        editor->setCurrentText( index.data().toString() );
        return editor;
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        QComboBox* combobox = static_cast<QComboBox*>(editor);
        if(!combobox) return;
        model->setData(index, TagInfo::enumTypeValue( combobox->currentText() ), Qt::EditRole);
    }
private:
    RTDataTableModel* _pModel;
};

//remove PollGroup 2017.11
//class PollGroupNameDelegate: public QItemDelegate{
//    Q_OBJECT
//public:
//    PollGroupNameDelegate(QObject* parent =0): QItemDelegate(parent){}
//    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
//                          const QModelIndex &index) const override{
//        Q_UNUSED(option)
//        QComboBox* editor = new QComboBox(parent);
//        editor->addItems( PollGroupMgr::Instance()->names() );
//        editor->setCurrentText( index.data().toString() );
//        return editor;
//    }
//};
#endif // ALARMDATADELEGATES_H

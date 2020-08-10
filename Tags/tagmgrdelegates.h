#ifndef TAGMGRDELEGATES_H
#define TAGMGRDELEGATES_H
#include <QItemDelegate>
#include <QComboBox>
#include <QCheckBox>
#include "../Interface/Tags/tagmgr.h"
#include "../Interface/Tags/tag.h"
#include "tagmgrtablemodel.h"

//remove PollGroup 2017.11
//#include "PollGroups/pollgroupmgr.h"

class TagRWStrategyDelegate: public QItemDelegate{
    Q_OBJECT
public:
    TagRWStrategyDelegate(TagMgrTableModel* model, QObject* parent =0): QItemDelegate(parent), _pModel(model){}
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override{
        Q_UNUSED(option)
        QComboBox* editor = new QComboBox(parent);
        QString tagName = _pModel->data( index.sibling(index.row(),0), Qt::UserRole ).toString();
        Tag* t = static_cast<Tag*>( TagMgr::Instance()->tag(tagName) );
        int partName, itemName;
        QString driverName;
        _pModel->getNames(index.column(), &partName, &itemName, &driverName);
        if(!t){
            editor->addItems( Tag::enumRWStrategyStrings() );
            return editor;
        }
        foreach (Tag::enumRWStrategyCode rws, t->availableRWStrategy( driverName )) {
            editor->addItem( Tag::enumRWStrategyString(rws),rws);
        }
        editor->setCurrentText( Tag::enumRWStrategyString(t->RWStrategy( driverName )) );
        return editor;
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        QComboBox* combobox = static_cast<QComboBox*>(editor);
        if(!combobox) return;
        model->setData(index, Tag::enumRWStrategyValue( combobox->currentText() ), Qt::EditRole);
    }
private:
    TagMgrTableModel* _pModel;
};

class TagTypeDelegate: public QItemDelegate{
    Q_OBJECT
public:
    TagTypeDelegate(TagMgrTableModel* model, QObject* parent = 0): QItemDelegate(parent), _pModel(model){}
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override{
        Q_UNUSED(option)
        QComboBox* editor = new QComboBox(parent);
        editor->addItems( Tag::enumTypeStrings() );
        editor->setCurrentText( index.data().toString() );
        return editor;
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override{
        QComboBox* combobox = static_cast<QComboBox*>(editor);
        if(!combobox) return;
        model->setData(index, Tag::enumTypeValue( combobox->currentText() ), Qt::EditRole);
    }
private:
    TagMgrTableModel* _pModel;
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
#endif // TAGMGRDELEGATES_H

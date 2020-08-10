#ifndef POLLGROUPMGR_H
#define POLLGROUPMGR_H

#include <QObject>
#include <QAbstractItemModel>
#include "iloadsave.h"
#include "pollgroup.h"

class PollGroupUI;

class PollGroupMgr : public QObject, public iLoadSave
{
    Q_OBJECT

    Q_PROPERTY(QList<QObject*> pollGroupList READ pollGroupList NOTIFY pollGroupListChanged)
private:
    explicit PollGroupMgr(QObject *parent = nullptr);
    static PollGroupMgr* _singleton;

public:
    static PollGroupMgr* Instance();
    virtual ~PollGroupMgr();
public slots:
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;
    /*!
     * \brief contains
     * 测试 pollGroupName 是否存在
     * \param pollGroupName
     * \return true=does contains
     */
    bool contains(const QString& pollGroupName , PollGroup* ignore = nullptr)const;
    /*!
     * \brief showUI
     * 显示管理页面
     * \param show
     * \param parent
     */
    void showUI(bool show=true, QWidget* parent=0);
    /*!
     * \brief names
     * 获取所有的点名称
     * \return
     */
    virtual QStringList names(void)const;
    /*!
     * \brief pollGroup
     * QML使用，获取pollGroup
     * 输入参数：
     * 1、name
     * 返回数值：返回QObject
     * 功能描述：
     * 1、查找pollGroupName
     * 2、找不到返回nullptr
     * \param name
     * \return
     */
    QObject* pollGroup(const QString& name)const;
    /*!
     * \brief pollgroupList
     * get all pollgroups, for QML
     * \return
     */
    QList<QObject*> pollGroupList(void)const;
    /*!
     * \brief genPollGroupName
     * if testName is unique， return testName, else return a unique name
     * \param testName
     * \return
     */
    QString genPollGroupName(const QString& testName);
    bool insertPollGroup(int pos, const QString& groupName=QString(), int pollInterv=1000);
    bool removePollGroup(const QString& groupName);
    bool removePollGroup(int pos);
signals:
    void pollGroupListChanged();

private:
    QAbstractTableModel* _pModel;
    PollGroupUI * _pUI;
    /*!
     * \brief _pollGroup
     * 所有测点集合
     */
    QList<PollGroup*> _pollGroups;
};

#endif // POLLGROUPMGR_H

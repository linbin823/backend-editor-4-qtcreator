#ifndef TAGMGR_H
#define TAGMGR_H

#include <QObject>
#include "iloadsave.h"

class Tag;
class TagAddress;
class TagMgrUI;
class TagMgrTableModel;

/*!
 * \brief The TagMgr class
 * This is a manager parent of all tag instances.
 * This class holds a list of tag*, with basic CRUD functions
 * TagMgr is the same during both config and runtime.
 */
class TagMgr : public QObject, public iLoadSave
{
    Q_OBJECT

    Q_PROPERTY(QList<Tag* > tagList READ tagList NOTIFY tagListChanged)

public slots:

    /*!
     * \brief tagList
     * get all tags, for drivers and data proxy
     * \return
     */
    QList<Tag* > tagList(void) const;

    /*!
     * \brief allAddresses
     * for driverManager.
     * can get all TagAddress.
     * \return
     */
    QList<TagAddress*> allAddresses(void) const;

    /*!
     * \brief tagNames
     * 获取所有的点名称
     * \return
     */
    QStringList tagNames(void) const;

    /*!
     * \brief tag
     * 获取tag
     * 输入参数：
     * 1、name
     * 返回数值：返回Tag*
     * 功能描述：
     * 1、查找tag
     * 2、找不到返回nullptr
     * \param name
     * \return
     */
    Tag* tag(const QString& name) const;

    /*!
     * \brief contains
     * 测试tagName 是否存在
     * \param tagName
     * \return true=does contains
     */
    bool contains(const QString& name , Tag* ignore = nullptr ) const;

    /*!
     * \brief showUI
     * 显示管理页面
     * \param show
     * \param parent
     * \result QWidget* return the UI.
     */
    QWidget* showUI(bool show=true, QWidget *parent = 0);

    //implement of iLoadSave
    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;

    bool insertTag(const QString& tagName=QString(), int pos = -1);
    bool removeTag(const QString& tagName);
    bool removeTag(int pos);

    //remove PollGroup 2017.11
//    void attachPollGroup(const QString& pollGroupName, iTagInfo* t=0)const override;


public:
    /*!
     * \brief genTagName
     * if testName is unique， return testName, else return a unique name
     * \param testName
     * \return
     */
    QString genTagName(const QString& testName);
    /*!
     * 单例模式
     * 功能描述：
     * 1、获取单例的指针
     */
    static TagMgr*  Instance();
    virtual ~TagMgr();
    /*!
     * 测点初始化函数
     * 功能描述：
     * 1、删除所有测点List
     */
    void dataInit();

    /*!
     * \title 读取CSIC711海标格式的的configDB
     * 输入参数：
     * 1、ConfigDB文件名
     * \brief
     * 1、使用SQLIT数据库，打开ConfigDB
     * 2、文件不存在、读取错误，弹出MSGBOX
     * 3、从数据库中读取测点信息
     * 4、判断tagID唯一性、判断address正确性
     * 5、读取多少条、失败多少条、成功多少条，弹出MSGBOX
     */
//    void load_CSIC711_ConfigDB(const QString& fileName = "./ConfigDB.db");

    /*!
     * \brief attachPollGroup
     * connect signals of PollGroup with slots of iTag
     * should call it after any changes made to RollGroups or iTags
     */
//    virtual void attachPollGroup(const QString& pollGroupName, iTag* t=0)const = 0;

signals:
    void beginResetModel(void);
    void endResetModel(void);
    void tagListChanged(void);

private:
    /*!
     * 构造函数
     * 输入参数：无
     * 返回数值：无
     * 功能描述：
     * 1、测点初始化
     */
    explicit TagMgr(QObject *parent = 0);

    /*!
     * \brief _compareTag
     * \param t1
     * \param t2
     * for sorting
     * instead of operator<() to compare the TagInfo*
     * \return
     */
    static bool _compareTag(const Tag* t1, const Tag* t2);

    static TagMgr* _singleton;
    TagMgrTableModel* _pModel;
    TagMgrUI * _pUI;
    /*!
     * \brief _tags
     * 所有测点集合
     */
    QList<Tag*> _tagList;
};

#endif // TAGMGR_H

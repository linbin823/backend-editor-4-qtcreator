#ifndef RTDATA_H
#define RTDATA_H

#include <QObject>
#include "Interface/iglobal.h"
#include "rtdatatablemodel.h"

namespace BackendEditor {
namespace Internal {

class TagInfo;
class RTDataUI;

/*!
 * \brief The RTData class
 * This is a manager parent of all tagInfo instances.
 * This class holds a list of tagInfo*, with basic CRUD functions
 */

class RTData : public iRTData
{
    Q_OBJECT
    friend class RTDataTableModel;

public slots:
    QList<QObject*> tagList(void)const override;
    QList<iTagInfo*> allTags(void)const override;
    QList<TagAddress*> allAddresses(void)const override;
    QStringList names(void)const override;
    QObject* tagInfo(const QString& name)const override;
    bool contains(const QString& name , iTagInfo* ignore = nullptr )const override;
    void showUI(bool show=true, QWidget *parent = 0) override;

    void save(iLoadSaveProcessor* processor) override;
    void load(iLoadSaveProcessor* processor) override;

    bool insertTagInfo(int pos, const QString& tagName=QString()) override;
    bool removeTagInfo(const QString& tagName) override;
    bool removeTagInfo(int pos) override;

    //remove PollGroup 2017.11
    //void attachPollGroup(const QString& pollGroupName, iTagInfo* t=0)const override;


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
    static RTData*  Instance();
    virtual ~RTData();
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
    void load_CSIC711_ConfigDB(const QString& fileName = "./ConfigDB.db");
signals:
    void beginResetModel(void);
    void endResetModel(void);
private:
    /*!
     * 构造函数
     * 输入参数：无
     * 返回数值：无
     * 功能描述：
     * 1、测点初始化
     */
    explicit RTData(QObject *parent = 0);
    /*!
     * \brief _compareTagInfo
     * \param t1
     * \param t2
     * for sorting
     * instead of operator<() to compare the TagInfo*
     * \return
     */
    static bool _compareTagInfo(const TagInfo* t1, const TagInfo* t2);

    static RTData* _singleton;
    RTDataTableModel* _pModel;
    RTDataUI * _pUI;
    /*!
     * \brief _tags
     * 所有测点集合
     */
    QList<TagInfo*> _tags;
};

} // namespace Internal
} // namespace BackendEditor

#endif // RTDATA_H

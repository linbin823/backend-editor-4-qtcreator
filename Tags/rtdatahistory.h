#ifndef RTDATAHISTORY_H
#define RTDATAHISTORY_H

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QTimer>
#include "rtdriverwebservice.h"
#include "taghistory.h"

class RTDataHistory : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool         ready           READ ready              WRITE setReady              NOTIFY readyChanged)
    //actualStartDate和actualStopDate：
    //实际有值的数据的起止日期时间，由驱动的返回更新，密度变化时需要归到零位
    Q_PROPERTY(QDateTime    actualStartDate READ actualStartDate    WRITE setActualStartDate    NOTIFY actualStartDateChanged)
    Q_PROPERTY(QDateTime    actualStopDate  READ actualStopDate     WRITE setActualStopDate     NOTIFY actualStopDateChanged)
    //reqStartDate和reqStopDate：
    //界面上请求的数据的起止日期时间，由界面请求更新，密度变化时不变
    Q_PROPERTY(QDateTime    reqStartDate    READ reqStartDate       WRITE setReqStartDate       NOTIFY reqStartDateChanged)
    Q_PROPERTY(QDateTime    reqStopDate     READ reqStopDate        WRITE setReqStopDate        NOTIFY reqStopDateChanged)

public:
    //singleton
    static RTDataHistory* Instance();

    enum EnumPointDensity{
        PD_PointPer100MS,
        PD_PointPerSec,
        PD_PointPer10Sec,
        PD_PointPerMin,
        PD_PointPer10Min,
        PD_PointPerHr
    };

    QDateTime actualStartDate()const;
    QDateTime actualStopDate()const;
    QDateTime reqStartDate()const;
    QDateTime reqStopDate()const;
    bool ready()const;
    void setReady(bool t);
    int density(void)const;
    void setDensity(int d);

    //把密度和启止时间转换为数量
    int  densityToCount(const QDateTime &startTime,const QDateTime &stopTime );

public slots:

    //QML获取单个历史测点数据
    QObject* getHistoryTag(int tagID);
    //清空历史数据
    void clearAllHistoryTags();
    void clearHistoryTag(int targetTagID);

    void setActualStartDate(const QDateTime& date);
    void setActualStopDate(const QDateTime& date);
    void setReqStartDate(const QDateTime& date);
    void setReqStopDate(const QDateTime& date);

private:
    explicit RTDataHistory(QObject *parent = 0);
    static RTDataHistory* _singleton;
    RTDriverWebService* _pDriver;

    bool _ready;
    int  _density;
    QDateTime _actualStartDate;
    QDateTime _actualStopDate;
    QDateTime _reqStartDate;
    QDateTime _reqStopDate;
    QList<TagHistory*> _historyData;

private slots:
    void updateDateSpan();

signals:
    void readyChanged();
    void actualStartDateChanged();
    void actualStopDateChanged();
    void reqStartDateChanged();
    void reqStopDateChanged();

    void updateDateSpanChanged(const QDateTime& start, const QDateTime& stop, const int number);
};

#endif // RTDATAHISTORY_H

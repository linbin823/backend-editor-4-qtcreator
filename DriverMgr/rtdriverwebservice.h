#ifndef RTDRIVERWEBSERVICE_H
#define RTDRIVERWEBSERVICE_H

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QTimer>
#include "taginfo.h"
#include "taghistory.h"
#include "gSOAPGEN/soaprdLiteProxy.h"

/* real time web service connection
 * for big data APP of csic711
 * created in 2017.02.14 by linbin
 * modified in 2017.03.16 by linbin:
 * 1. add method to refresh current view's tags' value once
 * 2. add method to continueous refresh particular tag's value
 * 3. add method to get historical data
 * 4. modify the mechanism:
 *      "driver class" refreshes active/particular set of tags.
 *      "data class" owns the data, fulfill generate/store/filter data
 */

class RTDriverWebService : public QObject
{
    Q_OBJECT

public:
    explicit RTDriverWebService(QObject *parent = 0);
    ~RTDriverWebService();

    enum __valueType :unsigned char{
        VT_CURRENTVALUE, //实时数据
        VT_CURRENTALARM, //实时报警
        VT_HISTORYVALUE, //历史数据
        VT_HISTORYREALVALUE, //未拟合(真实)历史数据
        VT_ALARM,
        VT_EVENT,
        VT_EXTENSION
    };

    enum __states{
        STAT_RUN,
        STAT_STOP,
        STAT_ERR
    };

    QDateTime lastConnectTime;

signals:
    void stateChanged(int state);
    void actualStartDateChanged(const QDateTime& start);
    void actualStopDateChanged(const QDateTime& stop);

public:
    void setAllTags(QList<TagInfo *> *pAll);
    void setAllHistoryTags(QList<TagHistory*>* pAll);

public slots:
    void start();
    void stop();
    void restart();
    int getState(){
        return _state;
    }

    void tagRWSChanged(TagInfo*one);
    void setUpdateDateSpan(const QDateTime& start, const QDateTime& stop, int number);

private:
    QTimer *_pRefreshTimer, * _pRefreshHistoryTimer , *_pWatchDogTimer;
    QDateTime _historyDateStart, _historyDateStop;
    int _historyPointsNumber;
    int _state;
    rdLiteProxy* _pService;
    QList<int> _readTagIDs;
    QList<TagInfo*> _readTags;
    QList<TagInfo*>* _pAllTags;
    QList<TagHistory*>* _pAllHistoryTags;
    //webservice建立
    void _connectEstab();
    void _setState(int state);


private slots:
    //webservice断开
    void _connectBroken();
    int _refreshTags();
    int _refreshHistoryTags();
};

#endif // RTDRIVERWEBSERVICE_H

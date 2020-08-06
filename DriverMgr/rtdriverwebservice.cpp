#include <QString>
#include <QThread>
#include <QDebug>
#include "rtdriverwebservice.h"
#include "gSOAPGEN/stdsoap2.h"
#include "gSOAPGEN/rdLite.nsmap"

RTDriverWebService::RTDriverWebService(QObject *parent) : QObject(parent)
{
    _pRefreshTimer = new QTimer(this);
    _pRefreshTimer->setInterval(2*1000);
    _pRefreshTimer->setSingleShot(true);
    connect(_pRefreshTimer,SIGNAL(timeout()),this,SLOT(_refreshTags()));

    _pRefreshHistoryTimer = new QTimer(this);
    _pRefreshHistoryTimer->setInterval(100);
    _pRefreshHistoryTimer->setSingleShot(true);
    connect(_pRefreshHistoryTimer,SIGNAL(timeout()),this,SLOT(_refreshHistoryTags()));

    _pWatchDogTimer = new QTimer(this);
    _pWatchDogTimer->setInterval(50*1000);
    _pWatchDogTimer->setSingleShot(true);
    connect(_pWatchDogTimer,SIGNAL(timeout()),this,SLOT(_connectBroken()));

    _setState(STAT_STOP);

    _historyDateStart = QDateTime::currentDateTime();
    _historyDateStop = QDateTime::currentDateTime();
    _historyPointsNumber = 0;

    _pAllTags = nullptr;
    _pAllHistoryTags = nullptr;
}

RTDriverWebService::~RTDriverWebService(){

}

void RTDriverWebService::start(){
    const char* addr = "www.csic711.net:1662";
    _pService = new rdLiteProxy(addr, SOAP_IO_KEEPALIVE);
    _pService->send_timeout = 20; //sec
    _pService->connect_timeout = 20; //sec
    _pService->recv_timeout = 20; //sec
    _pRefreshTimer->start();
    _pRefreshHistoryTimer->start();
    _pWatchDogTimer->start();
    _connectBroken();
}

void RTDriverWebService::stop(){
    _pRefreshTimer->stop();
    _pRefreshHistoryTimer->stop();
    _pWatchDogTimer->stop();
    _connectBroken();
}

void RTDriverWebService::restart(){
    stop();
    start();
}

void RTDriverWebService::_setState(int state){
    if(state > 0 && state <= STAT_ERR){
        _state = state;
        emit stateChanged(_state);
    }
}

void RTDriverWebService::_connectEstab(){
    _pWatchDogTimer->start();
    lastConnectTime = QDateTime::currentDateTime();

    if(_state == STAT_STOP){
        _setState( STAT_RUN );
    }
}

void RTDriverWebService::_connectBroken(){
    if(_state == STAT_RUN){
        _setState( STAT_ERR );
    }
}

void RTDriverWebService::tagRWSChanged(TagInfo* one){
    switch(one->tagRWStrategy()){
        case TagInfo::RWS_DISABLE:
            if(_readTags.contains(one)){
                _readTags.removeAll(one);
                _readTagIDs.removeAll(one->tagID());
            }
            break;
        case TagInfo::RWS_READ:
        case TagInfo::RWS_READ_ONCE:
            if(!_readTags.contains(one)){
                _readTags << one;
                _readTagIDs << one->tagID();
            }
            break;
        default:
            break;
    }
}

void RTDriverWebService::setUpdateDateSpan(const QDateTime& start, const QDateTime& stop, int number){
    if( start <= stop ){
        _historyDateStart = start;
        _historyDateStop = stop;
        _historyPointsNumber = number;
    }
}

void RTDriverWebService::setAllTags(QList<TagInfo *> *pAll){
    _pAllTags = pAll;
    _readTagIDs.clear();
    _readTags.clear();
    foreach(TagInfo* it, *_pAllTags){
        tagRWSChanged(it);//prepare _readTags & _readTagIDs
    }
}

void RTDriverWebService::setAllHistoryTags(QList<TagHistory *> *pAll){
    _pAllHistoryTags = pAll;
}

int RTDriverWebService::_refreshTags(){
    if(_pAllTags == nullptr ) return -2;
    if(_pAllTags->isEmpty()) return -2;
    _ns1__Query req;
    req.beginTime = 0;
    req.count = 0;
    req.endTime = 0;
    req.stationId = 1;
//prepare tags
    ns1__TagIdArray* pTagIDs = new ns1__TagIdArray();
    pTagIDs->item = _readTagIDs.toVector().toStdVector();
    req.tagId = pTagIDs;
//start soap require
    _ns1__QueryResponse res;
    //function prototype:
    //int Query(_ns1__Query *ns1__Query, _ns1__QueryResponse *ns1__QueryResponse)
    int ret = -1;
    ret = _pService->Query(&req, &res);
    qDebug()<<Q_FUNC_INFO<<pTagIDs->item.size()<<ret;
    delete pTagIDs;
    if(ret == SOAP_OK /*&& res.result->error == 0*/){
//success!
        qDebug()<<Q_FUNC_INFO<<"success!"<<res.result->error<<res.result->item.size();
        foreach (ns1__TagValue* value, res.result->item) {
            foreach(TagInfo* tag, *_pAllTags){
                if(tag->tagID() == value->id ){
                    QString tagValue = QString::fromStdString(*value->value);
                    tag->updateTagValue(tagValue, QDateTime::fromMSecsSinceEpoch(value->time));
                    if(tag->tagRWStrategy()==TagInfo::RWS_READ_ONCE){
                        //read once finished!
                        tag->setTagRWStrategy(TagInfo::RWS_DISABLE);
                    }
                    //to tolerant duplicated tagID, annotate following line.
                    //break;
                }
            }
        }
        _connectEstab();
        _pRefreshTimer->start();
        return 0;
    }
//fail
    _pRefreshTimer->start();
    return -1;
}

int RTDriverWebService::_refreshHistoryTags(){
    if( _pAllHistoryTags==nullptr ) return -2;
//empty or date span zero or number zero
    if( _pAllHistoryTags->isEmpty() ||  _historyDateStart>=_historyDateStop || _historyPointsNumber==0){
        _pRefreshHistoryTimer->start();
        return 0;
    }
//prepare webservice request
    _ns1__Query req;
    ns1__TagIdArray* pReqTagID = req.tagId;
    foreach (TagHistory* tag, *_pAllHistoryTags) {
        pReqTagID->item.push_back( tag->tagID() );
    }
    req.beginTime = _historyDateStart.toMSecsSinceEpoch();
    req.count= _historyPointsNumber;
    req.endTime = _historyDateStop.toMSecsSinceEpoch();
    req.stationId = 1;
    req.valueType = VT_HISTORYVALUE;
//start soap require
    _ns1__QueryResponse res;
    //function prototype:
    //int Query(_ns1__Query *ns1__Query, _ns1__QueryResponse *ns1__QueryResponse)
    int ret = -1;
    ret = _pService->Query(&req, &res);
    if(ret == SOAP_OK || res.result->error == 0){
//success! sorting by id, wrap into several QObjectLists
        foreach (ns1__TagValue * it, res.result->item) {
            foreach( TagHistory* tag, *_pAllHistoryTags){
                if(tag->tagID() == it->id){
                    PointHistory* newPoint = new PointHistory();
                    newPoint->setValue( QString::fromStdString(*(it->value)).toFloat() );
                    newPoint->setTime( QDateTime::fromMSecsSinceEpoch( it->time ) );
                    tag->points.push_back( newPoint );
                    break;
                }
            }
        }
        _connectEstab();
        _pRefreshHistoryTimer->start();
        return 0;
    }
    _pRefreshHistoryTimer->start();
    return -1;
}

#include <QThread>
#include <QQmlEngine>
#include "rtdatahistory.h"

RTDataHistory* RTDataHistory::_singleton = nullptr;

RTDataHistory* RTDataHistory::Instance(){
    if(_singleton == nullptr){
        _singleton = new RTDataHistory();
    }
    return _singleton;
}

RTDataHistory::RTDataHistory(QObject *parent) : QObject(parent){

    _ready            = false;
    _density          = PD_PointPerMin;
    _actualStartDate  = QDateTime::currentDateTime();
    _actualStopDate   = QDateTime::currentDateTime();
    _reqStartDate     = QDateTime::currentDateTime();
    _reqStopDate      = QDateTime::currentDateTime();

    _pDriver = new RTDriverWebService();
    connect( _pDriver, SIGNAL(actualStartDateChanged(QDateTime)),this,SLOT(setActualStartDate(QDateTime)));
    connect( _pDriver, SIGNAL(actualStopDateChanged(QDateTime)), this,SLOT(setActualStopDate(QDateTime)));
    connect( this,SIGNAL(updateDateSpanChanged(QDateTime,QDateTime,int)),_pDriver, SLOT(setUpdateDateSpan(QDateTime,QDateTime,int)));

    QThread* workThread = new QThread();
    _pDriver->moveToThread(workThread);
    connect(workThread,SIGNAL(started()),_pDriver,SLOT(start()));
    workThread->start();
}

bool RTDataHistory::ready()const{
    return _ready;
}

void RTDataHistory::setReady(bool t){
    if(_ready != t){
        _ready = t;
        emit readyChanged();
    }
}

int RTDataHistory::density(void)const{
    return _density;
}

void RTDataHistory::setDensity(int d){
    if(_density != d){
        foreach(TagHistory* it , _historyData){
            it->clearPoints();
        }
        _density = d;
        setActualStartDate( QDateTime::currentDateTime() );
        setActualStopDate ( QDateTime::currentDateTime() );
    }
}

QDateTime RTDataHistory::actualStartDate()const{
    return _actualStartDate;
}

void RTDataHistory::setActualStartDate(const QDateTime& date){
    if(_actualStartDate != date && date <= _actualStopDate ){
        _actualStartDate = date;
        updateDateSpan();
        emit actualStartDateChanged();
    }
}

QDateTime RTDataHistory::actualStopDate()const{
    return _actualStopDate;
}

void RTDataHistory::setActualStopDate(const QDateTime& date){
    if(_actualStopDate != date && date <= QDateTime::currentDateTime() ){
        _actualStopDate = date;
        updateDateSpan();
        emit actualStopDateChanged();
    }
}

QDateTime RTDataHistory::reqStartDate()const{
    return _reqStartDate;
}

void RTDataHistory::setReqStartDate(const QDateTime& date){
    if(_reqStartDate != date && date <= _reqStopDate ){
        if(date < _reqStartDate){
            _reqStartDate = date;
            updateDateSpan();
            emit reqStartDateChanged();
        }
    }
}

QDateTime RTDataHistory::reqStopDate()const{
    return _reqStopDate;
}

void RTDataHistory::setReqStopDate(const QDateTime& date){
    if(_reqStopDate != date && date <= QDateTime::currentDateTime() ){
        if(date > _reqStopDate){
            _reqStopDate = date;
            updateDateSpan();
            emit reqStopDateChanged();
        }
    }
}

int  RTDataHistory::densityToCount(const QDateTime &startTime, const QDateTime &stopTime ){
    int span = stopTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch();
    if(span <=0 ) return 0;
    switch (_density){
    case PD_PointPer100MS:
        return span / 100;
    case PD_PointPerSec:
        return span / 1000;
    case PD_PointPer10Sec:
        return span / 1000 / 10;
    case PD_PointPerMin:
        return span / 1000 / 60;
    case PD_PointPer10Min:
        return span / 1000 / 60 / 10;
    case PD_PointPerHr:
        return span / 1000 / 60 / 60;
    default:
        return span / 1000 / 60;
    }
}


QObject* RTDataHistory::getHistoryTag(int tagID){
    for(int i=0; i<_historyData.size();i++){
        TagHistory* currentItem = _historyData.at(i);
        if(currentItem->tagID() == tagID){
            QQmlEngine::setObjectOwnership(_historyData.at(i),QQmlEngine::CppOwnership);
            return _historyData.at(i);
        }
    }
    TagHistory* newOne = new TagHistory(tagID);
    _historyData<<newOne;
    QQmlEngine::setObjectOwnership(newOne,QQmlEngine::CppOwnership);
    return newOne;
}


void RTDataHistory::clearHistoryTag(int targetTagID){
    foreach(TagHistory* it, _historyData){
        if(it->tagID() == targetTagID){
            it->deleteLater();
            _historyData.removeOne(it);
        }
    }
}

void RTDataHistory::clearAllHistoryTags(){
    while(_historyData.size() != 0){
        _historyData.last()->deleteLater();
        _historyData.removeLast();
    }
}

void RTDataHistory::updateDateSpan(){
    if( _reqStartDate < _actualStartDate ){
        emit updateDateSpanChanged(_reqStartDate, _actualStartDate,densityToCount(_reqStartDate, _actualStartDate) );
        setReady(false);
    }else if( _reqStopDate > _actualStopDate){
        emit updateDateSpanChanged( _actualStopDate, _reqStopDate ,densityToCount(_actualStopDate, _reqStopDate) );
        setReady(false);
    }else{
        setReady(true);
    }
}

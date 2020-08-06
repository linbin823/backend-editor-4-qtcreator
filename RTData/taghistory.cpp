#include "taghistory.h"

PointHistory::PointHistory(QObject *parent): QObject(parent){
    _time = QDateTime::currentDateTime();
    _value = 0.0;
}

PointHistory::~PointHistory(){

}

QDateTime PointHistory::time()const{
    return _time;
}

void PointHistory::setTime(const QDateTime& t){
    _time = t;
    emit timeChanged();
}

float PointHistory::value()const{
    return _value;
}

void PointHistory::setValue(float t){
    _value = t;
    emit valueChanged();
}


TagHistory::TagHistory(int id, QObject *parent): QObject(parent){
    setTagID(id);
    clearPoints();
}

TagHistory::~TagHistory(){
    clearPoints();
}

void TagHistory::clearPoints(){
    while(points.size() != 0){
        points.last()->deleteLater();
        points.removeLast();
    }
}

int TagHistory::tagID()const{
    return _tagID;
}

void TagHistory::setTagID(int t){
    if(_tagID != t){
        _tagID = t;
        emit tagIDChanged();
    }
}

QList<float> TagHistory::findMaxAndMinData(const QDateTime &start, const QDateTime &stop)const{
    QList<float> ret;
    int maxPos = -1;
    int minPos = -1;
    for(int i=0; i<points.length(); i++){

        if(points.at(i)->time() <= stop && points.at(i)->time() >= start){
            if(maxPos == -1 || minPos == -1){
                maxPos = i;
                minPos = i;
                continue;
            }
            if(points.at(maxPos)->value() < points.at(i)->value()){
                maxPos = i;
            }
            if(points.at(minPos)->value() > points.at(i)->value()){
                minPos = i;
            }
        }
    }
    if(maxPos != -1){
        ret << points.at(maxPos)->value();
    }else{ ret << 10.0; }

    if(minPos != -1){
        ret << points.at(minPos)->value();
    }else{ ret << 0.0; }

    return ret;
}

QList<QObject*> TagHistory::selectData(const QDateTime &start, const QDateTime &stop)const{
    QList<QObject*> ret;
    foreach (PointHistory* it, points) {
        if(it->time() <= stop && it->time() >= start)
            ret << it;
    }
    return ret;
}

QList<QObject*> TagHistory::allData()const{
    QList<QObject*> ret;
    foreach (PointHistory* it, points) {
        ret << it;
    }
    return ret;
}

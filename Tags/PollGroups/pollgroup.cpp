#include "pollgroup.h"

PollGroup::PollGroup(QObject *parent) : QObject(parent)
{

}

PollGroup::~PollGroup(){

}

void PollGroup::start(){
    if(intervMs >0){
        pollTimer.setSingleShot(false);
        connect(&pollTimer,&QTimer::timeout, this, &PollGroup::pollUpdate);
        pollTimer.start(intervMs);
    }
}

void PollGroup::save(iLoadSaveProcessor *processor){
    processor->writeValue("name",name);
    processor->writeValue("intervMs",intervMs);
}

void PollGroup::load(iLoadSaveProcessor *processor){
    processor->readValue("name",name);
    processor->readValue("intervMs",intervMs);
    start();
}

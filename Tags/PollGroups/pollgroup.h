#ifndef POLLGROUP_H
#define POLLGROUP_H

#include <QObject>
#include <QTimer>
#include "iloadsave.h"

class PollGroup : public QObject, public iLoadSave
{
    Q_OBJECT
public:
    explicit PollGroup(QObject* parent=0);
    virtual ~PollGroup();

    QString name;
    int    intervMs;
    QTimer pollTimer;
    void start();
    void save(iLoadSaveProcessor *processor) override;
    void load(iLoadSaveProcessor *processor) override;
signals:
    void pollUpdate();
};


#endif // POLLGROUP_H

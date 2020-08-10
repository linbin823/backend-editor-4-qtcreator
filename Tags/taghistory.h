#ifndef TAGHISTORY_H
#define TAGHISTORY_H

#include <QObject>
#include <QDateTime>

class PointHistory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime time READ time NOTIFY timeChanged)
    Q_PROPERTY(float value READ value NOTIFY valueChanged)
public:
    explicit PointHistory(QObject *parent = 0);
    ~PointHistory();

    QDateTime time()const;
    void      setTime(const QDateTime& t);
    float     value()const;
    void      setValue(float t);

private:
    QDateTime _time;
    float _value;
signals:
    void timeChanged();
    void valueChanged();
};

class TagHistory : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int tagID READ tagID WRITE setTagID NOTIFY tagIDChanged)

public:
    explicit TagHistory(int id = 0, QObject *parent = 0);
    ~TagHistory();

    void clearPoints();

    int tagID()const;
    void setTagID(int t);

    QList<PointHistory*> points;

public slots:
    QList<float> findMaxAndMinData(const QDateTime &start, const QDateTime &stop)const;
    QList<QObject*> selectData(const QDateTime &start, const QDateTime &stop)const;
    QList<QObject*> allData()const;

private:
    int  _tagID;

signals:
    void tagIDChanged();
};


#endif // TAGHISTORY_H

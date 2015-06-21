#ifndef TCOUNT_H
#define TCOUNT_H

#include <QThread>

class TCount : public QThread
{
    Q_OBJECT
public:
    explicit TCount(QObject *parent = 0);
    TCount(time_t);
    void run();

signals:
    
public slots:

private:
    time_t StartTime;

    
};

#endif // TCOUNT_H

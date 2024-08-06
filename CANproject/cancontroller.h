#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H
#include <QObject>
#include <QTQml>

class CanController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    int devHandle[10];
    explicit CanController(QObject *parent = nullptr);
    Q_INVOKABLE int initPort();
    static CanController *getInstance();
    Q_INVOKABLE void getMsg();
    int getFramesSent() const;

private:
    int getFirmWareInfo(int* devHandle);
    int canInit(int* devHandle);
    int initGet(int* devHandle);
    void decode(QString str);
    Q_PROPERTY(int framesSent READ getFramesSent NOTIFY framesSentChanged FINAL);

    int m_framesSent;
    QString hexConvert(char input);


signals:
    void pressed();
    void forwardOne();
    void forwardTwo();
    void backWardOne();
    void backWardTwo();
    void normal();
    void framesSentChanged();
};

#endif // CANCONTROLLER_H

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
    Q_INVOKABLE void udsRead();
    void udsInit();
    int getGears() const;

    int getOrig() const;

    int getPhysical() const;

private:
    int orig;
    int physical;
    int lastEmitted;
    int gears; //gear 0 = P, gear 1 = D, gears 2 = N, gears 3 = R
    int getFirmWareInfo(int* devHandle);
    int canInit(int* devHandle);
    int initGet(int* devHandle);
    void decode(QString str);
    Q_PROPERTY(int framesSent READ getFramesSent NOTIFY framesSentChanged FINAL);

    int m_framesSent;
    QString hexConvert(char input);


    Q_PROPERTY(int gears READ getGears NOTIFY gearsChanged FINAL)

    Q_PROPERTY(int orig READ getOrig NOTIFY origChanged FINAL)

    Q_PROPERTY(int physical READ getPhysical NOTIFY physicalChanged FINAL)

signals:
    void pressed();
    void forwardOne();
    void forwardTwo();
    void backWardOne();
    void backWardTwo();
    void normal();
    void framesSentChanged();
    void gearsChanged();
    void origChanged();
    void physicalChanged();
};

#endif // CANCONTROLLER_H

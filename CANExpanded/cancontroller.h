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

    int getTurnLamp() const;

    int getFwiper() const;

    int getFwiperSens() const;

    int getShifterPos() const;

private:
    int turnLamp;
    int fwiper;
    int fwiperSens;
    int shifterPos;
    int getFirmWareInfo(int* devHandle);
    int canInit(int* devHandle);
    int initGet(int* devHandle);
    void decode(QString str);
    Q_PROPERTY(int framesSent READ getFramesSent NOTIFY framesSentChanged FINAL);

    int m_framesSent;
    QString hexConvert(char input);


    Q_PROPERTY(int turnLamp READ getTurnLamp NOTIFY turnLampChanged FINAL)

    Q_PROPERTY(int fwiper READ getFwiper NOTIFY fwiperChanged FINAL)

    Q_PROPERTY(int fwiperSens READ getFwiperSens NOTIFY fwiperSensChanged FINAL)

    Q_PROPERTY(int shifterPos READ getShifterPos NOTIFY shifterPosChanged FINAL)

signals:
    void framesSentChanged();
    void highBeamSig();
    void flashBeamSig();
    void fwasherSig();
    void parkSig();
    void fwiperChanged();
    void turnLampChanged();
    void fwiperSensChanged();
    void shifterPosChanged();
};

#endif // CANCONTROLLER_H

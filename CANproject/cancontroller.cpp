#include "cancontroller.h"
#include "canLib.h"
#include "deviceLib.h"
CanController::CanController(QObject *parent)
    : QObject{parent}
{
    CAN_INIT_CONFIG canInit;
    m_framesSent = 0;
}

CanController *CanController::getInstance()
{
    static CanController * obj = new CanController();
    return obj;
}

void CanController::getMsg()
{
    CAN_MSG CanMsgBuffer[10240];
    int CanNum = CAN_GetMsg(devHandle[0], 1, CanMsgBuffer);
    QString totalData;
    if(CanNum > 0){
        qDebug()<<QString().asprintf("CanNum = %d", CanNum);
        for(int i = 0; i<CanNum; i++){ //prints the string for every frame count
            qDebug()<<QString().asprintf("CanMsg[%d].ID=%d", i, CanMsgBuffer[i].ID);
            qDebug()<<QString().asprintf("CanMsg[%d].TimeStamp = %d",i,CanMsgBuffer[i].TimeStamp);
            QString str;
            for(int j = 0; j<CanMsgBuffer[i].DataLen; j++){
                str.append(QString().asprintf("%02X ",CanMsgBuffer[i].Data[j]));
            }
            totalData.append(str);
            if(CanMsgBuffer[i].ID == 0x1b2){
                qDebug()<< "1b2 found, intercepting";
                decode(str);
            }
            qDebug()<<QString().asprintf("CanMsg[%d].Data = ",i)<<str;
        }
    }
    return;
}

int CanController::initPort()
{
    if(USB_ScanDevice(devHandle) <= 0){
        qDebug() << "No Device connected";
        return EXIT_FAILURE;
    }
    bool state;
    state = USB_OpenDevice(devHandle[0]);
    if(!state){
        qDebug()<<"Open Device error";
        return EXIT_FAILURE;
    }
    getFirmWareInfo(devHandle);
    return EXIT_SUCCESS;
}

int CanController::getFirmWareInfo(int* devHandle){
    char FunctionStr[256]={0};
    //获取固件信息
    DEVICE_INFO devInfo;
    bool state;
    state = DEV_GetDeviceInfo(devHandle[0], &devInfo, FunctionStr);
    if(!state){
        qDebug()<< "GET DEVICE INFOMATION ERROR";
        return EXIT_FAILURE;
    } else {
        qDebug()<<"Firmware Info:";
        qDebug()<<QString().asprintf("Firmware Name:%s", devInfo.FirmwareName);
        qDebug()<<QString().asprintf("Firmware Build Date:%s",devInfo.BuildDate);
        qDebug()<<QString().asprintf("Firmware Version:v%d.%d.%d",(devInfo.FirmwareVersion>>24)&0xFF,(devInfo.FirmwareVersion>>16)&0xFF,devInfo.FirmwareVersion&0xFFFF);
        qDebug()<<QString().asprintf("Hardware Version:v%d.%d.%d",(devInfo.HardwareVersion>>24)&0xFF,(devInfo.HardwareVersion>>16)&0xFF,devInfo.HardwareVersion&0xFFFF);
        qDebug()<<QString().asprintf("Firmware Functions:%s",FunctionStr);
    }
    canInit(devHandle);
    return EXIT_SUCCESS;
}

int CanController::canInit(int* devHandle){

    CAN_FILTER_CONFIG CANFilter;
    CANFilter.Enable = 1;
    CANFilter.ExtFrame = 0;
    CANFilter.FilterIndex = 0;
    CANFilter.FilterMode = 0;
    CANFilter.MASK_IDE = 1;
    CANFilter.MASK_RTR = 1;
    CANFilter.MASK_Std_Ext = -1;
    CAN_Filter_Init(devHandle[0],1,&CANFilter);
    CAN_INIT_CONFIG CANConfig;
    int ret = CAN_GetCANSpeedArg(devHandle[0],&CANConfig,500000);
    if(ret != CAN_SUCCESS){
        printf("Get CAN Speed Arg Failed!\n");
        return 0;
    }else{
        printf("Get CAN Speed Arg Success!\n");
    }
    // CANConfig.CAN_Mode = 0;
    // CANConfig.CAN_ABOM = 0;//禁止自动离线
    // CANConfig.CAN_NART = 1;//禁止报文重传
    // CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
    // CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
    // //配置波特率,波特率 = 42M/(BRP*(SJW+BS1+BS2))
    // CANConfig.CAN_BRP = 4;
    // CANConfig.CAN_BS1 = 16;
    // CANConfig.CAN_BS2 = 4;
    // CANConfig.CAN_SJW = 1;
    if(CAN_Init(devHandle[0],1, &CANConfig) != CAN_SUCCESS) { //important configure function)
        qDebug() << "Config CAN failed";
        return EXIT_FAILURE;
    } else {
        qDebug() << "Config CAN Success";
    }
    return EXIT_SUCCESS;
}

int CanController::initGet(int* devHandle)
{
    CAN_STATUS CANStatus;
    if(CAN_GetStatus(devHandle[0], 1, &CANStatus) == CAN_SUCCESS){
        qDebug()<<QString().asprintf("TSR = %08X",CANStatus.TSR);
        qDebug()<<QString().asprintf("ESR = %08X",CANStatus.ESR);
    } else {
        qDebug() << QString().asprintf("Get CAN status error!");

    }

    return EXIT_SUCCESS;
}

void CanController::decode(QString str)
{
    QString result = "";
    m_framesSent++;
    emit framesSentChanged();
    for(int i=0; i<str.length(); i++){
        result.append(hexConvert(str.at(i).toLatin1()));
    }
    qDebug() << result;
    if(result.at(26) == '0' && result.at(30) == '0' && result.at(31) == '1'){ //ommitting double check conditions for testing purposes
        qDebug() << "P has been pressed";
        emit pressed();
    }
    //gearShiftPos == 16 - 19, gearShiftInverse == 20 - 23
    //forward1 03, 0C; forward2 02, 0D; backward1 04, 0B; backward2 05, 0A
    //normal 00, 0f
    qDebug() << str;
    if(str.at(6) == 'C' && str.at(7) == '3'){
        emit forwardOne();
    }
    if(str.at(6) == 'D' && str.at(7) == '2'){
        emit forwardTwo();
    }
    if(str.at(6) == 'B' && str.at(7) == '4'){
        emit backWardOne();
    }
    if(str.at(6) == 'A' && str.at(7) == '5'){
        emit backWardTwo();
    }
    if(str.at(6) == 'F' && str.at(7) == '0'){
        emit normal();
    }
}

QString CanController::hexConvert(char input)
{
    switch(toupper(input))
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return "";
    }
}

int CanController::getFramesSent() const
{
    return m_framesSent;
}

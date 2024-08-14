#include "cancontroller.h"
#include "deviceLib.h"
#include "CANFDLib.h"
#include "offline_type.h"
CanController::CanController(QObject *parent)
    : QObject{parent}
{
    CANFD_INIT_CONFIG canInit;
    m_framesSent = 0;
    turnLamp = 0;
    fwiper = 0;
    fwiperSens = 0;
    shifterPos = 0;
}

CanController *CanController::getInstance()
{
    static CanController * obj = new CanController();
    return obj;
}

void CanController::getMsg()
{
    CANFD_MSG CanMsgBuffer[1024];
    int CanNum = CANFD_GetMsg(devHandle[0], 1, CanMsgBuffer, 1024);
    if(CanNum > 0){
        qDebug()<<QString().asprintf("CanNum = %d", CanNum);
        for(int i = 0; i<CanNum; i++){ //prints the string for every frame count
            uint64_t tus = ((((uint64_t)CanMsgBuffer[i].TimeStampHigh<<32)|CanMsgBuffer[i].TimeStamp)*10.0);
            qDebug()<<QString().asprintf("CanMsg[%d].ID = 0x%08X\n", i, CanMsgBuffer[i].ID & CANFD_MSG_FLAG_ID_MASK);
            qDebug()<<QString().asprintf("CanMsg[%d].TimeStamp = %.6f s\n", i, tus/1000000.0);
            qDebug("CanMsg[%d].Data = ", i);
            QString str;
            for(int j = 0; j<CanMsgBuffer[i].DLC; j++){
                str.append(QString().asprintf("%02X ",CanMsgBuffer[i].Data[j]));
            }
            if(CanMsgBuffer[i].ID == 0x1f1){
                qDebug()<< "1f1 found, intercepting";
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

    CANFD_FILTER_CONFIG CANFilter;
    CANFilter.Enable = 1;
    CANFilter.Index = 0;
    CANFilter.__Res0 = 0;
    CANFilter.__Res1 = 0;
    CANFilter.ID_Accept = 0;
    CANFilter.ID_Mask = 0;
    if(CANFD_SetFilter(devHandle[0],1,&CANFilter,1) == CANFD_SUCCESS){
        qDebug() << "Filter initialization success";
    } else {
        qDebug() << "Filter initialization failed";
        return 0;
    }
    CANFD_INIT_CONFIG CANConfig;
    int ret = CANFD_GetCANSpeedArg(devHandle[0],&CANConfig,500000, 2000000);
    if(ret != WN_SUCCESS){
        qDebug() << "Get CANFD Speed Arg Failed!";
        return 0;
    }else{
        qDebug() << "Get CANFD Speed Arg Success!";
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
    if(CANFD_Init(devHandle[0],1, &CANConfig) != WN_SUCCESS) { //important configure function)
        qDebug() << "Config CAN failed";
        return EXIT_FAILURE;
    } else {
        qDebug() << "Config CAN Success";
    }
    return EXIT_SUCCESS;
}

int CanController::initGet(int* devHandle)
{
    int ret = CANFD_StartGetMsg(devHandle[0], 1);
    if(ret != CANFD_SUCCESS){
        qDebug() << "Start receive CANFD failed";
        return 0;
    } else {
        qDebug() << "Start receive CANFD Success";
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
    //
    // if(result.at(26) == '0' && result.at(30) == '0' && result.at(31) == '1'){ //ommitting double check conditions for testing purposes
    //     qDebug() << "P has been pressed";
    //     emit pressed();
    if(result.at(1) == '0'){
        if(result.at(2) == '0' && result.at(3) == '1')
            turnLamp = 1;
        if(result.at(2) == '0' && result.at(3) == '0')
            turnLamp = 0;
        if(result.at(2) == '1' && result.at(3) == '0')
            turnLamp = 2;
        if(result.at(2) == '1' && result.at(3) == '1')
            turnLamp = 3;
    } else if(result.at(1) == '1' && result.at(2) == '0' && result.at(3) == '0')
        turnLamp = 4;
    if(result.at(5) == '1' && result.at(4) == '0')
        emit highBeamSig();
    if(result.at(7) == '0' && result.at(6) == '1')
        emit flashBeamSig();
    if(result.at(14) == '1')
        emit fwasherSig();
    if(result.at(16) == '0'){ //not sure about 16
        if(result.at(12) == '0'){
            if(result.at(13) == '0')
                fwiper = 0;
            else
                fwiper = 1;
        } else {
            if(result.at(13) == '0')
                fwiper = 2;
            else
                fwiper = 3;
        }
    }
    if(result.at(15) == '0'){
        if(result.at(8) == '0')
            fwiperSens = 0;
        else
            fwiperSens = 1;
    } else
        fwiperSens = 2;
    if(result.at(23) == '0' && result.at(16) == '0' && result.at(17) == '0' && result.at(18) == '0' && result.at(28) == '0'
        && result.at(29) == '1' && result.at(30) == '0' && result.at(31) == '1'){
        emit parkSig();
    }
    if(result.at(11) == '0'){
        if(result.at(20) == '0'){
            if(result.at(21) == '0'){
                if(result.at(22) == '0')
                    shifterPos = 0;
                else
                    shifterPos = 1;
            } else {
                if(result.at(22) == '0')
                    shifterPos = 2;
                else
                    shifterPos = 3;
            }
        } else {
            if(result.at(21) == '0')
                if(result.at(22) == '0')
                    shifterPos = 4;
        }
    }

    //gearShiftPos == 16 - 19, gearShiftInverse == 20 - 23
    //forward1 03, 0C; forward2 02, 0D; backward1 04, 0B; backward2 05, 0A
    //normal 00, 0f
    qDebug() << str;

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

int CanController::getTurnLamp() const
{
    return turnLamp;
}

int CanController::getFwiper() const
{
    return fwiper;
}

int CanController::getFwiperSens() const
{
    return fwiperSens;
}

int CanController::getShifterPos() const
{
    return shifterPos;
}







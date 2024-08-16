#include "cancontroller.h"
#include "canLib.h"
#include "deviceLib.h"
#include "CANFDLib.h"
#include "can_uds.h"
CanController::CanController(QObject *parent)
    : QObject{parent}
{
    CAN_INIT_CONFIG canInit;
    m_framesSent = 0;
    gears = 2;
    lastEmitted = 2;
    orig = 0;
    physical = 0;
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
    if(ret != CAN_SUCCESS){
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
    if(CANFD_Init(devHandle[0],1, &CANConfig) != CAN_SUCCESS) { //important configure function)
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
    if(result.at(26) == '0' && result.at(30) == '0' && result.at(31) == '1'){ //ommitting double check conditions for testing purposes
        qDebug() << "P has been pressed";
        emit pressed();
        gears = 0;
        lastEmitted = -1;
        emit gearsChanged();
    }
    //gearShiftPos == 16 - 19, gearShiftInverse == 20 - 23
    //forward1 03, 0C; forward2 02, 0D; backward1 04, 0B; backward2 05, 0A
    //normal 00, 0f
    //lastEmitted 0, backWard 2, lastEmitted 1 backward 1, lastEmitted 2, neutral, lastEmitted 3, forward 1, lastEmitted 4, forward 2
    qDebug() << str;
    if(str.at(6) == 'C' && str.at(7) == '3'){
        emit forwardOne();
        if(lastEmitted != 3){
            if(gears != 0 && gears < 3)
                gears++;
            else if(gears == 0)
                gears = 2;
            emit gearsChanged();
        }
        lastEmitted = 3;
    }
    if(str.at(6) == 'D' && str.at(7) == '2'){
        emit forwardTwo();
        if(lastEmitted != 4){
            if(gears == 0)
                gears = 2;
            else
                gears = 3;
            emit gearsChanged();
        }
        lastEmitted = 4;
    }
    if(str.at(6) == 'B' && str.at(7) == '4'){
        emit backWardOne();
        if(lastEmitted != 1){
            if(gears == 0)
                gears = 2;
            else if(gears != 1)
                gears--;
            emit gearsChanged();
        }
        lastEmitted = 1;
    }
    if(str.at(6) == 'A' && str.at(7) == '5'){
        emit backWardTwo();
        if(lastEmitted != 0){
            if(gears == 0)
                gears = 2;
            else
                gears = 1;
        emit gearsChanged();
        }
        lastEmitted = 0;
    }
    if(str.at(6) == 'F' && str.at(7) == '0'){
        emit normal();
        lastEmitted = 2;
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

void CanController::udsRead()
{
    int ret;
    QString ans;
    for (int t = 0; t < 2; t++){
        //设置请求数据
        CAN_UDS_ADDR UDSAddr;
        UDSAddr.Flag = 0;//使用标准帧
        UDSAddr.AddrFormats = 0;
        UDSAddr.ReqID = 0x72B;
        UDSAddr.ResID = 0x73B;
        UDSAddr.MaxDLC = 8;
        uint8_t req_data[]={0x22,0x50,0x66};//第一字节为服务ID(SID)，后面的数据为对应的参数
        uint8_t res_data[4096]={0};
        ret = CAN_UDS_Request(devHandle[0],1,&UDSAddr,req_data,sizeof(req_data));
        if(ret != CAN_UDS_OK){
            qDebug() << QString().asprintf("CAN UDS request failed! %d\r\n",ret);
        }else{
            qDebug() << "Request:";
            for(int i=0;i<sizeof(req_data);i++){
                qDebug() << QString().asprintf(" %02X",req_data[i]);
            }
            qDebug() << "\r\n";
        }
        //获取响应数据，有可能因为设备会进入睡眠模式，若第一次无法正常获取响应，可以延时100ms之后再次请求获取响应
        ret = CAN_UDS_Response(devHandle[0],1,&UDSAddr,res_data,200);
        if(ret <= CAN_UDS_OK){
            qDebug() << QString().asprintf("CAN UDS response failed! %d",ret);
        }else{
            qDebug() << QString().asprintf("Response:");
            char inChar;
            for(int i=0;i<ret;i++){
                inChar = res_data[i];
                ans.append(inChar);
                qDebug() << QString().asprintf(" %02X",res_data[i]);
            }
            qDebug() << ans;
            orig = ans.last(ans.length() - ans.lastIndexOf('f') - 1).toInt();
            qDebug() << "orig is: " << orig;
            physical = orig * 180 / 65536 * 2;
            emit origChanged();
            emit physicalChanged();

        }
    }
}

int CanController::getGears() const
{
    return gears;
}

int CanController::getOrig() const
{
    return orig;
}

int CanController::getPhysical() const
{
    return physical;
}




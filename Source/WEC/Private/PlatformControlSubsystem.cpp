// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformControlSubsystem.h"
#include "Sockets.h"
#include "SocketSubsystem.h"


DEFINE_LOG_CATEGORY(PlatformControlSubsystem);



void UPlatformControlSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (StartUDPSocket())
    {
        UDPSender = MakeUnique<FUDPSender>(UDPSocket, *RemoteAddr);
    };

}

void UPlatformControlSubsystem::Deinitialize()
{
    if (UDPSender)
    {
        UDPSender->Stop();
    }

	CloseUDPSocket();
	Super::Deinitialize();
}

bool UPlatformControlSubsystem::StartUDPSocket()
{
	// 获取Socket子系统
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem) {
        UE_LOG(PlatformControlSubsystem, Error, TEXT("Failed to get socket subsystem!"));
        return false;
    }

    // 创建UDP Socket
    UDPSocket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("MotionControlSocket"), false);
    if (!UDPSocket) {
        UE_LOG(PlatformControlSubsystem, Error, TEXT("Failed to create UDP socket!"));
        return false;
    }

    // 设置目标地址：192.168.31.88:8080(默认)
    RemoteAddr = SocketSubsystem->CreateInternetAddr();
    bool bIsValid;
    RemoteAddr->SetIp(*IP, bIsValid);
    RemoteAddr->SetPort(Port);

    if (!bIsValid) {
        UE_LOG(PlatformControlSubsystem, Error, TEXT("Invalid IP address!"));
        CloseUDPSocket();
        return false;
    }

    UE_LOG(PlatformControlSubsystem, Log, TEXT("UDP Socket started successfully!"));
    return true;
}

void UPlatformControlSubsystem::CloseUDPSocket()
{
	 if (UDPSocket) {
        UDPSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(UDPSocket);
        UDPSocket = nullptr;
        UE_LOG(PlatformControlSubsystem, Log, TEXT("UDP Socket closed."));
    }
}


bool UPlatformControlSubsystem::RestartUDPSocket()
{
     // 先关闭现有的Socket（如果存在）
    CloseUDPSocket();

     // 重新启动Socket
    
    
    if (StartUDPSocket() && UDPSender)
    {
        UDPSender->ResetRemoteAddr(UDPSocket,*RemoteAddr);
        if (bRunning)
        {
            UDPSender->Start(SendInterval);
        }
        return true;
    }
    else
    {
        return false;
    }
	
    
}

bool UPlatformControlSubsystem::Send3AxisAttitudeControl(float XRot, float YRot, float ZHeight, uint8 SpeedLevel,bool Send)
{
	    if (!UDPSocket || !RemoteAddr.IsValid()) {
        UE_LOG(PlatformControlSubsystem, Error, TEXT("UDP Socket is not initialized!"));
        return false;
    }

    // 构造数据包（十七字节协议）
    TArray<uint8> Packet;
    Packet.SetNumUninitialized(17);

    // 帧头（0xA5）
    Packet[0] = 0xA5;

    // CMD（0x18）
    Packet[1] = 0x18;

    // DATA部分：Z轴高度（float）、X旋转（A轴）、Y旋转（B轴）
    auto FillFloatToBytes = [&](float Value, int32 StartIndex) {
        uint8* Bytes = reinterpret_cast<uint8*>(&Value);
        for (int32 i = 0; i < 4; i++) {
            // 小端序（根据文档示例判断）
            Packet[StartIndex + i] = Bytes[i];
        }
    };

    // Z轴高度（DATA0-DATA3）
    FillFloatToBytes(ZHeight, 2);

    // X轴旋转角度（A轴，DATA4-DATA7）
    FillFloatToBytes(XRot, 6);

    // Y轴旋转角度（B轴，DATA8-DATA11）
    FillFloatToBytes(YRot, 10);

    // 速度等级（BYTE14）
    Packet[14] = SpeedLevel;

    // 计算CRC16-MODBUS校验（覆盖BYTE15-BYTE16）
    uint16 CRC = 0xFFFF;
    for (int32 i = 0; i < 15; i++) { // 计算前15字节的CRC
        CRC ^= Packet[i];
        for (int32 j = 0; j < 8; j++) {
            CRC = (CRC & 1) ? (CRC >> 1) ^ 0xA001 : CRC >> 1;
        }
    }

    // 填充校验位（低字节在前）
    Packet[15] = static_cast<uint8>(CRC & 0xFF);   // 低字节
    Packet[16] = static_cast<uint8>(CRC >> 8);     // 高字节

    //// 发送数据
    if (Send){
    int32 BytesSent;
        if (!UDPSocket->SendTo(Packet.GetData(), Packet.Num(), BytesSent, *RemoteAddr)) {
            UE_LOG(PlatformControlSubsystem, Error, TEXT("Failed to send data!"));
            return false;
        }
        UE_LOG(PlatformControlSubsystem, Log, TEXT("Data send successfully!"));
        return true;
    }

    UDPSender->SetSendData(Packet);
  
    UE_LOG(PlatformControlSubsystem, Log, TEXT("Data set successfully!"));
    return true;
}

bool UPlatformControlSubsystem::SendPlatformReset()
{
    if (!UDPSocket || !RemoteAddr.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("UDP Socket is not initialized!"));
        return false;
    }

    // 构造数据包（八字节协议）
    TArray<uint8> Packet;
    Packet.SetNumUninitialized(8);

    // 帧头（0xA5）
    Packet[0] = 0xA5;

    // CMD（0x77）
    Packet[1] = 0x77;

    // DATA部分：全部为0（保留字节）
    for (int32 i = 2; i < 6; i++) {
        Packet[i] = 0x00;
    }

    // 计算CRC16-MODBUS校验（覆盖BYTE0-BYTE5）
    uint16 CRC = 0xFFFF;
    for (int32 i = 0; i < 6; i++) { // 计算前6字节的CRC
        CRC ^= Packet[i];
        for (int32 j = 0; j < 8; j++) {
            CRC = (CRC & 1) ? (CRC >> 1) ^ 0xA001 : CRC >> 1;
        }
    }

    // 填充校验位（低字节在前）
    Packet[6] = static_cast<uint8>(CRC & 0xFF);   // 低字节
    Packet[7] = static_cast<uint8>(CRC >> 8);     // 高字节

    // 发送数据
    int32 BytesSent;
    if (!UDPSocket->SendTo(Packet.GetData(), Packet.Num(), BytesSent, *RemoteAddr)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to send platform reset command!"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Platform reset command sent successfully!"));
    return true;
}

void UPlatformControlSubsystem::RunSendThread()
{

    if (UDPSender)
    {
        bRunning = true;
        UDPSender->Start(SendInterval);
    }
}

void UPlatformControlSubsystem::StopSendThread()
{
    if (UDPSender)
    {
        bRunning = false;
        UDPSender->Stop();
        
    }
}

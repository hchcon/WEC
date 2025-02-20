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
	// ��ȡSocket��ϵͳ
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem) {
        UE_LOG(PlatformControlSubsystem, Error, TEXT("Failed to get socket subsystem!"));
        return false;
    }

    // ����UDP Socket
    UDPSocket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("MotionControlSocket"), false);
    if (!UDPSocket) {
        UE_LOG(PlatformControlSubsystem, Error, TEXT("Failed to create UDP socket!"));
        return false;
    }

    // ����Ŀ���ַ��192.168.31.88:8080(Ĭ��)
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
     // �ȹر����е�Socket��������ڣ�
    CloseUDPSocket();

     // ��������Socket
    
    
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

    // �������ݰ���ʮ���ֽ�Э�飩
    TArray<uint8> Packet;
    Packet.SetNumUninitialized(17);

    // ֡ͷ��0xA5��
    Packet[0] = 0xA5;

    // CMD��0x18��
    Packet[1] = 0x18;

    // DATA���֣�Z��߶ȣ�float����X��ת��A�ᣩ��Y��ת��B�ᣩ
    auto FillFloatToBytes = [&](float Value, int32 StartIndex) {
        uint8* Bytes = reinterpret_cast<uint8*>(&Value);
        for (int32 i = 0; i < 4; i++) {
            // С���򣨸����ĵ�ʾ���жϣ�
            Packet[StartIndex + i] = Bytes[i];
        }
    };

    // Z��߶ȣ�DATA0-DATA3��
    FillFloatToBytes(ZHeight, 2);

    // X����ת�Ƕȣ�A�ᣬDATA4-DATA7��
    FillFloatToBytes(XRot, 6);

    // Y����ת�Ƕȣ�B�ᣬDATA8-DATA11��
    FillFloatToBytes(YRot, 10);

    // �ٶȵȼ���BYTE14��
    Packet[14] = SpeedLevel;

    // ����CRC16-MODBUSУ�飨����BYTE15-BYTE16��
    uint16 CRC = 0xFFFF;
    for (int32 i = 0; i < 15; i++) { // ����ǰ15�ֽڵ�CRC
        CRC ^= Packet[i];
        for (int32 j = 0; j < 8; j++) {
            CRC = (CRC & 1) ? (CRC >> 1) ^ 0xA001 : CRC >> 1;
        }
    }

    // ���У��λ�����ֽ���ǰ��
    Packet[15] = static_cast<uint8>(CRC & 0xFF);   // ���ֽ�
    Packet[16] = static_cast<uint8>(CRC >> 8);     // ���ֽ�

    //// ��������
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

    // �������ݰ������ֽ�Э�飩
    TArray<uint8> Packet;
    Packet.SetNumUninitialized(8);

    // ֡ͷ��0xA5��
    Packet[0] = 0xA5;

    // CMD��0x77��
    Packet[1] = 0x77;

    // DATA���֣�ȫ��Ϊ0�������ֽڣ�
    for (int32 i = 2; i < 6; i++) {
        Packet[i] = 0x00;
    }

    // ����CRC16-MODBUSУ�飨����BYTE0-BYTE5��
    uint16 CRC = 0xFFFF;
    for (int32 i = 0; i < 6; i++) { // ����ǰ6�ֽڵ�CRC
        CRC ^= Packet[i];
        for (int32 j = 0; j < 8; j++) {
            CRC = (CRC & 1) ? (CRC >> 1) ^ 0xA001 : CRC >> 1;
        }
    }

    // ���У��λ�����ֽ���ǰ��
    Packet[6] = static_cast<uint8>(CRC & 0xFF);   // ���ֽ�
    Packet[7] = static_cast<uint8>(CRC >> 8);     // ���ֽ�

    // ��������
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

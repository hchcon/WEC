#include "UDPSender.h"
#include "HAL/PlatformProcess.h"


FUDPSender::FUDPSender(FSocket* InSocket, const FInternetAddr& InRemoteAddr)
    : Socket(InSocket),
      RemoteAddr(ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr()),
      bRunning(false)
{
    // 克隆远程地址（避免地址被外部修改）
    uint32 OutIP;
    uint16 OutPort;
    InRemoteAddr.GetIp(OutIP);
    OutPort = InRemoteAddr.GetPort();
    RemoteAddr->SetIp(OutIP);
    RemoteAddr->SetPort(OutPort);
}

FUDPSender::~FUDPSender()
{
     Stop();
}

void FUDPSender::Start(float outSendInterval)
{

    if (!Thread.IsValid() && Socket && Socket->GetSocketType() == SOCKTYPE_Datagram)
    {
        SendInterval = outSendInterval;
        bRunning = true;
        Thread.Reset(FRunnableThread::Create(this, TEXT("UDPSenderThread"), 0, TPri_AboveNormal));
    }
}

void FUDPSender::Stop()
{
     bRunning = false;
    if (Thread.IsValid())
    {
        Thread->WaitForCompletion();
        Thread.Reset();
    }
}

void FUDPSender::SetSendData(const TArray<uint8>& Data)
{
    FScopeLock Lock(&BufferMutex);
    SendBuffer = Data;
}

void FUDPSender::ResetRemoteAddr(const FInternetAddr& InRemoteAddr)
{
    Stop();
    uint32 OutIP;
    uint16 OutPort;
    InRemoteAddr.GetIp(OutIP);
    OutPort = InRemoteAddr.GetPort();
    RemoteAddr->SetIp(OutIP);
    RemoteAddr->SetPort(OutPort);
}


uint32 FUDPSender::Run()
{
    
    double LastSendTime = FPlatformTime::Seconds();

    while (bRunning)
    {
        const double CurrentTime = FPlatformTime::Seconds();
        const double DeltaTime = CurrentTime - LastSendTime;

        // 精确等待10ms
        if (DeltaTime >= SendInterval)
        {
            if (SendBuffer != LastBuffer)
            {            
                // 发送数据
                TArray<uint8> LocalBuffer;
                {
                    FScopeLock Lock(&BufferMutex);
                    LocalBuffer = SendBuffer;
                    LastBuffer = LocalBuffer;
                }
           
           
                if (LocalBuffer.Num() > 0 && Socket && Socket->GetSocketType() == SOCKTYPE_Datagram)
                {
                    int32 BytesSent = 0;
                    if (!Socket->SendTo(LocalBuffer.GetData(), LocalBuffer.Num(), BytesSent, *RemoteAddr))
                    {
                        UE_LOG(LogTemp, Error, TEXT("Data send Failed!"));
                    } ;
                    UE_LOG(LogTemp, Log, TEXT("Data send successfully!"));
                }
           
            }

            LastSendTime = CurrentTime;
        }
        else
        {
            // 计算剩余等待时间（避免忙等待）
            const float RemainingTime = SendInterval - DeltaTime;
            FPlatformProcess::Sleep(FMath::Max(0.0f, RemainingTime - 0.001f));
        }
    }
    return 0;
}

bool FUDPSender::Init()
{
    UE_LOG(LogTemp, Log, TEXT("Background thread initialized."));
    return true;
}

void FUDPSender::Exit()
{
    UE_LOG(LogTemp, Log, TEXT("Background thread exited."));
    bRunning = false;
}
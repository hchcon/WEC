#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
//#include "Interfaces/IPv4/IPv4Address.h"

class WEC_API FUDPSender : public FRunnable
{
public:

    FUDPSender(FSocket* InSocket, const FInternetAddr& InRemoteAddr);
    virtual ~FUDPSender();
    // 启动线程
    void Start(float outSendInterval);
    // 停止线程
    void Stop();
    // 设置要发送的数据（线程安全）
    void SetSendData(const TArray<uint8>& Data);

    void ResetRemoteAddr(const FInternetAddr& InRemoteAddr);

protected:
    // FRunnable 接口实现
    virtual uint32 Run() override; // 线程主逻辑
    virtual bool Init() override;  // 线程初始化
    virtual void Exit() override;  // 线程退出


private:
    float SendInterval = 0.01f;             // 10ms周期
    FSocket* Socket;                        // 已初始化的UDP Socket
    TSharedRef<FInternetAddr> RemoteAddr;   // 目标地址
    TArray<uint8> SendBuffer;               // 发送缓冲区
    FCriticalSection BufferMutex;           // 缓冲区互斥锁
    FThreadSafeBool bRunning;               // 线程运行标志
    TUniquePtr<FRunnableThread> Thread;     // 线程对象
};
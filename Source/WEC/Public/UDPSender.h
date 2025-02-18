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
    // �����߳�
    void Start(float outSendInterval);
    // ֹͣ�߳�
    void Stop();
    // ����Ҫ���͵����ݣ��̰߳�ȫ��
    void SetSendData(const TArray<uint8>& Data);

    void ResetRemoteAddr(const FInternetAddr& InRemoteAddr);

protected:
    // FRunnable �ӿ�ʵ��
    virtual uint32 Run() override; // �߳����߼�
    virtual bool Init() override;  // �̳߳�ʼ��
    virtual void Exit() override;  // �߳��˳�


private:
    float SendInterval = 0.01f;             // 10ms����
    FSocket* Socket;                        // �ѳ�ʼ����UDP Socket
    TSharedRef<FInternetAddr> RemoteAddr;   // Ŀ���ַ
    TArray<uint8> SendBuffer;               // ���ͻ�����
    FCriticalSection BufferMutex;           // ������������
    FThreadSafeBool bRunning;               // �߳����б�־
    TUniquePtr<FRunnableThread> Thread;     // �̶߳���
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UDPSender.h"
#include "PlatformControlSubsystem.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(PlatformControlSubsystem,Log,All)
/**
 * UPlatformControlSubsystem
 */
UCLASS()
class WEC_API UPlatformControlSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()




public:
	UFUNCTION(BlueprintCallable)
	bool RestartUDPSocket();

	UFUNCTION(BlueprintCallable)
	bool Send3AxisAttitudeControl(float XRot, float YRot, float ZHeight, uint8 SpeedLevel = 1,bool Send = false);

	//地台归零
	UFUNCTION(BlueprintCallable)
	bool SendPlatformReset();

	UFUNCTION(BlueprintCallable)
	void RunSendThread();

	UFUNCTION(BlueprintCallable)
	void StopSendThread();

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FString IP = "192.168.31.88";

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	int32 Port = 8080;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float SendInterval = 0.01f;  

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bRunning = false;

	// 是否反转Roll（XRot）符号
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axis Configuration")
    bool bInvertRoll = false;

    // 是否反转Pitch（YRot）符号
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axis Configuration")
    bool bInvertPitch = false;

    // 是否交换Roll（XRot）和Pitch（YRot）的轴向映射
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axis Configuration")
    bool bSwapAxes = false;

protected:
		virtual	void Initialize(FSubsystemCollectionBase& Collection) override;
		virtual void Deinitialize() override;

private:
	bool StartUDPSocket();
	void CloseUDPSocket();
	
	

	FSocket* UDPSocket = nullptr;
	TSharedPtr<FInternetAddr> RemoteAddr;

	TUniquePtr<FUDPSender> UDPSender;
};

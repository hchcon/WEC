// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnBase.generated.h"

class UCameraComponent;

UCLASS()
class WEC_API APawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnBase();

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> VRCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float RotateThreshold; // 旋转阈值

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float RotatingTime; // 旋转时间

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    bool ShouldRotateX; // 是否应该旋转X

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool ShouldRotateY; // 是否应该旋转Y

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    FRotator VRCameraRotation; // VR 摄像头的旋转角度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	FVector VRCameraLocation; // VR 摄像头的位置

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    FRotator PlatformRotation; // 平台的旋转角度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float TargetX; // 目标 X 轴旋转角度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float TargetY; // 目标 Y 轴旋转角度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float TargetZ; // 目标 Z 轴旋转角度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float ErrorTolerance = .5f; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // 旋转逻辑
    UFUNCTION(BlueprintCallable)
    void RotateLogic(float AngleDifference,bool bIsX);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

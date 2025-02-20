// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnBase.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlatformControlSubsystem.h"

// Sets default values
APawnBase::APawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	VRCamera->SetupAttachment(RootComponent);
	VRCamera->SetRelativeLocation(FVector(0, 0, 0));
	VRCamera->bUsePawnControlRotation = true;

    RotateThreshold = 0.1f;
    RotatingTime = 0.0f;
    ShouldRotateX = false;
    ShouldRotateY = false;
    VRCameraRotation = FRotator::ZeroRotator;
    PlatformRotation = FRotator::ZeroRotator;
    TargetX = 0.0f;
    TargetY = 0.0f;
    TargetZ = 0.0f;
}

// Called when the game starts or when spawned
void APawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void APawnBase::RotateLogic(float AngleDifference, bool bIsX)
{
	// 判断是否需要旋转
    if (AngleDifference > RotateThreshold)
    {
        if (bIsX)
        {
            ShouldRotateX = true;
        }
        else
        {
            ShouldRotateY = true;
        }
        
        
    }

    if (UKismetMathLibrary::NearlyEqual_FloatFloat(AngleDifference, 0, ErrorTolerance))
    {
		// FString AD  = FString::Printf(TEXT("%.2f"),AngleDifference);
		// UKismetSystemLibrary::PrintString(this, AD, true, true, FLinearColor::Red, 2.0f);
        if (bIsX)
        {
            ShouldRotateX = false;
        }
        else
        {
            ShouldRotateY = false;
        }

    }

	if (bIsX ? ShouldRotateX : ShouldRotateY)
    {
        float deltaSeconds = 0.02f;
        // 更新旋转时间
        RotatingTime += deltaSeconds;


        // 插值计算旋转角度
        float InterpSpeed = UKismetMathLibrary::MapRangeClamped(RotatingTime,0,0.5,1,5);

        if (bIsX)
        {
            TargetX = UKismetMathLibrary::FInterpTo(PlatformRotation.Roll,VRCameraRotation.Roll,deltaSeconds,InterpSpeed);
        }
        else
        {
            TargetY = UKismetMathLibrary::FInterpTo(PlatformRotation.Pitch,VRCameraRotation.Pitch,deltaSeconds,InterpSpeed);
        }

       
        

        // 发送旋转数据到外部系统
        UPlatformControlSubsystem* PawnPlatformControlSubsystem = GetGameInstance()->GetSubsystem<UPlatformControlSubsystem>();
        if (PawnPlatformControlSubsystem)
        {
            PawnPlatformControlSubsystem->Send3AxisAttitudeControl(TargetX, TargetY, TargetZ, 1, false);
        }

        // 设置平台旋转角度
        PlatformRotation.Roll = TargetX;
        PlatformRotation.Pitch = TargetY;

        FString RotationString = FString::Printf(TEXT("Target Rotation: X=%.2f, Y=%.2f, Z=%.2f"), TargetX, TargetY, TargetZ);
        UKismetSystemLibrary::PrintString(this, RotationString, true, true, FLinearColor::Yellow, 1.0f);
    }
    else
    {
        // 重置旋转时间
        RotatingTime = 0.0f;
    }
}

// Called every frame
void APawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


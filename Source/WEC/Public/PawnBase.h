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
    float RotateThreshold; // ��ת��ֵ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float RotatingTime; // ��תʱ��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    bool ShouldRotateX; // �Ƿ�Ӧ����תX

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool ShouldRotateY; // �Ƿ�Ӧ����תY

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    FRotator VRCameraRotation; // VR ����ͷ����ת�Ƕ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	FVector VRCameraLocation; // VR ����ͷ��λ��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    FRotator PlatformRotation; // ƽ̨����ת�Ƕ�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float TargetX; // Ŀ�� X ����ת�Ƕ�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float TargetY; // Ŀ�� Y ����ת�Ƕ�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float TargetZ; // Ŀ�� Z ����ת�Ƕ�

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float ErrorTolerance = .5f; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // ��ת�߼�
    UFUNCTION(BlueprintCallable)
    void RotateLogic(float AngleDifference,bool bIsX);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

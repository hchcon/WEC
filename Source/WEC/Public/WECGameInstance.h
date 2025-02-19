// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WECGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WEC_API UWECGameInstance : public UGameInstance
{
	GENERATED_BODY()



public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float RotateThreshold = 2.f;
	
};

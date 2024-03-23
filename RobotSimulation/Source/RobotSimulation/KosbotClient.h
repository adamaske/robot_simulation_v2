// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TcpSocketConnection.h"
#include "KosbotClient.generated.h"

/**
 * 
 */
class ARobot;
UCLASS()
class ROBOTSIMULATION_API AKosbotClient : public ATcpSocketConnection
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TObjectPtr<ARobot> m_Robot;

	UFUNCTION(BlueprintCallable)
	void Parse_JSON_From_TCP(TArray<uint8> bytes);

};

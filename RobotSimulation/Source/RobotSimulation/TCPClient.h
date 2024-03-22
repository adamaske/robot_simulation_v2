// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TcpSocketConnection.h"
#include "TCPClient.generated.h"

class ARobot;
/**
 * 
 */
UCLASS()
class ROBOTSIMULATION_API ATCPClient : public ATcpSocketConnection
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TObjectPtr<ARobot> robot;

	UFUNCTION(BlueprintCallable)
	void Parse_JSON_From_TCP(TArray<uint8> bytes);

	
public:

};

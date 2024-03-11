// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Server.generated.h"

class ARobot;

UCLASS()
class ROBOTSIMULATION_API AServer : public AActor
{
	GENERATED_BODY()
	
private:		
	int b_Running = false;

	int b_Connected = false;
	WSADATA wsaData;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TObjectPtr<ARobot> m_Robot;
public:	
	// Sets default values for this actor's properties
	AServer();
	~AServer();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	int Start();
	int Connect();
	int Listen();
	int Shutdown();
	int Close(); // 0 failed to close, 1 closed
};

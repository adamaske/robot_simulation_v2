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

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Server.generated.h"

class ARobot;

UCLASS()
class ROBOTSIMULATION_API AServer : public AActor
{
	GENERATED_BODY()
	
private:		
	int running = false;
	int connected = false;

	WSADATA wsa_data;

	SOCKET listen_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TObjectPtr<ARobot> robot;
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

	int InitWSA();

	int StartListening();
	int Listen();//Set listen_socket in listening mode
	int Connect();

	int Shutdown();
	int Close(); // 0 failed to close, 1 closed

	int Recieve();
	//Helpers
	void PrintError(FString msg);
};

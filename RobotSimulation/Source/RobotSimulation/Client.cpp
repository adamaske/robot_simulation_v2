// Fill out your copyright notice in the Description page of Project Settings.


#include "Client.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

#include "Common/TcpSocketBuilder.h"
// Sets default values
AClient::AClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AClient::BeginPlay()
{
	Super::BeginPlay();
	
	//PrintError("Starting Client");
	//
	//FIPv4Address address;
	//FIPv4Address::Parse("127.0.0.1", address);
	//FIPv4Endpoint(address, 8000);
	//
	//FSocket* socket = FTcpSocketBuilder(TEXT("ClientSock")).AsNonBlocking().AsReusable().Build();
	//PrintError("Socket built");
	//
	//addr->SetPort(8000);
	//socket->GetAddress(addr.Get());
	//socket->Connect(addr.Get());
	//FString msg = "Msg from Unreal Engine";
	//uint8 out_bytes[1024];
	//StringToBytes(msg, out_bytes, 1024);
	//int32 sent;
	//socket->Send(out_bytes, 1024, sent);
	//
	//PrintError("Sent");
}

// Called every frame
void AClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClient::PrintError(FString msg)
{
	//int error = WSAGetLastError();
	FString warn = "Client : " + msg + " : " + FString::FromInt(0);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, warn);
}

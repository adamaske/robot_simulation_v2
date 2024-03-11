// Fill out your copyright notice in the Description page of Project Settings.
#include "Server.h"
#include "Robot.h"

// Sets default values
AServer::AServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AServer::~AServer()
{
	if (b_Running) {
		Close();
	}
	
}

// Called when the game starts or when spawned
void AServer::BeginPlay()
{
	Super::BeginPlay();

	b_Running = Start();
	if (!b_Running) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Start failed")));
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Server : Started")));

}

// Called every frame
void AServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Wait for a client to connect
	if (!b_Connected) {
		int res = Connect();
	}	
	//Not connected to a client, then 
	//if (!b_Connected) {
	//	b_Connected  = Connect();
	//}
	//else {
	//	Listen();
	//}
}

int AServer::Start() {

	int iResult;
	
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	
	
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : WSAStartup FAILED")));
		//printf("WSAStartup failed with error: %d\n", iResult);
		return 0;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Server : WSAStartup")));

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : AddressInfo failed : %d"), iResult));
	
		return 0;
	}

	// Create a SOCKET for the server to listen for client connections.
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Socket Creation Error : %d"), error));
		freeaddrinfo(result);
	
		return 0;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Binding ListenSocket Failed : %d"), error));
		freeaddrinfo(result);
		
		return 0;
	}

	freeaddrinfo(result);
	return 1;
}

int AServer::Connect()
{
	int iResult;

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Listening Failed : %d"), error));
		return 0;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Accepting Client Failed : %d"), error));

		return 0;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	return 1;
}

int AServer::Listen()
{
	int iResult;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult < 0) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Recv failed : %d"), error));

		return Close();
	}
	if (iResult > 0) {
		printf("Bytes received: %d\n", iResult);

		// Echo the buffer back to the sender
		iSendResult = send(ClientSocket, recvbuf, iResult, 0);
		if (iSendResult == SOCKET_ERROR) {
			int error = WSAGetLastError();
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Send Echo Failed : %d"), error));

			return Close();
		}
		printf("Bytes sent: %d\n", iSendResult);
	}
	
	return 0;
}

int AServer::Shutdown()
{
	// shutdown the connection since we're done
	int iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		int error = WSAGetLastError();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Server : Shutdown Client Socket failed : %d"), error));

		return Close();
	}
	return 0;
}

int AServer::Close()
{
	if (!b_Running) {
		return 0;
	}

	// cleanup
	closesocket(ListenSocket);
	closesocket(ClientSocket);
	WSACleanup();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Server : WSACleanup")));
	return 1;
}

// Fill out your copyright notice in the Description page of Project Settings.
#include "Server.h"
#include "Robot.h"


#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27015"


// Sets default values
AServer::AServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AServer::~AServer()
{
	//Close();
}

// Called when the game starts or when spawned
void AServer::BeginPlay()
{
	Super::BeginPlay();

	int result = 0;

	result = InitWSA();
	if (result != 1) {
		Close();
		return;
	}

	result = StartListening();
	if (result != 1) {
		Close();
		return;
	}
}

// Called every frame
void AServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	int result = 0;
	if (connected) {
		result = Recieve();
		if (result != 1) {
			//Is something wrong ? 
		}
		return;
	}
	else {
		result = Connect();


	}


	
}

int AServer::InitWSA()
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (result == 0) {
		return 1;
	}
	else {
		PrintError("WSAStartup failed");
		return 0;
	}
}

int AServer::StartListening() {

	int result;
	
	listen_socket = INVALID_SOCKET;
	client_socket = INVALID_SOCKET;
	
	struct addrinfo* addr = NULL;
	struct addrinfo hints; //specification 
	

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addr);
	if (result != 0) {
		PrintError("Resolve AddressInfo Failed");
		return 0;
	}

	// Create a SOCKET for the server to listen for client connections.
	listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (listen_socket == INVALID_SOCKET) {
		PrintError("Listen_Socket failed inti");
		freeaddrinfo(addr);
		return 0;
	}

	// Setup the TCP listening socket
	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);
	if (result == SOCKET_ERROR) {
		PrintError("Listen_Socket failed binding");
		freeaddrinfo(addr);		
		return 0;
	}

	freeaddrinfo(addr);

	//Set socket in listening mode
	result = listen(listen_socket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		PrintError("Listen_Socket set to listening failed");
		return 0;
	}
	return 1;
}

int AServer::Listen()
{

	return 0;
}

int AServer::Connect()
{
	int result = 0;

	// Accept a client socket
	client_socket = accept(listen_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET) {
		PrintError("Client accepting failed");
		return 0;
	}

	// No longer need server socket
	closesocket(listen_socket);
	return 1;
}


int AServer::Shutdown()
{
	// shutdown the connection since we're done
	int result = shutdown(client_socket, SD_SEND);
	if (result == SOCKET_ERROR) {
		PrintError("Client_Socket shutdown failed");
		return 0;
	}
	return 1;
}

int AServer::Close()
{
	// cleanup
	closesocket(listen_socket);
	closesocket(client_socket);

	WSACleanup();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Server : WSACleanup")));
	return 1;
}

int AServer::Recieve()
{
	int result = 0;

	int send_result = 0;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//TODO : NON BLOCKING VERSION
	result = recv(client_socket, recvbuf, recvbuflen, 0);
	if (result > 0) {
		printf("Bytes received: %d\n", result);

		// Echo the buffer back to the sender
		send_result = send(client_socket, recvbuf, result, 0);
		if (send_result == SOCKET_ERROR) {
			PrintError("Echo send failed");
			return 0;
		}
	}
	else if (result == 0){
		PrintError("Connection closing");
		return 0;
	}
	else {
		PrintError("Recieve failed");
		return 0;
	}

	return 1;
}

void AServer::PrintError(FString msg)
{
	int error = WSAGetLastError();
	FString warn = "Sender : " + msg + " : " + FString::FromInt(error);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, warn);
}

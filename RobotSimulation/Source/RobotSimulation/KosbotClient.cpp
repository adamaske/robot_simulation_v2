// Fill out your copyright notice in the Description page of Project Settings.
#include "KosbotClient.h"
#include "Robot.h"

void AKosbotClient::Parse_JSON_From_TCP(TArray<uint8> bytes)
{
	FString msg = Message_ReadString(bytes, bytes.Num());

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, msg);
	//Use string to deserialze the json
	TSharedPtr<FJsonObject> json;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(msg), json)) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("TCPClient : Failed JSON Deserialization")));
		return;
	}

	if (!json.IsValid()) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("TCPClient : Invalid JSON Parsing")));
		return;
	}



	if (!m_Robot) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("TCPClient : No Robot")));
		return;
	}

}

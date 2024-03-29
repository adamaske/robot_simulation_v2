// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Robot.generated.h"

//Changes the FK and IK functions
UENUM()
enum ERobotType{ DOF6, DOF3 };

USTRUCT(BlueprintType)
struct FRobotSpecification {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	FName m_Name;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ERobotType> m_Type;
};

UENUM()
enum ERotationAxis{ X, Y, Z};

UENUM()
enum ELinkType{ REVOLUTE, PRISMATIC  };
USTRUCT(BlueprintType)
struct FLink {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	FString m_Name = "Link 0";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ELinkType> m_Type = REVOLUTE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ERotationAxis> m_RotationAxis = X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float X_Translation = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float Y_Translation = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float Z_Translation = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float m_Theta = 0;
};

USTRUCT(BlueprintType)
struct FDHParam {
	GENERATED_BODY()
public:
	//Rotaiton along X-Axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_Alpha = 0;
	//Translation along X-axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_R = 0;

	//Rotation along Z-Axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_Theta = 0;
	//Translation along Z-axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_D = 0;
};

USTRUCT(BlueprintType)
struct FLinkVisual {
	GENERATED_BODY()
public:
	UStaticMeshComponent* x = nullptr;
	UStaticMeshComponent* y = nullptr;
	UStaticMeshComponent* z = nullptr;

};
UCLASS()
class ROBOTSIMULATION_API ARobot : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TArray<FDHParam> m_DHParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TArray<FLink> m_Links;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ERobotType> m_Type = DOF6;
public:	
	// Sets default values for this actor's properties
	ARobot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RecieveJSON(FJsonObject json);

	UFUNCTION(BlueprintCallable)
	FVector GetNumericalFKResult();

	UFUNCTION(BlueprintCallable)
	FVector GetDHFKResult();

	void Pose();
	
	FLink* EndEffectorLink();

	UFUNCTION(BlueprintCallable)
	void SetLinkAngles(TArray<float> angles);
	UFUNCTION(BlueprintCallable)
	void SetLinkAngle(int link_index, float angle);

#pragma region MATH
	FMatrix ForwardKinematics();
	FMatrix DH_ForwardKinematics();
	//void InverseKinematics();

	FMatrix PositionMatrix(FVector position);
	FMatrix RotationMatrix(ERotationAxis axis, float theta);
	FMatrix DH_TranslationMatrix(FDHParam dh);

	FVector RobotToWorldSpace(FVector input);
	FVector WorldToRobotSpace(FVector input);
#pragma endregion

#pragma region Visuals
private:
	//Base mesh for the robot arms to be scaled and changed later
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TSoftObjectPtr<UStaticMesh> m_LinkBaseMesh;
	//Array of the mesh components
	TArray<USceneComponent*> m_LinkParents;
	TArray<FLinkVisual> m_LinkVisuals;
	float m_LinkVisualThickness = 10;
public:
	void SetupVisual();
	void UpdateVisual();
	void DestroyVisual();
	void CreateVisualChain(FLink link, int idx);
	UFUNCTION(BlueprintCallable)
	FVector GetActualEndEffectorLocation();
#pragma endregion

};

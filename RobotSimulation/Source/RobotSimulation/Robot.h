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
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	FName m_Name = "Link 0";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ELinkType> m_Type = REVOLUTE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ERotationAxis> m_RotationAxis = X;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_Theta = 0;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_Width = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_Height = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float m_Depth = 10;
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
	
	FLink EndEffectorLink();
#pragma region MATH
	FMatrix ForwardKinematics();
	FMatrix DH_ForwardKinematics();
	//void InverseKinematics();

	FMatrix PositionMatrix(FVector position);
	FMatrix RotationMatrix(ERotationAxis axis, float theta);
	FMatrix DH_TranslationMatrix(FDHParam dh);

#pragma endregion

#pragma region Visuals
private:
	//Base mesh for the robot arms to be scaled and changed later
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TSoftObjectPtr<UStaticMesh> m_LinkBaseMesh;
	//Array of the mesh components
	TArray<USceneComponent*> m_LinkParents;
	TArray<UStaticMeshComponent*> m_LinkMeshes;

public:
	void SetupVisual();
	void UpdateVisual();
	void DestroyVisual();
	void CreateVisualChain(FLink link, int idx);
#pragma endregion

};

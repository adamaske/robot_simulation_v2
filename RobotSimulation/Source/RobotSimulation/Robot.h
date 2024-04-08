// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Robot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRobotPosed);

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
enum ERotationAxis{ X = 0, Y = 1, Z = 2};


USTRUCT(BlueprintType)
struct FDHParam {
	GENERATED_BODY()
public:
	//Rotation along Z-Axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float Theta = 0;
	//Rotaiton along X-Axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float Alpha = 0;

	//Translation along Z-axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float D = 0;
	//Translation along X-axis
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = True))
	float R = 0;

};

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
	FVector m_Translations = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	FVector m_RotationOffsets = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float m_Theta = 0;

};


USTRUCT(BlueprintType)
struct FTestResults {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	int test_amount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float average_numerical_error = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	float average_dh_error = 0;
};

UCLASS()
class ROBOTSIMULATION_API ARobot : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TEnumAsByte<ERobotType> m_Type = DOF6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TArray<FLink> m_Links;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TArray<FDHParam> m_DHParams;
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
	//==== POSING =====
	void Pose();

	UPROPERTY(BlueprintAssignable)
	FOnRobotPosed m_OnPosed;

	UFUNCTION(BlueprintCallable)
	void SetLinkAngles(TArray<float> angles);
	UFUNCTION(BlueprintCallable)
	void SetLinkAngle(int link_index, float angle);
	UFUNCTION(BlueprintCallable)
	void AddLinkAngle(int link_index, float delta);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	USceneComponent* m_EndEffector;
	float m_LinkVisualThickness = 10;
public:
	void SetupVisual();
	void UpdateVisual();
	void CreateVisualChain(FLink link, int idx);
	UStaticMeshComponent* CreateLinkMeshAndAttachToParent(USceneComponent* parent);

#pragma endregion

//=====End Effector=====
	FLink* EndEffectorLink();
	void UpdateEndEffector();

	UFUNCTION(BlueprintCallable)
	FVector GetActualEndEffectorLocation();

//=====Automated Testing====
	UFUNCTION(BlueprintCallable)
	FTestResults RunTests();

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Robot.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ARobot::ARobot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();
	
	SetupVisual();
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARobot::RecieveJSON(FJsonObject json)
{
	double version = json.GetNumberField("Kosbot");
	FString command = json.GetStringField("Command");

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("KOSBOT : %f"), version));
	if (command == "pose") {
		Pose();
	}
	else if (command == "update") {

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Robot : UPDATE")));
	}
}
FVector ARobot::GetNumericalFKResult()
{
	return ForwardKinematics().GetColumn(3);
}
FVector ARobot::GetDHFKResult()
{
	return  DH_ForwardKinematics().GetColumn(3);
}

void ARobot::Pose()
{
	FMatrix norm_fk = ForwardKinematics();
	FMatrix dh_fk = DH_ForwardKinematics();


	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Robot : POSE")));
}

FLink ARobot::EndEffectorLink()
{
	return m_Links[m_Links.Num()-1];
}

FMatrix ARobot::ForwardKinematics()
{
	FMatrix t = FMatrix::Identity;
	int idx = 0;
	for (auto link : m_Links) {
		FMatrix rot = RotationMatrix(link.m_RotationAxis, link.m_Theta);
		FMatrix pos = PositionMatrix(FVector(link.m_Depth, link.m_Width, link.m_Height));

		FMatrix l = rot * pos;
		t *= l;
	}
	return t;
}

FMatrix ARobot::DH_ForwardKinematics()
{
	FMatrix dh_t = FMatrix::Identity;
	int idx = 0;
	for (auto dh : m_DHParameters) {
		FMatrix dh_mat = DH_TranslationMatrix(dh);
		dh_t *= dh_mat;
	}
	return dh_t;
}

FMatrix ARobot::PositionMatrix(FVector position)
{
	auto mat = FMatrix(
		FPlane(1, 0, 0, position.X),
		FPlane(0, 1, 0, position.Y),
		FPlane(0, 0, 1, position.Z),
		FPlane(0, 0, 0, 1)
	);

	return mat;
}

FMatrix ARobot::RotationMatrix(ERotationAxis axis, float theta)
{
	FMatrix m;
	auto ct = FMath::Cos(FMath::DegreesToRadians(theta));
	auto st = FMath::Sin(FMath::DegreesToRadians(theta));

	switch (axis) {
	case(X):
		m = FMatrix(
			FPlane(1, 0, 0, 0),
			FPlane(0, ct, -st, 0),
			FPlane(0, st, ct, 0),
			FPlane(0, 0, 0, 1)
		);
		break;
	case(Y):
		m = FMatrix(
			FPlane(ct, 0, st, 0),
			FPlane(0, 1, 0, 0),
			FPlane(-st, 0, ct, 0),
			FPlane(0, 0, 0, 1)
		);
		break;
	case(Z):
		m = FMatrix(
			FPlane(ct, -st, 0, 0),
			FPlane(st, ct, 0, 0),
			FPlane(0, 0, 1, 0),
			FPlane(0, 0, 0, 1)
		);
		break;
	default:
		m = FMatrix(
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 1, 0),
			FPlane(0, 0, 0, 1)
		);
		break;
	}

	return m;
}

FMatrix ARobot::DH_TranslationMatrix(FDHParam dh)
{
	float t = FMath::DegreesToRadians(dh.m_Theta);
	float alpha = FMath::DegreesToRadians(dh.m_Alpha);
	float r = dh.m_R;
	float d = dh.m_D;

	float ct = cos(t);
	float st = sin(t);

	FMatrix mat = FMatrix(
		FPlane(ct, -st * cos(alpha), st * sin(alpha), r * ct),
		FPlane(st, ct * cos(alpha), -ct * sin(alpha), r * st),
		FPlane(0, sin(alpha), cos(alpha), d),
		FPlane(0, 0, 0, 1)

	);

	return mat;
}

void ARobot::SetupVisual()
{
	if (!m_LinkBaseMesh.Get()) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Robot : NO LINK BASE MESH")));
		return;
	}

	int num = m_Links.Num();
	m_LinkParents.SetNum(num);
	m_LinkMeshes.SetNum(num);

	DestroyVisual();

	int idx = 0;
	for (auto& link : m_Links) {

		CreateVisualChain(link, idx);
		idx++;
	}

	UpdateVisual();
}

void ARobot::UpdateVisual()
{
	int idx = 0;
	for (auto& link : m_Links) {

		USceneComponent* parent = m_LinkParents[idx];
		UStaticMeshComponent* mesh = m_LinkMeshes[idx];

		mesh->SetRelativeScale3D(FVector(link.m_Depth, link.m_Width, link.m_Height) / 100.f);

		mesh->SetRelativeLocation(FVector(0, 0, link.m_Height / 2.f));
		//Rotate according to theta
		auto rot = parent->GetRelativeRotation();
		rot.Roll = link.m_RotationAxis == X ? link.m_Theta : 0;
		rot.Pitch = link.m_RotationAxis == Y ? link.m_Theta : 0;
		rot.Yaw = link.m_RotationAxis == Z ? link.m_Theta : 0;
		parent->SetRelativeRotation(rot);

		if (idx != 0) {
			//
			parent->SetRelativeLocation(FVector(0, 0, m_Links[idx - 1].m_Height));
		}

		idx++;
	}
}

void ARobot::DestroyVisual()
{
	int idx = 0;
	for (auto link : m_Links) {
		if (m_LinkMeshes[idx]) {
			m_LinkMeshes[idx]->DestroyComponent();
			m_LinkMeshes[idx] = nullptr;
		}
		if (m_LinkParents[idx]) {
			m_LinkParents[idx]->DestroyComponent();
			m_LinkParents[idx] = nullptr;
		}
		idx++;
	}
}

void ARobot::CreateVisualChain(FLink link, int idx)
{
	UActorComponent* parent_comp = AddComponentByClass(USceneComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(parent_comp);

	USceneComponent* parent = Cast<USceneComponent>(parent_comp);
	m_LinkParents[idx] = parent;
	parent->AttachToComponent(idx == 0 ? RootComponent : m_LinkParents[idx - 1],
		FAttachmentTransformRules::SnapToTargetIncludingScale);

	UActorComponent* mesh_comp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(mesh_comp);
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(mesh_comp);

	m_LinkMeshes[idx] = mesh;
	mesh->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	mesh->SetStaticMesh(m_LinkBaseMesh.Get());
}
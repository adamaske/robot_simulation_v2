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
	return DH_ForwardKinematics().GetColumn(3);
}

void ARobot::Pose()
{
	UpdateEndEffector();
	UpdateVisual();
	m_OnPosed.Broadcast();
}

void ARobot::SetLinkAngles(TArray<float> angles)
{	int idx = 0;
	for (auto& link : m_Links) {
		link.m_Theta = angles[idx];
		m_DHParams[idx].Theta = angles[idx];
		idx++;
	}
	Pose();
}

void ARobot::SetLinkAngle(int link_index, float angle)
{
	m_Links[link_index].m_Theta = angle;
	m_DHParams[link_index].Theta = angle;
	Pose();
}

void ARobot::AddLinkAngle(int link_index, float delta)
{
	m_Links[link_index].m_Theta += delta;
	m_DHParams[link_index].Theta += delta;
	Pose();
}

FMatrix ARobot::ForwardKinematics()
{
	FMatrix t = FMatrix::Identity;
	int idx = 0;
	for (auto link : m_Links) {
		float theta = link.m_RotationAxis == Y ? -link.m_Theta : link.m_Theta; //Adjust for left hand system
		FMatrix rot = RotationMatrix(link.m_RotationAxis, theta);
		FMatrix pos = PositionMatrix(link.m_Translations);
		t = t * (rot * pos);
	}
	return t;
}

FMatrix ARobot::DH_ForwardKinematics()
{
	FMatrix dh_t = FMatrix::Identity;
	int idx = 0;
	for (auto dh : m_DHParams) {
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
	float t = FMath::DegreesToRadians(dh.Theta);
	float alpha = FMath::DegreesToRadians(dh.Alpha);
	float r = dh.R;
	float d = dh.D;
	float ct = cos(t);
	float st = sin(t);
	float ca = cos(alpha);
	float sa = sin(alpha);

	FMatrix mat = FMatrix(
		FPlane(ct, -st * ca, st * sa, r * ct),
		FPlane(st, ct * ca, -ct * sa, r * st),
		FPlane( 0, sa, ca, d),
		FPlane(0, 0, 0, 1)
	);

	return mat;
}

FVector ARobot::RobotToWorldSpace(FVector input)
{
	return GetActorLocation() + input;
}

FVector ARobot::WorldToRobotSpace(FVector input)
{
	return input - GetActorLocation();
}

void ARobot::SetupVisual()
{
	if (!m_LinkBaseMesh.Get()) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Robot : NO LINK BASE MESH")));
		return;
	}

	int num = m_Links.Num();
	m_LinkParents.SetNum(num);

	int diff = num - m_DHParams.Num();
	for (int i = 0; i < diff; i++) {
		m_DHParams.Add(FDHParam());
	}
	

	int idx = 0;
	for (auto& link : m_Links) {

		CreateVisualChain(link, idx);
		idx++;
	}

	UpdateEndEffector();

	UpdateVisual();
}

void ARobot::UpdateVisual()
{
	FMatrix T = FMatrix::Identity;
	for (int i = 0; i < m_Links.Num(); i++) {
		
		auto const link = m_Links[i];
		auto const parent = m_LinkParents[i];
		auto const axis = link.m_RotationAxis;
		auto const theta = link.m_Theta;
		auto const prev_loc = T.GetColumn(3);

		FRotator offset_rot = FRotator::ZeroRotator;
		if (bool use_offset = false) {
			auto offsets = link.m_RotationOffsets;
			FMatrix of = FMatrix::Identity;
			for (int j = 0; j < 3; j++) {
				if (offsets[j] != 0) {
					of *= RotationMatrix(ERotationAxis(i), offsets[j]);
				}
			}
			offset_rot = FRotationMatrix::MakeFromXZ(of.GetColumn(0), of.GetColumn(2)).Rotator();
		}

		//Rotate theta and translate the length of it
		auto rot = RotationMatrix(axis, axis == Y ? -theta : theta);
		auto pos = PositionMatrix(link.m_Translations); //Position of this link is the previous translation
		auto t = rot * pos;
		T = T * t;

		FVector forward = T.GetColumn(0);
		FVector right = T.GetColumn(1);
		FVector up = T.GetColumn(2);
		FVector new_loc = T.GetColumn(3);
		FRotator new_rot = FRotationMatrix::MakeFromXY(forward, right).Rotator();

		parent->SetRelativeRotation(new_rot);
		parent->SetRelativeLocation(prev_loc);

		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("=======================")));
		//if(use_offset){ GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("Offset : %s"), *offset_rot.ToString())); }
		//
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("Up : %s"), *up.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("Right : %s"), *right.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("Forward : %s"), *forward.ToString()));
		//
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("Rotation : %s"), *new_rot.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("Location : %s"), *new_loc.ToString()));
		//
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Cyan, FString::Printf(TEXT("===== LINK %d ======"), i));
	}
}

void ARobot::CreateVisualChain(FLink link, int idx)
{
	UActorComponent* parent_comp = AddComponentByClass(USceneComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(parent_comp);

	USceneComponent* parent = Cast<USceneComponent>(parent_comp);
	m_LinkParents[idx] = parent;
	parent->AttachToComponent(idx == 0 ? RootComponent : RootComponent,// m_LinkParents[idx - 1],
		FAttachmentTransformRules::SnapToTargetIncludingScale);

	//Translation is 0 then dont need a mesh
	bool x = link.m_Translations.X != 0;
	bool y = link.m_Translations.Y != 0;
	bool z = link.m_Translations.Z != 0;
	
	if (x) {
		auto mesh = CreateLinkMeshAndAttachToParent(parent);
		mesh->SetRelativeScale3D(FVector(m_LinkVisualThickness, m_LinkVisualThickness, link.m_Translations.X) / 100.f);
		mesh->SetRelativeLocation(FVector(0, 0, 0));

		//X points in -90 y
		auto rot  = mesh->GetRelativeRotation();
		rot.Pitch = -90;
		mesh->SetRelativeRotation(rot);

	}
	if (y) {
		auto mesh = CreateLinkMeshAndAttachToParent(parent);
		mesh->SetRelativeScale3D(FVector(m_LinkVisualThickness, m_LinkVisualThickness, link.m_Translations.Y) / 100.f);
		mesh->SetRelativeLocation(FVector(x ? link.m_Translations.X : 0, 0, 0));

		//Y points in 90 x
		auto rot = mesh->GetRelativeRotation();
		rot.Roll = 90;
		mesh->SetRelativeRotation(rot);
	}

	if (z) {
		auto mesh = CreateLinkMeshAndAttachToParent(parent);
		mesh->SetRelativeScale3D(FVector(m_LinkVisualThickness, m_LinkVisualThickness, link.m_Translations.Z) / 100.f);
		mesh->SetRelativeLocation(FVector(x ? link.m_Translations.X : 0, y ? link.m_Translations.Y : 0, 0));
	}
}

UStaticMeshComponent* ARobot::CreateLinkMeshAndAttachToParent(USceneComponent* parent)
{
	auto comp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(comp);

	auto mesh = Cast<UStaticMeshComponent>(comp);
	mesh->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	mesh->SetStaticMesh(m_LinkBaseMesh.Get());
	return mesh;
}

FLink* ARobot::EndEffectorLink()//Not a reference
{
	return &m_Links[m_Links.Num() - 1];
}

void ARobot::UpdateEndEffector()
{
	if (!m_EndEffector) {
		auto comp = AddComponentByClass(USceneComponent::StaticClass(), true, FTransform::Identity, false);
		AddInstanceComponent(comp);
		m_EndEffector = Cast<USceneComponent>(comp);
		m_EndEffector->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}

	auto fk = ForwardKinematics();
	auto rot = FRotationMatrix::MakeFromXY(fk.GetColumn(0), fk.GetColumn(2)).Rotator();
	auto loc = fk.GetColumn(3);

	m_EndEffector->SetRelativeRotation(rot);
	m_EndEffector->SetRelativeLocation(loc);
}

FVector ARobot::GetActualEndEffectorLocation()
{
	UpdateEndEffector();
	return WorldToRobotSpace(m_EndEffector->GetComponentLocation());
}

FTestResults ARobot::RunTests()
{
	int num = 100;

	TArray<float> thetas;
	thetas.SetNum(m_Links.Num());

	float total_numerical_error = 0;
	float total_dh_error = 0;

	for (int i = 0; i < num; i++) {
		for (int j = 0; j < m_Links.Num(); j++) {
			thetas[j] = FMath::RandRange(-180, 180);
		}
		SetLinkAngles(thetas);

		auto actual = GetActualEndEffectorLocation();

		auto numerical = ForwardKinematics().GetColumn(3);
		auto num_diff = numerical - actual;
		float num_error = num_diff.Length();
		total_numerical_error += num_error;

		auto dh = DH_ForwardKinematics().GetColumn(3);
		auto dh_diff = dh - actual;
		float dh_error = dh_diff.Length();
		total_dh_error += dh_error;
	}

	FTestResults result;
	result.test_amount = num;
	result.average_numerical_error = total_numerical_error / float(num);
	result.average_dh_error = total_dh_error / float(num);
	return result;
}

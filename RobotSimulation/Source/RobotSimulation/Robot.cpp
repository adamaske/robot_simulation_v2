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

FLink* ARobot::EndEffectorLink()//Not a reference
{
	return &m_Links[m_Links.Num()-1];
}

void ARobot::SetLinkAngles(TArray<float> angles)
{	int idx = 0;
	for (auto& link : m_Links) {
		link.m_Theta = angles[idx];
		idx++;
	}
	UpdateVisual();
}

void ARobot::SetLinkAngle(int link_index, float angle)
{
	m_Links[link_index].m_Theta = angle;
	UpdateVisual();
}

FMatrix ARobot::ForwardKinematics()
{
	FMatrix t = FMatrix::Identity;
	int idx = 0;
	for (auto link : m_Links) {
		FMatrix rot = RotationMatrix(link.m_RotationAxis, link.m_Theta);
		FMatrix pos = PositionMatrix(FVector(link.X_Translation, link.Y_Translation, link.Z_Translation));
		FMatrix l = rot * pos;
		
		t = t * rot * pos;
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

	DestroyVisual();

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
	int idx = 0;
	for (auto& link : m_Links) {

		USceneComponent* parent = m_LinkParents[idx];

		//Rotate according to theta
		auto rot = parent->GetRelativeRotation();
		rot.Roll = link.m_RotationAxis == X ? -link.m_Theta : 0;
		rot.Pitch = link.m_RotationAxis == Y ? -link.m_Theta : 0;
		rot.Yaw = link.m_RotationAxis == Z ? link.m_Theta : 0;
		parent->SetRelativeRotation(rot);

		if (idx != 0) {
			//
			parent->SetRelativeLocation(FVector(m_Links[idx - 1].X_Translation, 
												m_Links[idx - 1].Y_Translation, 
												m_Links[idx - 1].Z_Translation));
		}
		else {
			parent->SetRelativeLocation(FVector::ZeroVector);
		}

		idx++;
	}
}

void ARobot::DestroyVisual()
{
	return;
	for (auto parent : m_LinkParents) {
		parent->DestroyComponent();
	}
	m_LinkParents.Empty();
}

void ARobot::CreateVisualChain(FLink link, int idx)
{
	UActorComponent* parent_comp = AddComponentByClass(USceneComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(parent_comp);

	USceneComponent* parent = Cast<USceneComponent>(parent_comp);
	m_LinkParents[idx] = parent;
	parent->AttachToComponent(idx == 0 ? RootComponent : m_LinkParents[idx - 1],
		FAttachmentTransformRules::SnapToTargetIncludingScale);

	//Translation is 0 then dont need a mesh
	bool x = link.X_Translation != 0;
	bool y = link.Y_Translation != 0;
	bool z = link.Z_Translation != 0;
	
	if (x) {
		auto mesh = CreateLinkMeshAndAttachToParent(parent);
		mesh->SetRelativeScale3D(FVector(m_LinkVisualThickness, m_LinkVisualThickness, link.X_Translation) / 100.f);
		mesh->SetRelativeLocation(FVector(0, 0, 0));

		//X points in -90 y
		auto rot  = mesh->GetRelativeRotation();
		rot.Pitch = -90;
		mesh->SetRelativeRotation(rot);

	}
	if (y) {
		auto mesh = CreateLinkMeshAndAttachToParent(parent);
		mesh->SetRelativeScale3D(FVector(m_LinkVisualThickness, m_LinkVisualThickness, link.Y_Translation) / 100.f);
		mesh->SetRelativeLocation(FVector(x ? link.X_Translation : 0, 0, 0));

		//Y points in 90 x
		auto rot = mesh->GetRelativeRotation();
		rot.Roll = 90;
		mesh->SetRelativeRotation(rot);
	}

	if (z) {
		auto mesh = CreateLinkMeshAndAttachToParent(parent);
		mesh->SetRelativeScale3D(FVector(m_LinkVisualThickness, m_LinkVisualThickness, link.Z_Translation) / 100.f);
		mesh->SetRelativeLocation(FVector(x ? link.X_Translation : 0, y ? link.Y_Translation : 0, 0));
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

void ARobot::UpdateEndEffector()
{
	if (!m_EndEffector) {
		UActorComponent* ee = AddComponentByClass(USceneComponent::StaticClass(), true, FTransform::Identity, false);
		AddInstanceComponent(ee);
		m_EndEffector = Cast<USceneComponent>(ee);
	}
	auto parent = m_LinkParents[m_LinkParents.Num() - 1];
	auto link = m_Links[m_Links.Num() - 1];

	m_EndEffector->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	m_EndEffector->SetRelativeLocation(FVector(	link.X_Translation,
												link.Y_Translation,
												link.Z_Translation));
}

FVector ARobot::GetActualEndEffectorLocation()
{
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

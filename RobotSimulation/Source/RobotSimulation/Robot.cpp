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

void ARobot::Pose()
{
}

void ARobot::SetupVisual()
{
	if (!m_LinkBaseMesh) {
	}

	int idx = 0;
	for (auto& link : m_Links) {
		CreateVisualChain(link, idx);
		idx++;
	}
	
	UpdateVisual();
}

void ARobot::UpdateVisual()
{
	//Position and rotate all the link parents correctly


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

void ARobot::CreateVisualChain(FLink link, int idx)
{
	UActorComponent* parent_comp = AddComponentByClass(USceneComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(parent_comp);

	USceneComponent* parent = Cast<USceneComponent>(parent_comp);
	m_LinkParents.Add(parent);
	parent->AttachToComponent(idx == 0 ? RootComponent : m_LinkParents[idx - 1], 
								FAttachmentTransformRules::SnapToTargetIncludingScale);

	UActorComponent* mesh_comp = AddComponentByClass(UStaticMeshComponent::StaticClass(), true, FTransform::Identity, false);
	AddInstanceComponent(mesh_comp);
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(mesh_comp);

	m_LinkMeshes.Add(mesh);
	mesh->AttachToComponent(parent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	mesh->SetStaticMesh(m_LinkBaseMesh.Get());


}


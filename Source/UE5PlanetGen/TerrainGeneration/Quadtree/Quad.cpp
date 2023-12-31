﻿#include "Quad.h"

AQuad::AQuad()
{
	Quad = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Quad"));
	RootComponent = Quad;
}

void AQuad::PostLoad()
{
	Super::PostLoad();
}

void AQuad::BeginDestroy()
{
	Quad->RemoveFromRoot();
	Super::BeginDestroy();
}

void AQuad::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	UE_LOG(LogTemp, Warning, TEXT("Caca"));
}

void AQuad::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UE_LOG(LogTemp, Warning, TEXT("Caca2"));

}

void AQuad::PostActorCreated()
{
	Super::PostActorCreated();
	UE_LOG(LogTemp, Warning, TEXT("Quad Created"));
}
bool AQuad::IsPointInBounds(const FVector& PointLocation) const
{
	auto Max {Quad->Bounds.GetBox().Min};
	auto Min {Quad->Bounds.GetBox().Max};
	//UE_LOG(LogTemp, Warning, TEXT("\nQUAD NAME:%s\n\tBounds\n\tXMin: %f \n\tXMax: %f\n\t\n\tYMin: %f \n\tYMax: %f\n\tPointLocation: %f, %f, %f"),*this->GetName() ,Min.X, Max.X, Min.Y, Max.Y, PointLocation.X, PointLocation.Y, PointLocation.Z);
	return Quad->Bounds.GetBox().IsInsideXY(PointLocation);
}

void AQuad::SetActiveQuad(bool Active)
{
	if (MaterialInterface != nullptr)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialInterface, Quad);
	}
	if (DynamicMaterial)
	{
		if (Active)
		{
			DynamicMaterial->SetVectorParameterValue("Color", FVector(255,0,0));
			//MaterialInterface->SetVectorParameterValue("Base Color", FLinearColor::Red);
			Quad->SetMaterial(0, DynamicMaterial);
		}
		else
		{
			DynamicMaterial->SetVectorParameterValue("Color", FVector(0,255,0));
			//MaterialInterface->SetVectorParameterValue("Base Color", FLinearColor::Green);
			Quad->SetMaterial(0, DynamicMaterial);
		}
	}
	
}

void AQuad::BeginPlay()
{
	Super::BeginPlay();
}

void AQuad::CreateQuad(float InScale) 
{
	TArray<FVector> Vertices =
	{
		{-InScale, -InScale, 0.0f},
		{InScale, -InScale, 0.0f},
		{InScale, InScale, 0.0f}, 
		{-InScale, InScale, 0.0f}
	};
	Scale = InScale;
	Quad->CreateMeshSection_LinearColor(
		0,
		Vertices,
		Indices,
		Normals,
		Uv,
		VertexColors,
		Tangent,
		true
	);
}

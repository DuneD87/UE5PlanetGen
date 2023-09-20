#include "Quad.h"

#include "UE5PlanetGen/TerrainGeneration/Util/FastNoiseLite.h"

AQuad::AQuad()
{
	Quad = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Quad"));
	RootComponent = Quad;
	bListedInSceneOutliner=false;
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

void AQuad::PostActorCreated()
{
	Super::PostActorCreated();
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

void AQuad::CreateQuad(float InScale,  const TArray<FVector>& Vertices, const TArray<FVector2d>& Uv, const TArray<FVector>& Normals, const TArray<int>& Indices, const TArray<FLinearColor>& VertexsColor)
{
	Quad->CreateMeshSection_LinearColor(
		0,
		Vertices,
		Indices,
		Normals,
		Uv,
		VertexsColor,
TArray<FProcMeshTangent>(),
		true
	);
}


#include "Quad.h"

AQuad::AQuad()
{
	Quad = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Quad"));
	RootComponent = Quad;
}

void AQuad::PostLoad()
{
	Super::PostLoad();
	CreateQuad();
}

void AQuad::BeginDestroy()
{
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
	CreateQuad();
}

void AQuad::SetActiveQuad(bool Active)
{
	if (Active)
	{
		if (MaterialInterface.Num() > 0)
		{
			Quad->SetMaterial(0,MaterialInterface[0]);
		}
	}
	else
	{
		if (MaterialInterface.Num() > 1)
		{
			Quad->SetMaterial(0,MaterialInterface[1]);
		}
	}
		
}

void AQuad::BeginPlay()
{
	Super::BeginPlay();
}

void AQuad::CreateQuad() const
{

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

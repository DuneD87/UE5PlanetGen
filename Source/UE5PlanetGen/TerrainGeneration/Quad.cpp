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
}

void AQuad::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AQuad::PostActorCreated()
{
	Super::PostActorCreated();
	CreateQuad();
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

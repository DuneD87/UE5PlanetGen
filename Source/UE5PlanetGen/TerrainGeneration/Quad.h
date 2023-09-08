#pragma once
#include "ProceduralMeshComponent.h"
#include "Quad.generated.h"

UCLASS()
class AQuad : public AActor
{
	GENERATED_BODY()
public:
	AQuad();
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostActorCreated() override;

protected:
	virtual void BeginPlay() override;
private:
	TArray<FVector2d> Uv =
	{
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f}
	};
	TArray<FProcMeshTangent> Tangent
	{
		{1.0f,  1.0f, 0.0f},
		{1.0f,  1.0f, 1.0f},
		{1.0f,  1.0f, 1.0f},
		{1.0f,  1.0f, 0.0f}
	};

	TArray<FVector> Vertices =
	{
		{-100.0f, -100.0f, 0.0f},
		{100.0f, -100.0f, 0.0f},
		{100.0f, 100.0f, 0.0f}, 
		{-100.0f, 100.0f, 0.0f}
	};
	
	TArray<int> Indices =
	{
		0, 1, 2, 
		0, 2, 3 
	};
	
	TArray<FVector> Normals
	{
		{0.0f, 0.0f, 1.0f}, 
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}
	};
	TArray<FLinearColor> VertexColors
	{
		{0.75, 0.75, 0.75, 1.0},
		{0.75, 0.75, 0.75, 1.0},
		{0.75, 0.75, 0.75, 1.0},
		{0.75, 0.75, 0.75, 1.0},
		{0.75, 0.75, 0.75, 1.0},
		{0.75, 0.75, 0.75, 1.0}
	};
	void CreateQuad() const;
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> Quad;
};

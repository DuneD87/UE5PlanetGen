#pragma once
#include "ProceduralMeshComponent.h"
#include "CoreMinimal.h"
#include "Quad.generated.h"
class FastNoiseLite;
UCLASS()
class AQuad : public AActor
{
	GENERATED_BODY()
public:
	AQuad();
	//AActor interface implementation
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void PostActorCreated() override;
	virtual bool ShouldTickIfViewportsOnly() const override {return true;};
	//End AActor interface
	bool IsPointInBounds(const FVector& PointLocation) const;
	FVector GetQuadLocation() const {return Quad->GetComponentLocation();};
	void SetActiveQuad(bool Active);
	float GetScale() const {return Scale;};
	void CreateQuad(float InScale, const TArray<FVector>& Vertices, const TArray<FVector2d>& Uv, const TArray<FVector>& Normals, const TArray<int>& Indices, const TArray<FLinearColor>& VertexsColor);
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* MaterialInterface;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> VertexColors
	{
			{0.75, 0.75, 0.75, 1.0},
			{0.75, 0.75, 0.75, 1.0},
			{0.75, 0.75, 0.75, 1.0},
			{0.75, 0.75, 0.75, 1.0},
			{0.75, 0.75, 0.75, 1.0},
			{0.75, 0.75, 0.75, 1.0}
	};
private:
	UPROPERTY()
	float Scale;

	UPROPERTY()
	UProceduralMeshComponent* Quad;
};

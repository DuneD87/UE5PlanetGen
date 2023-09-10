﻿#pragma once
#include "ProceduralMeshComponent.h"
#include "CoreMinimal.h"
#include "Quad.generated.h"

UCLASS()
class AQuad : public AActor
{
	GENERATED_BODY()
public:
	AQuad();
	//AActor interface implementation
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostActorCreated() override;
	virtual bool ShouldTickIfViewportsOnly() const override {return true;};
	//End AActor interface
	void SetActiveQuad(bool Active);
	float GetScale() const {return Scale;};
protected:
	UPROPERTY(EditAnywhere)
	float Scale {100.f};
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	TArray<FVector2D> Uv =
	{
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};
	UPROPERTY(EditAnywhere)
	TArray<FProcMeshTangent> Tangent
	{
			{1.0f,  1.0f, 0.0f},
			{1.0f,  1.0f, 1.0f},
			{1.0f,  1.0f, 1.0f},
			{1.0f,  1.0f, 0.0f}
	};
	UPROPERTY(EditAnywhere)
	TArray<FVector> Vertices =
	{
		{-Scale, -Scale, 0.0f},
		{Scale, -Scale, 0.0f},
		{Scale, Scale, 0.0f}, 
		{-Scale, Scale, 0.0f}
	};
	UPROPERTY(EditAnywhere)
	TArray<int> Indices =
	{
		0, 3, 2,
		2, 1, 0, 
	};
	UPROPERTY(EditAnywhere)
	TArray<FVector> Normals
	{
			{0.0f, 0.0f, 1.0f}, 
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f}
	};
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
	
	
	void CreateQuad() const;
	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> Quad;
};

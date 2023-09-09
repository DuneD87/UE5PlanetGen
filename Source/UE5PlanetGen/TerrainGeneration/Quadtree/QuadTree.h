#pragma once
#include "QuadTree.generated.h"
class AQuad;
USTRUCT()
struct FQuadTreeNode
{
	GENERATED_BODY()
	TArray<TSharedPtr<FQuadTreeNode>> Nodes;
	TObjectPtr<AQuad> Value;
};

UCLASS()
class AQuadTree : public AActor
{
	GENERATED_BODY()
public:
	AQuadTree();

	//AActor interface implementation
	virtual void Destroyed() override;
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostActorCreated() override;
	virtual bool ShouldTickIfViewportsOnly() const override {return true;};
	//End AActor interface
protected:
	virtual void BeginPlay() override;
private:
	
	enum QuadNames
	{
		Top = 0,
		Left = 1,
		Right = 2,
		Bottom = 3
	};
	FVector CameraPosition;
	void BuildQuadTree();
	TSharedPtr<FQuadTreeNode> QuadRootNode;

};

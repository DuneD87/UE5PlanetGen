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
	
	enum Quadrants
	{
		TopLeft = 0,
		TopRight = 1,
		BottomLeft = 2,
		BottomRight = 3
	};
	TMap<int, float> LODLevels
	{
		{1, 10000},
		{2, 5000},
		{3, 2500},
		{4, 1250},
		{5, 625}
	};
	FVector CameraPosition;
	void BuildQuadTree(TSharedPtr<FQuadTreeNode> InNode, FVector InLocation, int LodLevel, float Scale);
	void DestroyTree(TSharedPtr<FQuadTreeNode> InNode);
	TSharedPtr<FQuadTreeNode> QuadRootNode;

};

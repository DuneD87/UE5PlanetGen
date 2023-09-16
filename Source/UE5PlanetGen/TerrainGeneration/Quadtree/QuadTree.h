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
	UFUNCTION()
	void OnTimerEvent();
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
	void TraverseTree(TSharedPtr<FQuadTreeNode>& InNode, int LodLevel, float Scale);
	void BuildQuadTree(TSharedPtr<FQuadTreeNode>& InNode, const FVector& InLocation, float Scale);
	void RemoveQuadTree(TSharedPtr<FQuadTreeNode>& InNode, float Scale);
	void DestroyTree(TSharedPtr<FQuadTreeNode>& InNode);
	//std::vector<float> LOD_LEVELS { 10.0f, 100.0f, 250.0f,500.0f,1000.0f,2500.0f,5000.0f,10000.0f };

	std::vector<float> LOD_LEVELS { 100000.0f,50000.0f, 25000.0f, 10000.0f, 5000.0f, 2500.0f, 1000.0f, 100.0f };
	enum Quadrants
	{
		TopLeft = 0,
		TopRight = 1,
		BottomLeft = 2,
		BottomRight = 3
	};
	FTimerHandle FuzeTimerHandle;
	FVector CameraPosition;
	TSharedPtr<FQuadTreeNode> QuadRootNode;
	TSharedPtr<FQuadTreeNode> ClosestQuad;
};

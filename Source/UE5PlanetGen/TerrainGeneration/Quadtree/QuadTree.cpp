#include "QuadTree.h"

#include "Quad.h"
#include "Editor.h"
#include "Subsystems/UnrealEditorSubsystem.h"



AQuadTree::AQuadTree()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("QuadScene"));
	RootComponent->bComputeBoundsOnceForGame = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

void AQuadTree::Destroyed()
{
	if (QuadRootNode != nullptr)
	{
		DestroyTree(QuadRootNode);
	}
	Super::Destroyed();
}

void AQuadTree::BeginDestroy()
{
	Super::BeginDestroy();
}

void AQuadTree::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}

void AQuadTree::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//En un futuro usar un delegado que detecte si el usuario se ha movido y no estar constantemente calculando distancia
	FVector CurrCameraPosition;
	FRotator CurrCameraRotator;
	GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>()->GetLevelViewportCameraInfo(CurrCameraPosition, CurrCameraRotator);
	if (CurrCameraPosition != CameraPosition)
	{
		CameraPosition = CurrCameraPosition;
		int MaxDistIndex {};
		float MaxDist {std::numeric_limits<float>::max()};
		for (int i = 0; i < QuadRootNode->Nodes.Num(); i++)
		{
			const auto& ChildPos {QuadRootNode->Nodes[i]->Value->GetActorLocation()};
			double Distance {FVector::Distance(ChildPos, CameraPosition)};
			if (Distance < MaxDist)
			{
				UE_LOG(LogTemp, Warning, TEXT("Distance from child %d is %f"),i, Distance);
				MaxDist = Distance;
				MaxDistIndex = i;
			}
		}
		for (int i = 0; i < QuadRootNode->Nodes.Num(); i++)
		{
			if (i == MaxDistIndex)
			{
				UE_LOG(LogTemp, Warning, TEXT("Setting child active: %d"), MaxDistIndex);
				QuadRootNode->Nodes[MaxDistIndex]->Value->SetActiveQuad(true);
			}
			else
			{
				QuadRootNode->Nodes[i]->Value->SetActiveQuad(false);
			}
		}
	}
}

void AQuadTree::PostLoad()
{
	Super::PostLoad();
	//BuildQuadTree();
}

void AQuadTree::PostActorCreated()
{
	Super::PostActorCreated();
	if (!HasAllFlags(RF_Transient))
	{
		//CameraPosition = GetWorld()->GetFirstPlayerController()->K2_GetActorLocation();
		QuadRootNode = MakeShared<FQuadTreeNode>();
		BuildQuadTree(QuadRootNode, this->GetActorLocation(),5, 10000);
	}
}

void AQuadTree::BeginPlay()
{
	Super::BeginPlay();
}

void AQuadTree::BuildQuadTree(TSharedPtr<FQuadTreeNode> InNode, FVector InLocation, int LodLevel, float Scale)
{
	UClass* QuadClass {AQuad::StaticClass()};
	const auto& ForwardVec {RootComponent->GetForwardVector()};
	const auto& RightVec {RootComponent->GetRightVector()};
	float MaxDist {std::numeric_limits<float>::max()};
	int ClosestIndex {-1};
	FVector CurrCameraPosition;
	FRotator CurrCameraRotator;
	GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>()->GetLevelViewportCameraInfo(CurrCameraPosition, CurrCameraRotator);
	
	for (int i = 0; i < 4; i++)
	{
		auto NewQuad {MakeShared<FQuadTreeNode>()};
		FVector Offset;
		switch (i)
		{
		case TopLeft:
			Offset = (ForwardVec + -RightVec) * Scale;
			break;
		case TopRight:
			Offset = (ForwardVec + RightVec) * Scale;
			break;
		case BottomLeft:
			Offset = (-ForwardVec + -RightVec) * Scale;
			break;
		case BottomRight:
			Offset = (-ForwardVec + RightVec) * Scale;
			break;
		default:
			break;
		}

		NewQuad->Value = GetWorld()->SpawnActor<AQuad>(QuadClass, InLocation + Offset, this->GetActorRotation(), FActorSpawnParameters());
		double Distance {FVector::Distance(InLocation + Offset, CameraPosition)};
		if (Distance < MaxDist)
		{
			MaxDist = Distance;
			ClosestIndex = i;
		}
		InNode->Nodes.Push(NewQuad);
		NewQuad->Value->CreateQuad(Scale);
	}
	if (LodLevel != 0)
	{
		BuildQuadTree(InNode->Nodes[ClosestIndex],InNode->Nodes[ClosestIndex]->Value->GetActorLocation(), --LodLevel, Scale/2);
		InNode->Nodes[ClosestIndex]->Value->Destroy();
	}
}

void AQuadTree::DestroyTree(TSharedPtr<FQuadTreeNode> InNode)
{
	if (!InNode->Nodes.IsEmpty())
	{
		for (auto& Node : InNode->Nodes)
		{
			DestroyTree(Node);
			Node->Value->Destroy();
			Node.Reset();
		}
	}
}

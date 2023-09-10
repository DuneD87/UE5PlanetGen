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
	// if (RootNode != nullptr)
	// {
	// 	RootNode->Value->Destroy();
	// 	for (auto Node : RootNode->Nodes)
	// 		Node->Value->Destroy();
	// }
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
	BuildQuadTree();
}

void AQuadTree::PostActorCreated()
{
	Super::PostActorCreated();
	if (!HasAllFlags(RF_Transient))
	{
		//CameraPosition = GetWorld()->GetFirstPlayerController()->K2_GetActorLocation();
		BuildQuadTree();
	}
}

void AQuadTree::BeginPlay()
{
	Super::BeginPlay();
}

void AQuadTree::BuildQuadTree()
{
	QuadRootNode = MakeShared<FQuadTreeNode>();
	UClass* QuadClass {AQuad::StaticClass()};
	float RootScale {100.0f};
	const auto& ForwardVec {RootComponent->GetForwardVector()};
	const auto& RightVec {RootComponent->GetRightVector()};

	for (int i = 0; i < 4; i++)
	{
		auto NewQuad {MakeShared<FQuadTreeNode>()};
		FVector Offset;
		switch (i)
		{
		case 0:
			//TopLeft
			Offset = (ForwardVec + -RightVec) * RootScale;
			break;
		case 1:
			//TopRight
			Offset = (ForwardVec + RightVec) * RootScale;
			break;
		case 2:
			//BottomLeft
			Offset = (-ForwardVec + -RightVec) * RootScale;
			break;
		case 3:
			//BottomRight
			Offset = (-ForwardVec + RightVec) * RootScale;
			break;
		default:
			break;
		}

		NewQuad->Value = GetWorld()->SpawnActor<AQuad>(QuadClass, this->GetActorLocation() + Offset, this->GetActorRotation(), FActorSpawnParameters());
		QuadRootNode->Nodes.Push(NewQuad);	
	}
}

#include "QuadTree.h"

#include "Quad.h"
#include "Editor.h"
#include "Subsystems/UnrealEditorSubsystem.h"

const float MAX_LOD {32.0f};
const float QUADTREE_BOUNDS {10000.0f};

AQuadTree::AQuadTree()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("QuadScene"));
	RootComponent->bComputeBoundsOnceForGame = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

void AQuadTree::OnTimerEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("THIS WORKS!"));
	//RemoveQuadTree(QuadRootNode->Nodes[2]->Nodes[1]->Nodes[3]);

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
		TraverseTree(QuadRootNode, 0, QUADTREE_BOUNDS);
	}
}

void AQuadTree::PostLoad()
{
	Super::PostLoad();
}

void AQuadTree::PostActorCreated()
{
	Super::PostActorCreated();
	//GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &AQuadTree::OnTimerEvent, 15, false);
	if (!HasAllFlags(RF_Transient))
	{
		QuadRootNode = MakeShared<FQuadTreeNode>();

		BuildQuadTree(QuadRootNode, this->GetActorLocation(), QUADTREE_BOUNDS);
	}
}

void AQuadTree::BeginPlay()
{
	Super::BeginPlay();
}

void AQuadTree::TraverseTree(TSharedPtr<FQuadTreeNode>& InNode, int LodLevel, float Scale)
{
	if (InNode->Nodes.Num() !=  0)
	{
		Algo::Sort(InNode->Nodes, [this](const TSharedPtr<FQuadTreeNode>& ValueA, const TSharedPtr<FQuadTreeNode>& ValueB )
			{return FVector::Distance(CameraPosition, ValueA->Value->GetActorLocation()) < FVector::Distance(CameraPosition, ValueB->Value->GetActorLocation());});
		for (auto& Node: InNode->Nodes)
		{
			if (LodLevel < MAX_LOD)
			{
				double DistanceToCamera {FVector::Distance(CameraPosition, Node->Value->GetQuadLocation())};
				UE_LOG(LogTemp, Warning, TEXT("QUAD: %s -- Distance to camera: %f --- LODLEVEL: %d"),*Node->Value->GetActorLabel(), DistanceToCamera, LodLevel);
				if (Node->Nodes.Num() == 0 && (DistanceToCamera <= (Scale*10)/LodLevel + 1))
				{
					BuildQuadTree(Node, Node->Value->GetQuadLocation(), Scale/2);
				}
				else if (Node->Nodes.Num() != 0 && (DistanceToCamera > (Scale*10)/LodLevel))
				{
					RemoveQuadTree(Node, Scale);
				}

				TraverseTree(Node, ++LodLevel, Scale/2);
			}
		}
	}
}

void AQuadTree::BuildQuadTree(TSharedPtr<FQuadTreeNode>& InNode, const FVector& InLocation, float Scale)
{
	const auto& ForwardVec {RootComponent->GetForwardVector()};
	const auto& RightVec {RootComponent->GetRightVector()};
	FColor DebugColor;
	for (int i = 0; i < 4; i++)
	{
		auto NewQuad {MakeShared<FQuadTreeNode>()};
		FVector Offset;
		switch (i)
		{
		case TopLeft:
			Offset = (ForwardVec + -RightVec) * Scale;
			DebugColor = FColor::Blue;
			break;
		case TopRight:
			Offset = (ForwardVec + RightVec) * Scale;
			DebugColor = FColor::Green;
			break;
		case BottomLeft:
			Offset = (-ForwardVec + -RightVec) * Scale;
			DebugColor = FColor::Yellow;
			break;
		case BottomRight:
			Offset = (-ForwardVec + RightVec) * Scale;
			DebugColor = FColor::Red;
			break;
		default:
			break;
		}
		auto NewLabel {FString::Printf(TEXT("Quad - %f - %d"), Scale, i)};
		NewQuad->Value = GetWorld()->SpawnActor<AQuad>(AQuad::StaticClass(), InLocation + Offset, this->GetActorRotation(), FActorSpawnParameters());
		//DrawDebugPoint(GetWorld(),InLocation + Offset, 100, DebugColor);
		NewQuad->Value->SetActorLabel(NewLabel);
		NewQuad->Value->CreateQuad(Scale);
		InNode->Nodes.Push(NewQuad);
	}
	if (InNode->Value != nullptr)
	{
		InNode->Value->Destroy();
	}
}

void AQuadTree::RemoveQuadTree(TSharedPtr<FQuadTreeNode>& InNode, float Scale)
{
	if (InNode->Nodes.Num() != 0)
	{
		FVector CenterPoint {0};
		for (auto& Node : InNode->Nodes)
		{
			CenterPoint.X += Node->Value->GetActorLocation().X;
			CenterPoint.Y += Node->Value->GetActorLocation().Y;
			CenterPoint.Z += Node->Value->GetActorLocation().Z;
			if (Node->Nodes.Num() != 0)
			{
				RemoveQuadTree(Node, Scale);
			}
			Node->Value->Destroy();
			Node.Reset();
		}
		InNode->Value = GetWorld()->SpawnActor<AQuad>(AQuad::StaticClass(), CenterPoint / 4, this->GetActorRotation(), FActorSpawnParameters());
		InNode->Value->CreateQuad(Scale);
		InNode->Nodes.Empty();
	}
}

void AQuadTree::DestroyTree(TSharedPtr<FQuadTreeNode>& InNode)
{
	FlushPersistentDebugLines(GetWorld());
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

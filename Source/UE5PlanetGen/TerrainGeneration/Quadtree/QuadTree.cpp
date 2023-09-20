#include "QuadTree.h"

#include "Quad.h"
#include "Editor.h"
#include "../Util/FastNoiseLite.h"
#include "GeometryCollection/Facades/CollectionUVFacade.h"
#include "Subsystems/UnrealEditorSubsystem.h"

constexpr float MAX_LOD {4.f};
constexpr float QUADTREE_BOUNDS {100000.f};
constexpr float MAGIC_NUMBER {37.f};

AQuadTree::AQuadTree()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("QuadScene"));
	RootComponent->bComputeBoundsOnceForGame = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	NoiseFunc = new FastNoiseLite;
	NoiseFunc->SetSeed(1337);
	NoiseFunc->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	NoiseFunc->SetFrequency(0.0001);
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
		//TraverseTree(QuadRootNode, 0, QUADTREE_BOUNDS);
	}
}

void AQuadTree::PostLoad()
{
	Super::PostLoad();
}

void AQuadTree::PostActorCreated()
{
	Super::PostActorCreated();
	float Scale {QUADTREE_BOUNDS};
	//GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &AQuadTree::OnTimerEvent, 15, false);
	if (!HasAllFlags(RF_Transient))
	{
		QuadRootNode = MakeShared<FQuadTreeNode>();
		for (int i = 0; i < MAX_LOD; i ++)
		{
			Vertices.Emplace(TArray<FVector>());
			Normals.Emplace(TArray<FVector>());
			Indices.Emplace(TArray<int>());
			Uvs.Emplace(TArray<FVector2d>());
			VertexColor.Emplace(TArray<FLinearColor>());
			
			CreateBaseQuad(Scale,4,1, i);
			Scale /= 2;
		}
		
		BuildQuadTree(QuadRootNode, this->GetActorLocation(), QUADTREE_BOUNDS, 0);
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
		auto SortPred {
			[this](const TSharedPtr<FQuadTreeNode>& ValueA, const TSharedPtr<FQuadTreeNode>& ValueB )
			{
				return FVector::Distance(CameraPosition, ValueA->Value->GetActorLocation()) < FVector::Distance(CameraPosition, ValueB->Value->GetActorLocation());
			}
		};
		
		Algo::Sort(InNode->Nodes, SortPred);
		
		for (auto& Node: InNode->Nodes)
		{
			if (LodLevel < MAX_LOD)
			{
				TraverseTree(Node, ++LodLevel, Scale/2);

				double DistanceToCamera {FVector::Distance(CameraPosition, Node->Value->GetQuadLocation())};
				if (Node->Nodes.Num() == 0 && (DistanceToCamera <= (Scale*MAGIC_NUMBER)/LodLevel + 1))//Remember motherfucker, this is to avoid divisions by zero, ITS NOT A MISTAKE.
				{
					BuildQuadTree(Node, Node->Value->GetQuadLocation(), Scale/2, MAX_LOD - LodLevel - 1);
				}
				else if (Node->Nodes.Num() != 0 && (DistanceToCamera > (Scale*MAGIC_NUMBER)/LodLevel + 1)) // Same here petite satanas.
				{
					RemoveQuadTree(Node, Scale, MAX_LOD - LodLevel - 1);
				}
				
			}
		}
	}
}

void AQuadTree::BuildQuadTree(TSharedPtr<FQuadTreeNode>& InNode, const FVector& InLocation, float Scale, int LodLevel)
{
	const auto& ForwardVec {RootComponent->GetForwardVector()};
	const auto& RightVec {RootComponent->GetRightVector()};
	FColor DebugColor;
	auto AuxScale {Scale / 2.0f};
	for (int i = 0; i < 4; i++)
	{
		auto NewQuad {MakeShared<FQuadTreeNode>()};
		FVector Offset;
		switch (i)
		{
		case TopLeft:
			Offset = (ForwardVec + -RightVec) * AuxScale;
			DebugColor = FColor::Blue;
			break;
		case TopRight:
			Offset = (ForwardVec + RightVec) * AuxScale;
			DebugColor = FColor::Green;
			break;
		case BottomLeft:
			Offset = (-ForwardVec + -RightVec) * AuxScale;
			DebugColor = FColor::Yellow;
			break;
		case BottomRight:
			Offset = (-ForwardVec + RightVec) * AuxScale;
			DebugColor = FColor::Red;
			break;
		default:
			break;
		}
		
		NewQuad->Value = GetWorld()->SpawnActor<AQuad>(AQuad::StaticClass(), InLocation + Offset, this->GetActorRotation(), FActorSpawnParameters());
		NewQuad->Value->CreateQuad(Scale, Vertices[LodLevel],Uvs[LodLevel], Normals[LodLevel], Indices[LodLevel], VertexColor[LodLevel]);
		
		InNode->Nodes.Push(NewQuad);
	}
	if (InNode->Value != nullptr)
	{
		InNode->Value->Destroy();
	}
}


void AQuadTree::RemoveQuadTree(TSharedPtr<FQuadTreeNode>& InNode, float Scale, int LodLevel)
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
				RemoveQuadTree(Node, Scale, LodLevel);
			}
			Node->Value->Destroy();
			Node.Reset();
		}
		InNode->Value = GetWorld()->SpawnActor<AQuad>(AQuad::StaticClass(), CenterPoint / 4, this->GetActorRotation(), FActorSpawnParameters());
		InNode->Value->CreateQuad(Scale, Vertices[LodLevel],Uvs[LodLevel], Normals[LodLevel], Indices[LodLevel], VertexColor[LodLevel]);
		
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

void AQuadTree::CreateBaseQuad(float SideLength, int NumSeg, float MaxHeight, int LodLevel)
{
	// Calculate the step size for vertex placement
	float Step = SideLength / NumSeg;
	float UVStep = 1.0f / NumSeg;
	
	// Generate vertices
	for (int32 i = 0; i <= NumSeg; i++)
	{
		for (int32 j = 0; j <= NumSeg; j++)
		{
			// Calculate vertex position
			float X = j * Step - (SideLength / 2.0f);
			float Y = i * Step - (SideLength / 2.0f);
			float Z = NoiseFunc->GetNoise(X, Y) * MaxHeight;
			UE_LOG(LogTemp, Warning, TEXT("ZPos: %f"),Z);
			FVector Vertex = FVector(X, Y, Z);
			VertexColor[LodLevel].Push(FLinearColor::White);
			Vertices[LodLevel].Add(Vertex);
			FVector2D UV = FVector2D(j * UVStep, i * UVStep);
			Uvs[LodLevel].Push(UV);
		}
	}

	// Generate triangles (indices)
	for (int32 i = 0; i < NumSeg; i++)
	{
		for (int32 j = 0; j < NumSeg; j++)
		{
			// Calculate the indices of the four vertices in this quad
			int32 Index0 = i * (NumSeg + 1) + j;
			int32 Index1 = Index0 + 1;
			int32 Index2 = (i + 1) * (NumSeg + 1) + j;
			int32 Index3 = Index2 + 1;

			// Define two triangles using these vertices
			Indices[LodLevel].Add(Index0);
			Indices[LodLevel].Add(Index3);
			Indices[LodLevel].Add(Index2);

			Indices[LodLevel].Add(Index2);
			Indices[LodLevel].Add(Index1);
			Indices[LodLevel].Add(Index0);
		}
	}
	
	for (int i = 0 ; i < Indices.Num(); i+=3)
	{
		int IndexA {Indices[LodLevel][i]};
		int IndexB {Indices[LodLevel][i + 1]};
		int IndexC {Indices[LodLevel][i + 2]};

		FVector EdgeAB {Vertices[LodLevel][IndexB] - Vertices[LodLevel][IndexA]};
		FVector EdgeAC {Vertices[LodLevel][IndexC] - Vertices[LodLevel][IndexA]};

		FVector Normal {EdgeAB.Cross(EdgeAC)};

		Normals[LodLevel].Push(Normal);
	}
}

// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshISMManagerDefs.generated.h"

#undef NULL
#define NULL nullptr
#define KIRCLog( Level, Text ) UE_LOG( LogKeshISMManager, Level, TEXT( Text ) )
#define KIRCLogF( Level, Format, ... ) UE_LOG( LogKeshISMManager, Level, TEXT( Format ), __VA_ARGS__ )

class UKeshISMComponent;
class AKeshISMActor;


USTRUCT()
struct FKeshISMManagerComponentData
{
	GENERATED_BODY()

	FKeshISMManagerComponentData( UInstancedStaticMeshComponent* ISMComponent = NULL )
	{
		this->ISMComponent = ISMComponent;
		KeshISMComponents.SetNum( 0 );
	}

	UPROPERTY()
	TWeakObjectPtr<UInstancedStaticMeshComponent> ISMComponent;

	UPROPERTY()
	TArray<UKeshISMComponent*> KeshISMComponents;

};


USTRUCT()
struct FKeshISMManagerChannelMesh
{
	GENERATED_BODY()

	FKeshISMManagerChannelMesh() { ComponentDataList.SetNum( 0 ); }

	UPROPERTY()
	TArray<FKeshISMManagerComponentData> ComponentDataList;

};


USTRUCT()
struct FKeshISMManagerChannel
{
	GENERATED_BODY()

	FKeshISMManagerChannel() { MeshList.Empty(); }

	UPROPERTY()
	TMap<UStaticMesh*, FKeshISMManagerChannelMesh> MeshList;

};


USTRUCT()
struct FKeshISMManagerWorldData
{
	GENERATED_BODY()

	FKeshISMManagerWorldData( UWorld* World = NULL, AKeshISMActor* KeshISMActor = NULL )
	{
		this->World = World;
		this->KeshISMActor = KeshISMActor;
		Channels.Empty();
	}

	UPROPERTY()
	TWeakObjectPtr<UWorld> World;

	UPROPERTY()
	AKeshISMActor* KeshISMActor;

	UPROPERTY()
	TMap<FName, FKeshISMManagerChannel> Channels;

};


USTRUCT()
struct FKeshISMManagerChannelList
{
	GENERATED_BODY()

	FKeshISMManagerChannelList( FName Channel = NAME_Default )
	{
		this->Channel = Channel;
		ISMComponents.SetNum( 0 );
	}

	UPROPERTY( Category="Kesh ISM Manager", VisibleInstanceOnly )
	FName Channel;

	UPROPERTY( Category = "Kesh ISM Manager", VisibleInstanceOnly, Instanced )
	TArray<UInstancedStaticMeshComponent*> ISMComponents;

};


UENUM( BlueprintType )
enum class EKeshISMPlayUpdateType : uint8
{
	UT_None         UMETA( DisplayName = "Do Not Update" ),
	UT_DisableTick  UMETA( DisplayName = "Disable Ticking" ),
	UT_ConstantTick UMETA( DisplayName = "Constant Ticking" )
};

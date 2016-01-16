// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshISMManagerPluginPrivatePCH.h"
#include "KeshISMManagerPlugin/KeshISMComponent.h"
#include "KeshISMManagerPlugin/KeshISMActor.h"
#include "KeshISMManagerPlugin/KeshISMManager.h"


UKeshISMManager* UKeshISMManager::GetInstance()
{
	static UKeshISMManager* Manager = NULL;
	static FName ManagerName = FName( TEXT( "Kesh ISM Manager" ) );

	if ( Manager == NULL )
	{
		Manager = NewObject<UKeshISMManager>( GetTransientPackage(), ManagerName );
		Manager->AddToRoot();
	}

	return Manager;
}


UKeshISMManager::UKeshISMManager( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	WorldList.SetNum( 0 );
}


bool UKeshISMManager::AddInstance( UKeshISMComponent* KeshISMComponent )
{
	if ( KeshISMComponent == NULL )
		return false;

	if ( KeshISMComponent->Mesh == NULL )
		return false;

	if ( KeshISMComponent->Index >= 0 )
		return UpdateInstance( KeshISMComponent );

	FKeshISMManagerComponentData* ComponentData = GetComponentData( KeshISMComponent );

	if ( ComponentData == NULL )
		return false;

	if ( !ComponentData->ISMComponent.IsValid() )
		return false;

	KeshISMComponent->Index = ComponentData->ISMComponent->AddInstanceWorldSpace( KeshISMComponent->GetComponentTransform() );
	ComponentData->ISMComponent->MarkRenderStateDirty();
	ComponentData->ISMComponent->SetVisibility( false, true );
	ComponentData->ISMComponent->SetVisibility( true, true );

	AKeshISMActor* KeshISMActor = Cast<AKeshISMActor>( ComponentData->ISMComponent->GetOwner() );

	if ( KeshISMActor != NULL )
	{
		FKeshISMManagerChannelList* Channel = NULL;

		for ( FKeshISMManagerChannelList& ChannelData : KeshISMActor->Channels )
		{
			if ( ChannelData.Channel != KeshISMComponent->Channel )
				continue;

			Channel = &ChannelData;
			break;
		}

		if ( Channel == NULL )
		{
			KeshISMActor->Channels.Add( FKeshISMManagerChannelList( KeshISMComponent->Channel ) );
			Channel = &( KeshISMActor->Channels[ KeshISMActor->Channels.Num() - 1 ] );
		}

		if ( !Channel->ISMComponents.Contains( ComponentData->ISMComponent ) )
			Channel->ISMComponents.Add( ComponentData->ISMComponent.Get() );
	}

	if ( KeshISMComponent->Index < 0 )
		return false;

	KeshISMComponent->ChannelComponent = ComponentData->ISMComponent.Get();
	
	if ( ComponentData->KeshISMComponents.Num() <= KeshISMComponent->Index )
		ComponentData->KeshISMComponents.SetNum( KeshISMComponent->Index + 1 );

	ComponentData->KeshISMComponents[ KeshISMComponent->Index ] = KeshISMComponent;

	return true;
}


bool UKeshISMManager::UpdateInstance( UKeshISMComponent* KeshISMComponent )
{
	if ( KeshISMComponent == NULL )
		return false;

	if ( KeshISMComponent->Index < 0 )
		return AddInstance( KeshISMComponent );

	if ( KeshISMComponent->Mesh == NULL )
		return false;

	bool bUpdateTransformOnly = true;

	if ( KeshISMComponent->ChannelComponent == NULL )
		bUpdateTransformOnly = false;

	else if ( KeshISMComponent->ChannelComponent->StaticMesh != KeshISMComponent->Mesh )
		bUpdateTransformOnly = false;

	else if ( KeshISMComponent->ChannelComponent->GetNumOverrideMaterials() != KeshISMComponent->MaterialOverrides.Num() )
		bUpdateTransformOnly = false;

	else
	{
		for ( int32 i = 0, length = KeshISMComponent->ChannelComponent->GetNumOverrideMaterials(); i < length; ++i )
		{
			if ( KeshISMComponent->ChannelComponent->GetMaterial( i ) == KeshISMComponent->MaterialOverrides[ i ] )
				continue;

			bUpdateTransformOnly = false;
			break;
		}

		if ( bUpdateTransformOnly )
		{
			FName ISMChannel = NAME_None;
			AKeshISMActor* KeshISMActor = Cast<AKeshISMActor>( KeshISMComponent->ChannelComponent->GetOwner() );

			if ( KeshISMActor == NULL )
				bUpdateTransformOnly = false;

			else if ( !GetISMChannel( KeshISMActor, KeshISMComponent->ChannelComponent, ISMChannel, false ) )
				bUpdateTransformOnly = false;

			else
				bUpdateTransformOnly = ( ISMChannel == KeshISMComponent->Channel );
		}
	}

	if ( bUpdateTransformOnly )
		return UpdateInstanceTransform( KeshISMComponent );

	RemoveInstance( KeshISMComponent );
	return AddInstance( KeshISMComponent );
}


bool UKeshISMManager::UpdateInstanceTransform( UKeshISMComponent* KeshISMComponent )
{
	if ( KeshISMComponent == NULL )
		return false;

	if ( KeshISMComponent->Index < 0 )
		return AddInstance( KeshISMComponent );

	if ( KeshISMComponent->Mesh == NULL )
		return false;

	if ( KeshISMComponent->ChannelComponent == NULL )
		return false;

	KeshISMComponent->ChannelComponent->UpdateInstanceTransform( KeshISMComponent->Index, KeshISMComponent->GetComponentTransform(), true, true );
	KeshISMComponent->ChannelComponent->MarkRenderStateDirty();
	KeshISMComponent->ChannelComponent->SetVisibility( false, true );
	KeshISMComponent->ChannelComponent->SetVisibility( true, true );
	
	return true;
}


bool UKeshISMManager::RemoveInstance( UKeshISMComponent* KeshISMComponent )
{
	if ( KeshISMComponent == NULL )
		return false;

	if ( KeshISMComponent->Index < 0 )
		return false;

	if ( KeshISMComponent->Mesh == NULL )
		return false;
	
	if ( KeshISMComponent->GetWorld() == NULL )
		return NULL;

	FKeshISMManagerWorldData* WorldData = GetWorldData( KeshISMComponent->GetWorld(), false );

	if ( WorldData == NULL )
		return false;

	FKeshISMManagerChannel* ChannelData = GetChannelData( *WorldData, KeshISMComponent->Channel, false );

	if ( ChannelData == NULL )
		return false;

	FKeshISMManagerChannelMesh* MeshData = GetMeshData( *ChannelData, KeshISMComponent->Mesh, false );

	if ( MeshData == NULL )
		return false;

	FKeshISMManagerComponentData* ComponentData = GetComponentData( WorldData->KeshISMActor, KeshISMComponent->Channel, KeshISMComponent->Mesh, *MeshData, KeshISMComponent->MaterialOverrides, false );

	if ( ComponentData == NULL )
		return false;

	if ( KeshISMComponent->Index >= ComponentData->KeshISMComponents.Num() )
		return false;

	if ( WorldData->KeshISMActor != NULL )
	{
		int32 Index = INDEX_NONE;

		for ( int32 i = 0, length = WorldData->KeshISMActor->Channels.Num(); i < length; ++i )
		{
			if ( WorldData->KeshISMActor->Channels[ i ].Channel != KeshISMComponent->Channel )
				continue;

			Index = i;
			break;
		}

		if ( Index != INDEX_NONE )
		{
			WorldData->KeshISMActor->Channels[ Index ].ISMComponents.Remove( ComponentData->ISMComponent.Get() );

			if ( WorldData->KeshISMActor->Channels[ Index ].ISMComponents.Num() == 0 )
				WorldData->KeshISMActor->Channels.RemoveAt( Index, 1, true );
		}
	}

	// Remove component data and possibly entire chain.
	if ( ComponentData->KeshISMComponents.Num() == 1 )
	{
		ComponentData->ISMComponent->DestroyComponent();
		ComponentData->ISMComponent = NULL;

		// Remove mesh data and potentially channel data
		if ( MeshData != NULL )
		{
			for ( int32 i = 0; i < MeshData->ComponentDataList.Num(); ++i )
			{
				if ( &( MeshData->ComponentDataList[ i ] ) != ComponentData )
					continue;

				MeshData->ComponentDataList.RemoveAtSwap( i, 1, true );
				break;
			}

			if ( MeshData->ComponentDataList.Num() == 0 && ChannelData != NULL )
			{
				ChannelData->MeshList.Remove( KeshISMComponent->Mesh );

				if ( ChannelData->MeshList.Num() == 0 && WorldData != NULL )
					WorldData->Channels.Remove( KeshISMComponent->Channel );
			}
		}
	}

	// Swap or remove the instance
	else
	{
		int32 LastIndex = ComponentData->KeshISMComponents.Num() - 1;

		if ( LastIndex != KeshISMComponent->Index )
		{
			FTransform Transform;

			ComponentData->ISMComponent->GetInstanceTransform( LastIndex, Transform, true );
			ComponentData->ISMComponent->UpdateInstanceTransform( KeshISMComponent->Index, Transform, true, true );

			if ( ComponentData->KeshISMComponents[ LastIndex ] != NULL )
				ComponentData->KeshISMComponents[ LastIndex ]->Index = KeshISMComponent->Index;

			ComponentData->KeshISMComponents[ KeshISMComponent->Index ] = ComponentData->KeshISMComponents[ LastIndex ];
		}			

		ComponentData->ISMComponent->RemoveInstance( LastIndex );
		ComponentData->ISMComponent->MarkRenderStateDirty();
		ComponentData->ISMComponent->SetVisibility( false, true );
		ComponentData->ISMComponent->SetVisibility( true, true );

		ComponentData->KeshISMComponents.SetNum( LastIndex );
	}

	KeshISMComponent->Index = -1;
	KeshISMComponent->ChannelComponent = NULL;

	return true;
}


bool UKeshISMManager::RegisterWorld( AKeshISMActor* KeshISMActor )
{
	if ( KeshISMActor == NULL )
		return false;

	WorldList.Add( FKeshISMManagerWorldData( KeshISMActor->GetWorld(), KeshISMActor ) );

	FKeshISMManagerWorldData* WorldData = GetWorldData( KeshISMActor->GetWorld(), false );

	if ( WorldData == NULL )
		return false;

	TArray<UActorComponent*> ISMComponents = KeshISMActor->GetComponentsByClass( UInstancedStaticMeshComponent::StaticClass() );

	for ( UActorComponent* ActorComponent : ISMComponents )
	{
		UInstancedStaticMeshComponent* ISMComponent = Cast<UInstancedStaticMeshComponent>( ActorComponent );

		if ( ISMComponent == NULL )
			continue;

		ISMComponent->ClearInstances();

		if ( ISMComponent->GetWorld() == NULL )
			continue;

		if ( ISMComponent->StaticMesh == NULL )
			continue;

		FName Channel;
		bool bSuccess = GetISMChannel( KeshISMActor, ISMComponent, Channel, true );

		if ( !bSuccess )
			continue;

		FKeshISMManagerChannel* ChannelData = GetChannelData( *WorldData, Channel, true );

		if ( ChannelData == NULL )
			continue;

		FKeshISMManagerChannelMesh* MeshData = GetMeshData( *ChannelData, ISMComponent->StaticMesh, true );

		if ( MeshData == NULL )
			continue;

		TArray<UMaterialInterface*> MaterialOverrides;
		MaterialOverrides.SetNum( ISMComponent->GetNumOverrideMaterials() );

		for ( int32 i = 0, length = ISMComponent->GetNumOverrideMaterials(); i < length; ++i )
			MaterialOverrides[ i ] = ISMComponent->GetMaterial( i );

		FKeshISMManagerComponentData* ComponentData = GetComponentData( WorldData->KeshISMActor, Channel, ISMComponent->StaticMesh, *MeshData, MaterialOverrides, false );

		if ( ComponentData != NULL )
			ComponentData->KeshISMComponents.SetNum( 0 );

		else
			MeshData->ComponentDataList.Add( FKeshISMManagerComponentData( ISMComponent ) );
	}

	return true;
}


void UKeshISMManager::RemoveWorld( UWorld* World )
{
	if ( World == NULL )
		return;

	for ( int32 i = 0; i < WorldList.Num(); ++i )
	{
		if ( WorldList[ i ].World.Get() != World )
			continue;

		if ( WorldList[ i ].KeshISMActor != NULL )
		{
			TArray<UActorComponent*> Components = WorldList[ i ].KeshISMActor->GetComponentsByClass( UKeshISMComponent::StaticClass() );			

			for ( UActorComponent* ActorComponent : Components )
			{
				UKeshISMComponent* KeshISMComponent = Cast<UKeshISMComponent>( ActorComponent );

				if ( KeshISMComponent == NULL )
					continue;

				KeshISMComponent->Index = -1;
			}

			WorldList[ i ].KeshISMActor = NULL;
		}

		WorldList[ i ].World = NULL;
		WorldList.RemoveAtSwap( i, 1, true );
		break;
	}
}


void UKeshISMManager::ScanForInvalidWorlds()
{
	for ( int32 i = 0; i < WorldList.Num(); ++i )
	{
		if ( WorldList[ i ].World.Get() != NULL )
			continue;

		WorldList.RemoveAtSwap( i, 1, true );
		--i;
	}
}


FKeshISMManagerComponentData* UKeshISMManager::GetComponentData( UInstancedStaticMeshComponent* ISMComponent, bool bCreate )
{
	if ( ISMComponent == NULL )
		return NULL;

	if ( ISMComponent->StaticMesh == NULL )
		return NULL;

	UWorld* World = ISMComponent->GetWorld();

	if ( World == NULL )
		return NULL;

	AKeshISMActor* KeshISMActor = Cast<AKeshISMActor>( ISMComponent->GetOwner() );

	if ( KeshISMActor == NULL )
		return NULL;

	ScanForInvalidWorlds();

	FKeshISMManagerWorldData* WorldData = GetWorldData( World, bCreate );

	if ( WorldData == NULL )
		return NULL;

	FName Channel;
	bool bSuccess = GetISMChannel( KeshISMActor, ISMComponent, Channel, bCreate );

	if ( !bSuccess )
		return NULL;

	FKeshISMManagerChannel* ChannelData = GetChannelData( *WorldData, Channel, bCreate );

	if ( ChannelData == NULL )
		return NULL;

	FKeshISMManagerChannelMesh* MeshData = GetMeshData( *ChannelData, ISMComponent->StaticMesh, bCreate );

	if ( MeshData == NULL )
		return NULL;

	TArray<UMaterialInterface*> MaterialOverrides;
	MaterialOverrides.SetNum( ISMComponent->GetNumOverrideMaterials() );

	for ( int32 i = 0, length = ISMComponent->GetNumOverrideMaterials(); i < length; ++i )
		MaterialOverrides[ i ] = ISMComponent->GetMaterial( i );

	return GetComponentData( WorldData->KeshISMActor, Channel, ISMComponent->StaticMesh, *MeshData, MaterialOverrides, bCreate );
}


FKeshISMManagerComponentData* UKeshISMManager::GetComponentData( UKeshISMComponent* KeshISMComponent, bool bCreate )
{
	if ( KeshISMComponent == NULL )
		return NULL;

	if ( KeshISMComponent->Index < 0 && !bCreate )
		return NULL;

	UWorld* World = KeshISMComponent->GetWorld();

	if ( World == NULL )
		return NULL;

	ScanForInvalidWorlds();
	
	FKeshISMManagerWorldData* WorldData = GetWorldData( World, bCreate );

	if ( WorldData == NULL )
		return NULL;

	FKeshISMManagerChannel* ChannelData = GetChannelData( *WorldData, KeshISMComponent->Channel, bCreate );

	if ( ChannelData == NULL )
		return NULL;

	FKeshISMManagerChannelMesh* MeshData = GetMeshData( *ChannelData, KeshISMComponent->Mesh, bCreate );

	if ( MeshData == NULL )
		return NULL;

	return GetComponentData( WorldData->KeshISMActor, KeshISMComponent->Channel, KeshISMComponent->Mesh, *MeshData, KeshISMComponent->MaterialOverrides, bCreate );
}


FKeshISMManagerWorldData* UKeshISMManager::GetWorldData( UWorld* World, bool bCreate )
{
	if ( World == NULL )
		return NULL;

	for ( FKeshISMManagerWorldData& WorldData : WorldList )
	{
		if ( WorldData.World.Get() != World )
			continue;

		return &WorldData;
	}

	if ( !bCreate )
		return NULL;

	AKeshISMActor* KeshISMActor = NULL;

	for ( TActorIterator<AActor> ActorItr( World ); ActorItr; ++ActorItr )
	{
		KeshISMActor = Cast<AKeshISMActor>( *ActorItr );

		if ( KeshISMActor != NULL )
			break;
	}

	if ( KeshISMActor == NULL )
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;

		KeshISMActor = World->SpawnActor<AKeshISMActor>( SpawnParams );

		if ( KeshISMActor == NULL )
			return NULL;
	}

	WorldList.Add( FKeshISMManagerWorldData( World, KeshISMActor ) );

	return &( WorldList[ WorldList.Num() - 1 ] );
}


bool UKeshISMManager::GetISMChannel( AKeshISMActor* KeshISMActor, UInstancedStaticMeshComponent* ISMComponent, FName& Channel, bool bCreate )
{
	if ( KeshISMActor == NULL )
		return false;

	for ( FKeshISMManagerChannelList& ChannelList : KeshISMActor->Channels )
	{
		if ( !ChannelList.ISMComponents.Contains( ISMComponent ) )
			continue;

		Channel = ChannelList.Channel;
		return true;
	}

	return false;
}


FKeshISMManagerChannel* UKeshISMManager::GetChannelData( FKeshISMManagerWorldData& WorldData, FName Channel, bool bCreate )
{
	if ( !WorldData.Channels.Contains( Channel ) )
	{
		if ( !bCreate )
			return NULL;

		WorldData.Channels.Emplace( Channel, FKeshISMManagerChannel() );
	}

	return &( WorldData.Channels[ Channel ] );
}


FKeshISMManagerChannelMesh* UKeshISMManager::GetMeshData( FKeshISMManagerChannel& ChannelData, UStaticMesh* Mesh, bool bCreate )
{
	if ( Mesh == NULL )
		return NULL;

	if ( !ChannelData.MeshList.Contains( Mesh ) )
	{
		if ( !bCreate )
			return NULL;

		ChannelData.MeshList.Emplace( Mesh, FKeshISMManagerChannelMesh() );
	}

	return &( ChannelData.MeshList[ Mesh ] );
}


FKeshISMManagerComponentData* UKeshISMManager::GetComponentData( AKeshISMActor* KeshISMActor, FName Channel, UStaticMesh* Mesh, FKeshISMManagerChannelMesh& MeshData, const TArray<UMaterialInterface*>& MaterialOverrides, bool bCreate )
{
	int32 MaterialCount = MaterialOverrides.Num();
	
	for ( FKeshISMManagerComponentData& ComponentData : MeshData.ComponentDataList )
	{
		if ( !ComponentData.ISMComponent.IsValid() )
			continue;

		if ( ComponentData.ISMComponent->GetNumOverrideMaterials() != MaterialCount )
			continue;

		bool bMatch = true;

		for ( int32 i = 0; i < MaterialCount; ++i )
		{
			if ( ComponentData.ISMComponent->GetMaterial( i ) == MaterialOverrides[ i ] )
				continue;

			bMatch = false;
			break;
		}

		if ( bMatch )
			return &ComponentData;
	}

	if ( !bCreate )
		return NULL;

	if ( KeshISMActor == NULL )
		return NULL;

	UInstancedStaticMeshComponent* ISMComponent = NewObject<UInstancedStaticMeshComponent>( KeshISMActor );
	ISMComponent->RegisterComponent();

	if ( ISMComponent == NULL )
		return NULL;

	int32 Index = INDEX_NONE;

	for ( int32 i = 0, length = KeshISMActor->Channels.Num(); i < length; ++i )
	{
		if ( KeshISMActor->Channels[ i ].Channel != Channel )
			continue;

		Index = i;
		break;
	}

	if ( Index == INDEX_NONE )
	{
		KeshISMActor->Channels.Add( FKeshISMManagerChannelList( Channel ) );
		Index = KeshISMActor->Channels.Num() - 1;
	}

	KeshISMActor->Channels[ Index ].ISMComponents.Add( ISMComponent );

	ISMComponent->SetStaticMesh( Mesh );

	for ( int32 i = 0; i < MaterialCount; ++i )
		ISMComponent->SetMaterial( i, MaterialOverrides[ i ] );

	MeshData.ComponentDataList.Add( FKeshISMManagerComponentData( ISMComponent ) );

	return &( MeshData.ComponentDataList[ MeshData.ComponentDataList.Num() - 1 ] );
}

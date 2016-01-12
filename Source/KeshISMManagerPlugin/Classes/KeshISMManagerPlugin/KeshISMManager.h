// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshISMManagerPlugin/KeshISMManagerTypes.h"
#include "KeshISMManager.generated.h"

class UKeshISMComponent;
class AKeshISMActor;


/**
* Singleton manager for all ISM components.
*/
UCLASS( Category = "Kesh ISM Manager", Blueprintable, BlueprintType )
class KESHISMMANAGERPLUGIN_API UKeshISMManager : public UObject
{
	GENERATED_BODY()

public:

	static UKeshISMManager* GetInstance();

	UKeshISMManager( const class FObjectInitializer& ObjectInitializer );

	virtual bool AddInstance( UKeshISMComponent* KeshISMComponent );

	virtual bool UpdateInstance( UKeshISMComponent* KeshISMComponent );

	virtual bool UpdateInstanceTransform( UKeshISMComponent* KeshISMComponent );

	virtual bool RemoveInstance( UKeshISMComponent* KeshISMComponent );

	virtual bool RegisterWorld( AKeshISMActor* KeshISMActor );

	virtual void RemoveWorld( UWorld* World );

protected:

	TArray<FKeshISMManagerWorldData> WorldList;

	void ScanForInvalidWorlds();

	virtual FKeshISMManagerComponentData* GetComponentData( UInstancedStaticMeshComponent* ISMComponent, bool bCreate = true );

	virtual FKeshISMManagerComponentData* GetComponentData( UKeshISMComponent* KeshISMComponent, bool bCreate = true );

	FKeshISMManagerWorldData* GetWorldData( UWorld* World, bool bCreate = true );

	bool GetISMChannel( AKeshISMActor* KeshISMActor, UInstancedStaticMeshComponent* ISMComponent, FName& Channel, bool bCreate = true );

	FKeshISMManagerChannel* GetChannelData( FKeshISMManagerWorldData& WorldData, FName Channel, bool bCreate = true );

	FKeshISMManagerChannelMesh* GetMeshData( FKeshISMManagerChannel& ChannelData, UStaticMesh* Mesh, bool bCreate = true );

	FKeshISMManagerComponentData* GetComponentData( AKeshISMActor* KeshISMActor, FName Channel, UStaticMesh* Mesh, FKeshISMManagerChannelMesh& MeshData, const TArray<UMaterialInterface*>& MaterialOverrides, bool bCreate = true );

};
// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshISMManagerPlugin/KeshISMManagerDefs.h"
#include "KeshISMComponent.generated.h"

/**
* Scene component representing a single instance of an instanced static mesh.
*/
UCLASS( ClassGroup = "Kesh ISM Manager", Blueprintable, BlueprintType, Meta = ( BlueprintSpawnableComponent ) )
class KESHISMMANAGERPLUGIN_API UKeshISMComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UKeshISMComponent( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	FName GetChannel() const { return Channel; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	int32 GetIndex() { return Index; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	UInstancedStaticMeshComponent* GetChannelComponent() const { return ChannelComponent; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	UStaticMesh* GetStaticMesh() const { return Mesh; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	virtual void SetStaticMesh( UStaticMesh* Mesh );

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	TArray<UMaterialInterface*> GetMaterialOverridesBP() const { return MaterialOverrides; }
	const TArray<UMaterialInterface*>& GetMaterialOverrides() const { return MaterialOverrides; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	virtual void SetMaterialOverride( int32 Index, UMaterialInterface* Material );

#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
	virtual void PostEditComponentMove( bool bComplete ) override; // Will help to fix things if realtime is off and the KeshISMComponent is the root.
#endif

	virtual void PostLoad() override;
	virtual void OnRegister() override;
	virtual void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	virtual void SendRenderTransform_Concurrent() override;
	virtual void OnComponentDestroyed() override;

protected:

	friend class UKeshISMManager;

	int32 Index;
	bool bValidISM;
	bool bValidTransform;
	float fLastTickUpdate;

	static const float TIME_BEFORE_TICK_DISABLE;

	// The mesh will update 1 tick behind the camera. Do not attach to the camera.
	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	EKeshISMPlayUpdateType UpdateDuringPlay;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	FName Channel;

	UPROPERTY()
	UInstancedStaticMeshComponent* ChannelComponent;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	UStaticMesh* Mesh;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	TArray<UMaterialInterface*> MaterialOverrides;

};

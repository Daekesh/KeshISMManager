// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshISMManagerPlugin/KeshISMManagerTypes.h"
#include "KeshISMComponent.generated.h"

/**
* Scene component representing a single instance of the ISM manager.
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
	UInstancedStaticMeshComponent* GetChannelComponent() const { return ChannelComponent; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	UStaticMesh* GetMesh() const { return Mesh; }

	UFUNCTION( Category = "Kesh ISM Manager", BlueprintCallable )
	TArray<UMaterialInterface*> GetMaterialOverridesBP() const { return MaterialOverrides; }
	const TArray<UMaterialInterface*>& GetMaterialOverrides() const { return MaterialOverrides; }

#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent );
	virtual void PostEditComponentMove( bool bFinished ) override;
#endif

	virtual void PostLoad() override;
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	virtual void OnComponentDestroyed() override;

protected:

	friend class UKeshISMManager;

	int32 Index;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	bool bUpdateDuringPlay;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	FName Channel;

	UPROPERTY()
	UInstancedStaticMeshComponent* ChannelComponent;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	UStaticMesh* Mesh;

	UPROPERTY( Category = "Kesh ISM Manager|Component", EditAnywhere )
	TArray<UMaterialInterface*> MaterialOverrides;

};

// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshISMManagerPlugin/KeshISMManagerDefs.h"
#include "KeshISMActor.generated.h"

/**
* Scene actor holding all the ISMs for a specific world.
* Deletes itself on start-up so a new world-specific actor can be made.
*/
UCLASS( ClassGroup = "Kesh ISM Manager", Blueprintable, BlueprintType, NotPlaceable )
class KESHISMMANAGERPLUGIN_API AKeshISMActor : public AActor
{
	GENERATED_BODY()

public:

	AKeshISMActor( const class FObjectInitializer& ObjectInitializer );

	virtual void PostLoad() override;
	virtual void Destroyed() override;

protected:

	friend class UKeshISMManager;

	UPROPERTY( Category = "Kesh ISM Manager|Actor", VisibleAnywhere )
	TArray<FKeshISMManagerChannelList> Channels;

};

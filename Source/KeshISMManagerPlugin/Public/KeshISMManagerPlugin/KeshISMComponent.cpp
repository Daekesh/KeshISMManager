// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshISMManagerPluginPrivatePCH.h"
#include "KeshISMManagerPlugin/KeshISMManager.h"
#include "KeshISMManagerPlugin/KeshISMComponent.h"

namespace FKeshISMComponent
{
	FName DefaultChannel = FName( TEXT( "Default" ) );
}


UKeshISMComponent::UKeshISMComponent( const class FObjectInitializer& ObjectInitializer )
{
	Index = -1;
	Channel = FKeshISMComponent::DefaultChannel;
	Mesh = NULL;
	MaterialOverrides.SetNum( 0 );
}


#if WITH_EDITOR

void UKeshISMComponent::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	if ( PropertyChangedEvent.Property == NULL )
		return;

	if ( PropertyChangedEvent.Property->GetName() == "Channel" ||
		 PropertyChangedEvent.Property->GetName() == "Mesh" ||
		 PropertyChangedEvent.Property->GetName() == "MaterialOverrides" )
	{
		UKeshISMManager* Manager = UKeshISMManager::GetInstance();
		Manager->UpdateInstance( this );
	}

	else if ( PropertyChangedEvent.Property->GetName() == "RelativeLocation" ||
			  PropertyChangedEvent.Property->GetName() == "RelativeRotation" ||
			  PropertyChangedEvent.Property->GetName() == "RelativeScale3D" )
	{
		UKeshISMManager* Manager = UKeshISMManager::GetInstance();
		Manager->UpdateInstanceTransform( this );
	}
}


void UKeshISMComponent::PostEditComponentMove( bool bFinished )
{
	Super::PostEditComponentMove( bFinished );

	AActor* RootActor = GetOwner();
	UKeshISMManager* Manager = UKeshISMManager::GetInstance();

	if ( RootActor == NULL )
	{
		Manager->UpdateInstanceTransform( this );
		return;
	}

	TArray<UActorComponent*> KeshISMComponents = RootActor->GetComponentsByClass( UKeshISMComponent::StaticClass() );

	for ( UActorComponent* ActorComponent : KeshISMComponents )
	{ 
		UKeshISMComponent* KeshISMComponent = Cast<UKeshISMComponent>( ActorComponent );

		if ( KeshISMComponent == NULL )
			continue;

		Manager->UpdateInstanceTransform( KeshISMComponent );
	}
}

#endif


void UKeshISMComponent::PostLoad()
{
	Super::PostLoad();

	Index = -1;
}


void UKeshISMComponent::OnRegister()
{
	Super::OnRegister();

	UKeshISMManager* Manager = UKeshISMManager::GetInstance();
	Manager->AddInstance( this );
}


void UKeshISMComponent::OnComponentDestroyed()
{
	UKeshISMManager* Manager = UKeshISMManager::GetInstance();
	Manager->RemoveInstance( this );

	Super::OnComponentDestroyed();
}

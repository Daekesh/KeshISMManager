// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshISMManagerPluginPrivatePCH.h"
#include "KeshISMManagerPlugin/KeshISMManager.h"
#include "KeshISMManagerPlugin/KeshISMComponent.h"


const float UKeshISMComponent::TIME_BEFORE_TICK_DISABLE = 0.1f;


UKeshISMComponent::UKeshISMComponent( const class FObjectInitializer& ObjectInitializer )
{
	Index = -1;
	Channel = NAME_Default;
	Mesh = NULL;
	MaterialOverrides.SetNum( 0 );
	UpdateDuringPlay = EKeshISMPlayUpdateType::UT_None;
	bValidISM = false;
	bValidTransform = false;
	fLastTickUpdate = 0.f;

	bAutoActivate = true;
	bNeverNeedsRenderUpdate = false;
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
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
		bValidISM = false;

		if ( UpdateDuringPlay == EKeshISMPlayUpdateType::UT_DisableTick )
			SetComponentTickEnabled( true );
	}
}


void UKeshISMComponent::PostEditComponentMove( bool bComplete )
{
	Super::PostEditComponentMove( bComplete );

	AActor* Owner = GetOwner();

	if ( Owner == NULL )
		return;

	UKeshISMManager* Manager = UKeshISMManager::GetInstance();

	if ( Manager == NULL )
		return;

	for ( UActorComponent* ActorComponent : Owner->GetComponents() )
	{
		UKeshISMComponent* KeshISMComponent = Cast<UKeshISMComponent>( ActorComponent );

		if ( KeshISMComponent == NULL )
			return;

		Manager->UpdateInstanceTransform( KeshISMComponent );
	}
}

#endif


void UKeshISMComponent::SendRenderTransform_Concurrent()
{
	Super::SendRenderTransform_Concurrent();

	bValidTransform = false;

	if ( UpdateDuringPlay == EKeshISMPlayUpdateType::UT_DisableTick )
		SetComponentTickEnabled( true );
}


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

#if WITH_EDITOR
	if ( GetWorld()->WorldType == EWorldType::Editor )
		SetComponentTickEnabled( true );

	else
#endif
		SetComponentTickEnabled( UpdateDuringPlay == EKeshISMPlayUpdateType::UT_ConstantTick );
}


void UKeshISMComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if ( bValidISM && bValidTransform )
	{

#if WITH_EDITOR
		if ( GetWorld()->WorldType == EWorldType::Editor )
			return;
#endif

		if ( UpdateDuringPlay != EKeshISMPlayUpdateType::UT_ConstantTick )
		{
			if ( ( GetWorld()->TimeSeconds - fLastTickUpdate ) >= UKeshISMComponent::TIME_BEFORE_TICK_DISABLE )
				SetComponentTickEnabled( false );
		}

		return;
	}

	UKeshISMManager* Manager = UKeshISMManager::GetInstance();

	if ( Manager != NULL )
	{
		if ( !bValidISM )
			Manager->UpdateInstance( this );

		else if ( !bValidTransform )
			Manager->UpdateInstanceTransform( this );
	}

	bValidISM = true;
	bValidTransform = true;

	if ( UpdateDuringPlay == EKeshISMPlayUpdateType::UT_DisableTick )
		fLastTickUpdate = GetWorld()->TimeSeconds;
}


void UKeshISMComponent::OnComponentDestroyed()
{
	UKeshISMManager* Manager = UKeshISMManager::GetInstance();

	if ( Manager != NULL )
		Manager->RemoveInstance( this );

	Super::OnComponentDestroyed();
}


void UKeshISMComponent::SetStaticMesh( UStaticMesh* Mesh )
{
	if ( this->Mesh == Mesh )
		return;

	UKeshISMManager* Manager = UKeshISMManager::GetInstance();

	if ( Manager != NULL && this->ChannelComponent != NULL )
		Manager->RemoveInstance( this );

	this->Mesh = Mesh;

	if ( this->Mesh != NULL )
	{
		bValidISM = false;

		if ( UpdateDuringPlay == EKeshISMPlayUpdateType::UT_DisableTick )
			SetComponentTickEnabled( true );
	}
}


void UKeshISMComponent::SetMaterialOverride( int32 Index, UMaterialInterface* Material )
{
	if ( Index < 0 )
		return;

	if ( Index >= MaterialOverrides.Num() )
	{
		if ( Material == NULL )
			return;

		MaterialOverrides.AddZeroed( Index - MaterialOverrides.Num() + 1 );
	}

	if ( Index == ( MaterialOverrides.Num() - 1 ) && Material == NULL )
		MaterialOverrides.SetNum( MaterialOverrides.Num() - 1 );

	else
		MaterialOverrides[ Index ] = Material;

	bValidISM = false;

	if ( UpdateDuringPlay == EKeshISMPlayUpdateType::UT_DisableTick )
		SetComponentTickEnabled( true );
}

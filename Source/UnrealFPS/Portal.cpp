// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    bIsActive = false;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent->Mobility = EComponentMobility::Static;
    
    // Allows us to access the portal plane without depending on blueprint functions
    PortalRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRootComponent"));
    PortalRootComponent->SetupAttachment( GetRootComponent() );
    PortalRootComponent->SetRelativeLocation( FVector(0.0f, 0.0f, 0.0f) );
    PortalRootComponent->SetRelativeRotation( FRotator(0.0f, 0.0f, 0.0f) );
    PortalRootComponent->Mobility = EComponentMobility::Movable; // Set as dynamic in case the blueprint class animates it

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::SetActive(bool NewActiveState)
{
    bIsActive = NewActiveState;
}

bool APortal::IsActive()
{
    return bIsActive;
}

// Empty default implementation
void APortal::SetRenderTargetTexture_Implementation(UTexture *RenderTexture)
{

}

// Empty default implementation
void APortal::ClearRenderTargetTexture_Implementation()
{

}

// Empty default implementation
void APortal::ForceTick_Implementation()
{

}

void APortal::SetTarget(AActor *NewTarget)
{
    Target = NewTarget;
}

AActor *APortal::GetTarget()
{
    return Target;
}

bool APortal::IsLocationInFront(FVector Location, FVector PortalLocation, FVector PortalNormal)
{
    FPlane PlaneOfPortal = FPlane(PortalLocation, PortalNormal);
    
    return PlaneOfPortal.PlaneDot(Location) >= 0;
}

bool APortal::HasLocationCrossed(FVector NewLocation, FVector PortalLocation, FVector PortalNormal)
{
    FVector IntersectionPoint;
    FPlane PlaneOfPortal = FPlane(PortalLocation, PortalNormal);
    
    bool HasIntersected = FMath::SegmentPlaneIntersection(LastLocation, NewLocation, PlaneOfPortal, IntersectionPoint);
    bool HasCrossedPortal = HasIntersected && IsLocationInFront(LastLocation, PortalLocation, PortalNormal) && !IsLocationInFront(NewLocation, PortalLocation, PortalNormal);
    UE_LOG(LogTemp, Warning, TEXT("HasCrossedPortal: %s"), ( HasCrossedPortal ? TEXT("true") : TEXT("false") ));
    
    LastLocation = NewLocation;
    return HasCrossedPortal;
}


void APortal::TeleportActor(AActor *ActorToTeleport)
{
    if (ActorToTeleport == nullptr || Target == nullptr)
    {
        return;
    }
    
    // Apply new location
    FVector NewLocation = ConvertLocationToActorSpace(ActorToTeleport->GetActorLocation(), this, Target);
    FHitResult HitResult;
    ActorToTeleport->SetActorLocation(NewLocation, false, &HitResult, ETeleportType::TeleportPhysics);
    
    // Update the last location
    LastLocation = NewLocation;
    
    // Apply new rotation
    FRotator NewRotation = ConvertRotationToActorSpace(ActorToTeleport->GetActorRotation(), this, Target);
    ActorToTeleport->SetActorRotation( NewRotation );
    
    // TODO THIS IS INCOMPLETE
    if (ActorToTeleport->IsA(AMainCharacter::StaticClass()))
    {
        FVector Velocity = FVector::ZeroVector;
        AMainCharacter *MainCharacter = nullptr;

        if(ActorToTeleport->IsA(AMainCharacter::StaticClass()))
        {
            MainCharacter = Cast<AMainCharacter>(ActorToTeleport);

            Velocity = MainCharacter->GetVelocity();
        }
        
        // Update Controller
        APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (PlayerController != nullptr)
        {
            NewRotation = ConvertRotationToActorSpace(PlayerController->GetControlRotation(), this, Target);

            PlayerController->SetControlRotation(NewRotation);
        }

        // Reapply Velocity (Need to reorient direction into local space of Portal)
        {
            FVector Dots;
            Dots.X  = FVector::DotProduct( Velocity, GetActorForwardVector() );
            Dots.Y  = FVector::DotProduct( Velocity, GetActorRightVector() );
            Dots.Z  = FVector::DotProduct( Velocity, GetActorUpVector() );

            FVector NewVelocity     = Dots.X * Target->GetActorForwardVector()
                                    + Dots.Y * Target->GetActorRightVector()
                                    + Dots.Z * Target->GetActorUpVector();

            // TODO THIS IS INCOMPLETE
            //->GetMovementComponent()->Velocity = NewVelocity;
        }
    }
}

bool APortal::IsPointInsideBox( FVector Point, UBoxComponent* Box )
{
    if (Box == nullptr)
    {
        return false;
    }

    // From https://stackoverflow.com/questions/52673935/check-if-3d-point-inside-a-box/52674010

    FVector Center      = Box->GetComponentLocation();
    FVector Half        = Box->GetScaledBoxExtent();
    FVector DirectionX  = Box->GetForwardVector();
    FVector DirectionY  = Box->GetRightVector();
    FVector DirectionZ  = Box->GetUpVector();
    
    //UE_LOG(LogTemp, Warning, TEXT("Center: %s, Half: %s, DirectionX: %s, DirectionY: %s, DirectionZ: %s"), *Center.ToString(), *Half.ToString(),*DirectionX.ToString(),*DirectionY.ToString(),*DirectionZ.ToString());

    FVector Direction   = Point - Center;

    bool IsInside = FMath::Abs( FVector::DotProduct( Direction, DirectionX ) ) <= Half.X &&
                    FMath::Abs( FVector::DotProduct( Direction, DirectionY ) ) <= Half.Y &&
                    FMath::Abs( FVector::DotProduct( Direction, DirectionZ ) ) <= Half.Z;
    
    //UE_LOG(LogTemp, Warning, TEXT("Half: %s, Direction: %s, %s, %s, %s"),*Half.ToString(), *Direction.ToString(),( FMath::Abs( FVector::DotProduct( Direction, DirectionX ) ) <= Half.X ? TEXT("true") : TEXT("false") ),( FMath::Abs( FVector::DotProduct( Direction, DirectionY ) ) <= Half.Y ? TEXT("true") : TEXT("false") ),( FMath::Abs( FVector::DotProduct( Direction, DirectionZ ) ) <= Half.Z ? TEXT("true") : TEXT("false") ));
    //UE_LOG(LogTemp, Warning, TEXT("IsInside: %s"), ( IsInside ? TEXT("true") : TEXT("false") ));

    return IsInside;
}

FVector APortal::ConvertLocationToActorSpace(FVector Location, AActor *ReferenceActor, AActor *TargetActor)
{
    if (ReferenceActor == nullptr || TargetActor == nullptr)
    {
        return FVector::ZeroVector;
    }

    FVector Direction       = Location - ReferenceActor->GetActorLocation();
    FVector TargetLocation  = TargetActor->GetActorLocation();

    FVector Dots;
    Dots.X  = FVector::DotProduct( Direction, ReferenceActor->GetActorForwardVector() );
    Dots.Y  = FVector::DotProduct( Direction, ReferenceActor->GetActorRightVector() );
    Dots.Z  = FVector::DotProduct( Direction, ReferenceActor->GetActorUpVector() );

    FVector NewDirection    = Dots.X * TargetActor->GetActorForwardVector()
                            + Dots.Y * TargetActor->GetActorRightVector()
                            + Dots.Z * TargetActor->GetActorUpVector();

    return TargetLocation + NewDirection;
}

FRotator APortal::ConvertRotationToActorSpace(FRotator Rotation, AActor *ReferenceActor, AActor *TargetActor)
{
    if (ReferenceActor == nullptr || TargetActor == nullptr)
    {
        return FRotator::ZeroRotator;
    }

    FTransform SourceTransform  = ReferenceActor->GetActorTransform();
    FTransform TargetTransform  = TargetActor->GetActorTransform();
    FQuat QuatRotation          = FQuat( Rotation );

    FQuat LocalQuat             = SourceTransform.GetRotation().Inverse() * QuatRotation;
    FQuat NewWorldQuat          = TargetTransform.GetRotation() * LocalQuat;

    return NewWorldQuat.Rotator();
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();

}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if( InputComponent)
	{
		UE_LOG(LogTemp,Warning,TEXT("Input Compent found %s"),*GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this,&UGrabber::Grab);
		InputComponent->BindAction("Grab",IE_Released,this,&UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandle()
{
	// checking for Physics Handle Component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle)
	{

	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("No physics handle commonent found on %s!"),*GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp,Warning,TEXT("Grabber Pressed"));
	// Get players viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotaion;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotaion
		);

	//draw a line from player showing the reach
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotaion.Vector() * Reach;

	// TODO to only raycast when key is pressed and see if we hit something then attach the physics handle.
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	// Try and reach any actors with physics body collision channel set 
	if( PhysicsHandle && HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			LineTraceEnd
		);
	}
	// TODO attach physics handle.
}
void UGrabber::Release()
{
	UE_LOG(LogTemp,Warning,TEXT("Grabber Released"));
	// TODO remove/release the physics handle.
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotaion;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotaion
		);
	//draw a line from player showing the reach
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotaion.Vector() * Reach;

	// If the physic handle is attach.
	if( PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		// Move the object we are holding.
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
		UE_LOG(LogTemp,Warning,TEXT(" run here"));
	}
		


	//GetFirstPhysicsBodyInReach();

}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	// Get players viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotaion;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotaion
		);

	/*UE_LOG(LogTemp,Warning, TEXT("Location:%s, Rotation:%s"),
		*PlayerViewPointLocation.ToString(),
		*PlayerViewPointRotaion.ToString()
		);
	*/
	//draw a line from player showing the reach
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotaion.Vector() * Reach;

	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0,255,0),
		false,
		0.f,
		0,
		5.f
	);

	FHitResult Hit;
	// Ray-cast out to a certain distance
	FCollisionQueryParams TraceParams(FName(TEXT("")),false,GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	// see what it hits
	AActor* ActorHit = Hit.GetActor();

	if( ActorHit)
	{
		UE_LOG(LogTemp,Error,TEXT("hit:%s"),*(ActorHit->GetName()));
	}

	return Hit;
}

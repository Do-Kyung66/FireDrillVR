#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Engine/LocalPlayer.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "FireExtinguisherActor.h"


AVRCharacter::AVRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);

	LeftHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(RootComponent);
	LeftHand->SetTrackingMotionSource(TEXT("Left"));

	RightHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(RootComponent);
	RightHand->SetTrackingMotionSource(TEXT("Right"));

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->InterpolationSpeed = 20.f; 

	ConstructorHelpers::FObjectFinder<UInputMappingContext> TempIMC(TEXT("'/Game/HDK/Inputs/IMC_VRInput.IMC_VRInput'"));
	if (TempIMC.Succeeded())
	{
		IMC_VRInput = TempIMC.Object;

	}

	ConstructorHelpers::FObjectFinder<UInputAction> TempIA_Move(TEXT("'/Game/HDK/Inputs/IA_VRMove.IA_VRMove'"));
	if (TempIA_Move.Succeeded())
	{
		IA_Move = TempIA_Move.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> TempIA_Mouse(TEXT("'/Game/HDK/Inputs/IA_VRMouse.IA_VRMouse'"));
	if (TempIA_Mouse.Succeeded())
	{
		IA_Mouse = TempIA_Mouse.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> TempIA_Grab(TEXT("'/Game/HDK/Inputs/IA_VRGrab.IA_VRGrab'"));
	if (TempIA_Grab.Succeeded())
	{
		IA_Grab = TempIA_Grab.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> TempIA_Spray(TEXT("'/Game/HDK/Inputs/IA_VRSpray.IA_VRSpray'"));
	if (TempIA_Spray.Succeeded())
	{
		IA_Spray = TempIA_Spray.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> TempIA_OpenDoor(TEXT("'/Game/HDK/Inputs/IA_VROpenDoor.IA_VROpenDoor'"));
	if (TempIA_OpenDoor.Succeeded())
	{
		IA_OpenDoor = TempIA_OpenDoor.Object;
	}

}

void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHoldingDoor && PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->SetTargetLocation(FixedGrabLoc);

		const FRotator Cur = PhysicsHandle->GetGrabbedComponent()->GetComponentRotation();
		const FRotator Hand = RightHand->GetComponentRotation();
		const FRotator Smoothed = FMath::RInterpTo(Cur, Hand, DeltaTime, 3.f);
		PhysicsHandle->SetTargetRotation(Smoothed);
	}

}

void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		auto LocalPlayer = PC->GetLocalPlayer();
		auto SS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		if (SS)
		{
			SS->AddMappingContext(IMC_VRInput, 1);
		}

	}

	auto InputSystem = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputSystem)
	{
		InputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRCharacter::Move);
		InputSystem->BindAction(IA_Mouse, ETriggerEvent::Triggered, this, &AVRCharacter::Turn);

		
		InputSystem->BindAction(IA_Grab, ETriggerEvent::Started, this, &AVRCharacter::TryGrab);
		InputSystem->BindAction(IA_Grab, ETriggerEvent::Completed, this, &AVRCharacter::TryUnGrab);

		InputSystem->BindAction(IA_OpenDoor, ETriggerEvent::Started, this, &AVRCharacter::OnDoorInteractStarted);
		InputSystem->BindAction(IA_OpenDoor, ETriggerEvent::Completed, this, &AVRCharacter::OnDoorInteractCompleted);

		InputSystem->BindAction(IA_Spray, ETriggerEvent::Started, this, &AVRCharacter::OnSprayStarted);
		InputSystem->BindAction(IA_Spray, ETriggerEvent::Completed, this, &AVRCharacter::OnSprayCompleted);
	}
}

void AVRCharacter::Move(const struct FInputActionValue& Values)
{
	FVector2d Scale = Values.Get<FVector2d>();
	AddMovementInput(VRCamera->GetForwardVector(), Scale.Y);
	AddMovementInput(VRCamera->GetRightVector(), Scale.X);
}

void AVRCharacter::Turn(const struct FInputActionValue& Values)
{
	FVector2d Scale = Values.Get<FVector2d>();
	AddControllerPitchInput(-Scale.Y);
	AddControllerYawInput(Scale.X);
}

void AVRCharacter::TryGrab(const struct FInputActionValue& Values)
{
	// 소화기를 집는다
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FVector HandPose = LeftHand->GetComponentLocation();
	TArray<FOverlapResult> HitObjects;
	bool bHit = GetWorld()->OverlapMultiByChannel(HitObjects, HandPose, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(GrabRadius), Params);

	if (bHit == false) return;

	for (auto& Hit : HitObjects)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->ActorHasTag(TEXT("FireExtinguisher")))
			{
				UPrimitiveComponent* HitComp = Hit.GetComponent();
				if (HitComp && HitComp->IsSimulatingPhysics())
				{
					grabbedObject = HitComp;
					bIsGrabbing = true;

					grabbedObject->SetSimulatePhysics(false);
					
					grabbedObject->AttachToComponent(LeftHand, FAttachmentTransformRules::KeepWorldTransform, TEXT("GrabSocket"));

					// 앞 방향으로 소화기 고정
					FVector NewLoc = LeftHand->GetComponentLocation() + LeftHand->GetUpVector() * -20.f + LeftHand->GetRightVector() * 20.f;
					FRotator HandRot = LeftHand->GetComponentRotation();
					FRotator OffsetRot = FRotator(0.f, 180.f, 0.f);
					FRotator NewRot = (HandRot.Quaternion() * OffsetRot.Quaternion()).Rotator();
					grabbedObject->SetWorldLocationAndRotation(NewLoc, NewRot);


					UE_LOG(LogTemp, Warning, TEXT("Grab!!!!!!!!!!!!!!!!!"));
					break;
				}
			}
		}
	}

	FColor SphereColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugSphere(
		GetWorld(),
		HandPose,    
		GrabRadius,     
		16,           
		SphereColor, 
		false,
		1.0f,         
		0,            
		0.5f          
	);

}

void AVRCharacter::TryUnGrab(const struct FInputActionValue& Values)
{
	// 소화기를 놓는다
	if (bIsGrabbing == false) return;

	bIsGrabbing = false;

	grabbedObject->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	grabbedObject->SetSimulatePhysics(true);
	grabbedObject->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	grabbedObject = nullptr;
}

void AVRCharacter::OnSprayStarted(const FInputActionValue&)
{
	if(!bIsGrabbing || !grabbedObject) return;
	if (auto* Ext = Cast<AFireExtinguisherActor>(grabbedObject->GetOwner()))
		Ext->StartSpray();

	UE_LOG(LogTemp, Warning, TEXT("Spray Start"));
}

void AVRCharacter::OnSprayCompleted(const FInputActionValue&)
{
	if (!bIsGrabbing || !grabbedObject) return;
	if (auto* Ext = Cast<AFireExtinguisherActor>(grabbedObject->GetOwner()))
		Ext->StopSpray();
}

void AVRCharacter::OnDoorInteractStarted(const FInputActionValue& Values)
{
	if(!RightHand || !PhysicsHandle) return;

	const FVector Start = RightHand->GetComponentLocation();
	const FVector End = Start + RightHand->GetForwardVector() * TraceDistance;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 2.0f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DoorTrace), false, this);
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (!bHit) return;

	AActor* HitActor = Hit.GetActor();
	USceneComponent* PivotComp = nullptr;

	

	if (!HitActor) return;

	const bool bIsDoor = HitActor->ActorHasTag(TEXT("Door"));
	if (!bIsDoor) return;

	if (bIsDoor)
	{
		UPrimitiveComponent* DoorComp = Hit.GetComponent();

		if(!DoorComp || !DoorComp->IsSimulatingPhysics()) return;

		if (!PivotComp)
		{
			PivotComp = Cast<USceneComponent>(HitActor->FindComponentByClass<USceneComponent>());
		}

		const FVector Pivot = PivotComp ? PivotComp->GetComponentLocation() : Hit.ImpactPoint;
		
		PhysicsHandle->GrabComponentAtLocationWithRotation(DoorComp, NAME_None, Pivot, DoorComp->GetComponentRotation());

		FixedGrabLoc = Pivot;
	}

	bHoldingDoor = true;
	UE_LOG(LogTemp, Warning, TEXT("bHoldingDoor True"));
	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 1.0f, 0, 3.0f);
}

void AVRCharacter::OnDoorInteractCompleted(const FInputActionValue& Values)
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		UPrimitiveComponent* Comp = PhysicsHandle->GetGrabbedComponent();
		PhysicsHandle->ReleaseComponent();

		/*Comp->SetSimulatePhysics(false);
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);*/
	}
	bHoldingDoor = false;
}


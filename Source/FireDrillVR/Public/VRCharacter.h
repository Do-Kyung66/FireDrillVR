#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

UCLASS()
class FIREDRILLVR_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVRCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* VRCamera;

	// 모션컨트롤러 등록
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* LeftHand;
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* RightHand;
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* RightAim;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_VRInput;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	
	// 마우스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Mouse;

	// 잡기
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Grab;

	bool bIsGrabbing = false;

	UPROPERTY(EditAnywhere, Category = "Grab")
	float GrabRadius = 100;

	// 잡은 물체 기억할 변수
	UPROPERTY()
	class UPrimitiveComponent* grabbedObject;

public:
	// 함수
	void Move(const struct FInputActionValue& Values);
	void Turn(const struct FInputActionValue& Values);
	void TryGrab(const struct FInputActionValue& Values);
	void TryUnGrab(const struct FInputActionValue& Values);

};

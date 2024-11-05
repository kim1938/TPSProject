// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT22_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



public:
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	// USpringArmComponent 등록된 자식 컴포넌트를 자신과 지정된 거리안에 유지되도록 처리하는 컴포넌트 탄럭젹인 움직임
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	// USpringArmComponent 등록된 자식 컴포넌트를 자신과 지정된 거리안에 유지되도록 처리하는 컴포넌트 탄럭젹인 움직임
	class UCameraComponent* tpsCamComp;

	// 입력 매핑 컨텍스트를 설정합니다. (편집 시 기본값으로 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* imc_TPS;
	// Look Up 동작을 정의하는 입력 액션을 설정합니다. (편집 시 기본값으로 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;
	// Turn 동작을 정의하는 입력 액션을 설정합니다. (편집 시 기본값으로 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;

	// 좌우 회천 입력처리
	void Turn(const struct FInputActionValue& inputValue);
	// 상하 회천 입력처리
	void LookUp(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;

	
	UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
	float walkSpeed = 200;

	// 달리기 속도
	UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
	float runSpeed = 600;


	FVector direction;

	void Move(const struct  FInputActionValue& GetInputVectorAxisValue);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;

	void InputJump(const struct  FInputActionValue& inputValue);
	// 플레이어 이동 처리
	void PlayerMove();

	// 총 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, Category = "GunMesh")
	class USkeletalMeshComponent* gunMeshComp;

	// 총알 공장
	UPROPERTY(EditDefaultsOnly, Category = "BulletFactory")
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;

	//총알 발사 처리 함수
	void InputFire(const struct FInputActionValue& inputValue);

	// 스나이퍼건 스태틱 메쉬 추가
	UPROPERTY(VisibleAnywhere, Category = "GunMesh")
	class UStaticMeshComponent* sniperGunComp;

	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_GrenadeGun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_SniperGun;
	// 유탄 사용중 인지 여부
	bool bUsinGrenadeGun = true;
	// 유탄총 으로 변경
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);
	// 스나로 변경
	void ChangeToSniperGun(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_sniper;

	//스나이퍼 조준 처리함수
	void SniperAim();

	//  조준중인지 여부 확인
	bool bSniperAim = false;

	// 스나이퍼 ui 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	// 스나이퍼 ui 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _sniperUI;

	// 총알 파편 효과 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;


	// 일반 조준 크로스헤어 ui 위젯
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	//크로스 헤어 인스턴스
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	// 달리기 입력
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;
	// 달리기 이벤트 처리 함수
	void InputRun();

	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;


	// 현재체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 hp;
	// 초기 hp 값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 initialHp = 10;

	// 피격 받았을 때 처리
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// 게임오버 될 때 호출될 함수
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Health)
	void OnGameOver();

	// 총 바꿀 때 호출되는 이벤트 함수
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Health)
	void OnUsingGrenade(bool isGrenade);
};



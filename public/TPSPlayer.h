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
	// USpringArmComponent ��ϵ� �ڽ� ������Ʈ�� �ڽŰ� ������ �Ÿ��ȿ� �����ǵ��� ó���ϴ� ������Ʈ ź������ ������
	class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	// USpringArmComponent ��ϵ� �ڽ� ������Ʈ�� �ڽŰ� ������ �Ÿ��ȿ� �����ǵ��� ó���ϴ� ������Ʈ ź������ ������
	class UCameraComponent* tpsCamComp;

	// �Է� ���� ���ؽ�Ʈ�� �����մϴ�. (���� �� �⺻������ ����)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* imc_TPS;
	// Look Up ������ �����ϴ� �Է� �׼��� �����մϴ�. (���� �� �⺻������ ����)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;
	// Turn ������ �����ϴ� �Է� �׼��� �����մϴ�. (���� �� �⺻������ ����)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;

	// �¿� ȸõ �Է�ó��
	void Turn(const struct FInputActionValue& inputValue);
	// ���� ȸõ �Է�ó��
	void LookUp(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;

	
	UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
	float walkSpeed = 200;

	// �޸��� �ӵ�
	UPROPERTY(EditDefaultsOnly, Category = PlayerSetting)
	float runSpeed = 600;


	FVector direction;

	void Move(const struct  FInputActionValue& GetInputVectorAxisValue);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Jump;

	void InputJump(const struct  FInputActionValue& inputValue);
	// �÷��̾� �̵� ó��
	void PlayerMove();

	// �� ���̷�Ż �޽�
	UPROPERTY(VisibleAnywhere, Category = "GunMesh")
	class USkeletalMeshComponent* gunMeshComp;

	// �Ѿ� ����
	UPROPERTY(EditDefaultsOnly, Category = "BulletFactory")
	TSubclassOf<class ABullet> bulletFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;

	//�Ѿ� �߻� ó�� �Լ�
	void InputFire(const struct FInputActionValue& inputValue);

	// �������۰� ����ƽ �޽� �߰�
	UPROPERTY(VisibleAnywhere, Category = "GunMesh")
	class UStaticMeshComponent* sniperGunComp;

	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_GrenadeGun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_SniperGun;
	// ��ź ����� ���� ����
	bool bUsinGrenadeGun = true;
	// ��ź�� ���� ����
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);
	// ������ ����
	void ChangeToSniperGun(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_sniper;

	//�������� ���� ó���Լ�
	void SniperAim();

	//  ���������� ���� Ȯ��
	bool bSniperAim = false;

	// �������� ui ���� ����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	// �������� ui ���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _sniperUI;

	// �Ѿ� ���� ȿ�� ����
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect)
	class UParticleSystem* bulletEffectFactory;


	// �Ϲ� ���� ũ�ν���� ui ����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	//ũ�ν� ��� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	// �޸��� �Է�
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;
	// �޸��� �̺�Ʈ ó�� �Լ�
	void InputRun();

	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
	TSubclassOf<class UCameraShakeBase> cameraShake;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* bulletSound;


	// ����ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 hp;
	// �ʱ� hp ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	int32 initialHp = 10;

	// �ǰ� �޾��� �� ó��
	UFUNCTION(BlueprintCallable, Category = Health)
	void OnHitEvent();

	// ���ӿ��� �� �� ȣ��� �Լ�
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Health)
	void OnGameOver();

	// �� �ٲ� �� ȣ��Ǵ� �̺�Ʈ �Լ�
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Health)
	void OnUsingGrenade(bool isGrenade);
};



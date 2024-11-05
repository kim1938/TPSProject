// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h> // SpringArmComponent�� ����ϱ� ���� ��� ���� ����
#include <Camera/CameraComponent.h>           // CameraComponent�� ����ϱ� ���� ��� ���� ����
#include "EnhancedInputSubsystems.h"          // Enhanced Input �ý����� ����ϱ� ���� ��� ���� ����
#include "EnhancedInputComponent.h"           // Enhanced Input ������Ʈ�� ����ϱ� ���� ��� ���� ����
#include "InputActionValue.h"                 // �Է� �׼��� ���� ó���ϱ� ���� ��� ���� ����
#include "Bullet.h"                 // �߻� �׼��� ���� ó���ϱ� ���� ��� ���� ����
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerAnim.h"
#include "PlayerMove.h"
#include "TPSProject22.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// �� ĳ���Ͱ� �� �����Ӹ��� Tick() �Լ��� ȣ���ϵ��� �����մϴ�.
	// ���� ����� �ʿ��� ��� �� �ɼ��� �� �� �ֽ��ϴ�.
	PrimaryActorTick.bCanEverTick = true;

	// 1. ���̷�Ż�޽� �����͸� �ҷ����� �ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	
	// �޽� �ε忡 �������� ���, �޽� ������Ʈ�� ���̷�Ż �޽ø� �����մϴ�.
	if (TempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// �޽� ������Ʈ�� ��� ��ġ�� ȸ�� ���� �����մϴ�.
		// ĳ������ ��ġ�� �����Ͽ� ���鿡 �°� ��ġ�ϰ�, ȸ������ �����մϴ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// TPS(Third-Person Shooter) ī�޶� ĳ���Ϳ� �����ϱ� ����
	// SpringArm ������Ʈ�� �����ϰ� �����մϴ�.

	// SpringArm ������Ʈ�� �����ϰ�, "SpringArmComp"��� �̸��� �ο��մϴ�.
	// SpringArm�� ī�޶�� ĳ���� ������ �Ÿ��� �����ϸ� ȸ���ϴ� ������ �մϴ�.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	// SpringArm�� ĳ������ ��Ʈ ������Ʈ�� �����մϴ�.
	// �̷��� �ϸ� SpringArm�� ĳ���Ϳ� �Բ� �����Դϴ�.
	springArmComp->SetupAttachment(RootComponent);

	// SpringArm�� ��� ��ġ�� �����մϴ�.
	// ĳ������ ���� 70 ����, ���� 90 ���� ��ġ�� ī�޶� ��ġ�ϵ��� �մϴ�.
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));

	// ī�޶�� ĳ���� ������ �Ÿ��� �����մϴ�.
	// TargetArmLength ���� 400 ��������, ī�޶� ĳ���ͷκ��� ������ �Ÿ��� �ǹ��մϴ�.
	springArmComp->TargetArmLength = 400;

	// SpringArm�� ĳ������ ȸ���� ���� ȸ���ϵ��� �����մϴ�.
	// ��, �÷��̾ ���콺�� �����̸� ī�޶� �Բ� ȸ���մϴ�.
	springArmComp->bUsePawnControlRotation = true;

	// TPS ī�޶� ������Ʈ�� �����ϰ�, SpringArm�� �����մϴ�.
	// �� ī�޶�� �÷��̾ ���� ������ �����մϴ�.
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);

	// TPS ī�޶� SpringArm�� ȸ���� ������ �ʵ��� �����մϴ�.
	// SpringArm�� ȸ���� �� ī�޶� �Բ� ȸ������ �ʵ��� �����մϴ�.
	tpsCamComp->bUsePawnControlRotation = false;

	// ĳ���Ͱ� Yaw ��(������)�� �������� ȸ���ϵ��� �����մϴ�.
	// ���콺�� �¿� �����ӿ� ���� ĳ���Ͱ� ȸ���ϰ� �˴ϴ�.
	bUseControllerRotationYaw = true;

	// 2�� ����
	JumpMaxCount = 2;

	//4. �� ���̷��� �޽� ������Ʈ ���
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	//4.1 �θ� ������Ʈ�� �޽� ������Ʈ�� ����
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocKet"));
	//4.2 ���̷�Ż �޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	//4.3 ������ �ε尡 �����ߴٸ�
	if(TempGunMesh.Succeeded()) {
		//4.4���̷��� �޽� ������ �Ҵ�
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		//4,5 ��ġ �����ϱ�
		gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));

	}

	//5.�������� �� ������Ʈ ���
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	//5.1 �θ� ������Ʈ�� �޽� ������Ʈ�� ����
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocKet"));
	// ���̷�Ż �޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// ������ �ε尡 �����ߴٸ�
	if (TempSniperMesh.Succeeded()) {
		// �޽� ������ �Ҵ�
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// ��ġ �����ϱ�
		sniperGunComp->SetRelativeLocation(FVector(-42, 7, 1));
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));
		// ũ�� ����
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (tempSound.Succeeded()) {
		bulletSound = tempSound.Object;
	}


}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	hp = initialHp;

	//�ʱ� �ӵ��� �ȱ�� ����
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	// ���� ĳ������ ��Ʈ�ѷ��� APlayerController�� ĳ�����մϴ�.
	// APlayerController�� �÷��̾ �����ϴ� ĳ������ �Է��� ó���ϴ� ������ �մϴ�.
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		// Enhanced Input �ý����� ���� �÷��̾� ����ý��ۿ��� �����ɴϴ�.
		// ����ý����� �Է� ���� ���ؽ�Ʈ�� �����մϴ�.
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			// ����ý��ۿ� TPS �Է� ���� ���ؽ�Ʈ�� �߰��մϴ�.
			// �� ���ؽ�Ʈ�� Ư�� �Է¿� ���� ���ǵ� �ൿ���� ���ε��մϴ�.
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	// 1. �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. �Ϲ� ���� ui ũ�ν���� �ν��Ͻ� ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. �Ϲ� ���� ui ���
	_crosshairUI->AddToViewport();

	// �⺻���� �������� ����ϵ��ϼ���
	ChangeToSniperGun(FInputActionValue());
}

void ATPSPlayer::OnHitEvent()
{
	//PRINT_LOG(TEXT("Damaged !!!!!"));
	hp--;
	if (hp <= 0)
	{
		//PRINT_LOG(TEXT("Player is dead!"));
		OnGameOver();
	}
}

void ATPSPlayer::OnGameOver_Implementation()
{
	// ���ӿ����� �Ͻ�����
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	PlayerMove();

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input ������Ʈ�� ĳ�����մϴ�.
	// EnhancedInputComponent�� ���Ӱ� ��ȭ�� �Է� �ý��ۿ��� ����ϴ� ������Ʈ�Դϴ�.
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		// �÷��̾� �Է¿� ���� �ൿ�� ���ε��մϴ�.
		// ���� ���, Turn�� LookUp �Լ��� Ư�� �Է��� Ʈ���ŵ� �� ȣ��ǵ��� �����մϴ�.

		// 'ia_Turn' �׼ǿ� ���� Ʈ���� �̺�Ʈ�� �߻��� �� 'Turn' �Լ��� ȣ��ǵ��� ���ε��մϴ�.
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);

		// 'ia_LookUp' �׼ǿ� ���� Ʈ���� �̺�Ʈ�� �߻��� �� 'LookUp' �Լ��� ȣ��ǵ��� ���ε��մϴ�.
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);

		// 'ia_Move' �׼ǿ� ���� Ʈ���� �̺�Ʈ�� �߻��� �� 'Move' �Լ��� ȣ��ǵ��� ���ε��մϴ�.
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);

		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);

		//�Ѿ� �߻� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);

		//�� ��ü �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);

		// �������� ���� ��� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_sniper, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_sniper, ETriggerEvent::Completed , this, &ATPSPlayer::SniperAim);

		// �޸��� �Է� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);

	}
}

void ATPSPlayer::InputRun()
{
	

	auto movement = GetCharacterMovement();
	// ���� �޸��� �����
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// �ȱ� �ӵ��� ��ȯ
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

void ATPSPlayer::SniperAim()
{
	// �⺻���� ������尡 �ƴ϶�� ó������ �ʴ´�
	if (bUsinGrenadeGun) {
		return;
	}
	// pressed �Է� ó��
	if (bSniperAim == false) {
		// ���� ��� Ȱ��
		bSniperAim = true;
		// ���� ui ���
		_sniperUI->AddToViewport();
		// �þ߰� ����
		tpsCamComp->SetFieldOfView(45.0f);
		//�Ϲ� ���� ��� ����
		_crosshairUI->RemoveFromViewport();
	}
	else {
		// ���� ��� ��Ȱ��
		bSniperAim = false;
		// ���� ui ����
		_sniperUI->RemoveFromViewport();
		// �þ߰� ����
		tpsCamComp->SetFieldOfView(90.0f);
		// �Ϲ� ���� ��� ����
		_crosshairUI->AddToViewport();
	}
}


void ATPSPlayer::InputFire(const struct FInputActionValue& inputValue)
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);


	// ī�޶� ����ũ ���
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);


	// ���ݾִ� ���̼� ���
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	// ��ź ����
	if(bUsinGrenadeGun) {
		// �Ѿ� �߻� ó��
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else {
		//�������� ����
		FVector startPos = tpsCamComp->GetComponentLocation();
		// ���� Ʈ���̽� ���� ��ġ
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// linetrace �� �浹 ������ ���� ����
		FHitResult hitInfo;
		// �浹 �ɼ� ���� ����
		FCollisionQueryParams params;
		// �ڱ� �ڽ��� �浹���� ����
		params.AddIgnoredActor(this);
		// ü�� ���͸� �̿��� lientrace �浹 ����( �浹 ����, ������ġ, ������ġ, �����³� ,�浹�ɼ�)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		//linetrace�� �ε�������
		if (bHit) {
			// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
			FTransform bulletTrans;
			// �ε��� ��ġ �Ҵ�
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// �Ѿ� ���� ȿ�� �ν��Ͻ� ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
			
			auto hitComp = hitInfo.GetComponent();
			// ���� ������Ʈ�� ������ ����Ǿ� �ִٸ�
			if (hitComp&&hitComp->IsSimulatingPhysics()) {
				// ������ ���� �ʿ�
				FVector dir = (endPos = startPos).GetSafeNormal();
				// ���� ���� �� (f=ma)
				FVector force = dir * hitComp->GetMass() * 500000;
				// �� �ݴ� �������� ���� ������ �ʹ�
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			//�ε��� ����� ������ �Ǵ�
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy) {
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamegeProcess();
			}
		}

	}
};

void ATPSPlayer::PlayerMove()
{


	// �� �����Ӹ��� ����� �ڵ尡 ���� ��� ���⿡ �߰��մϴ�.
	// ĳ������ �̵� ������ ���� ��ǥ�迡�� ���� ��ǥ��� ��ȯ�մϴ�.
	direction = FTransform(GetControlRotation()).TransformVector(direction);

	//// ���� ĳ������ ��ġ�� �����ɴϴ�.
	//FVector P0 = GetActorLocation();

	//// ���� ���Ϳ� �̵� �ӵ��� DeltaTime�� ���Ͽ� �̵� ���͸� ����մϴ�.
	//FVector vt = direction * walkSpeed * DeltaTime;

	//// ���� ��ġ�� �̵� ���͸� ���Ͽ� ���ο� ��ġ�� ����մϴ�.
	//FVector P = P0 + vt;

	//// ���� ��ġ�� ĳ���͸� �̵���ŵ�ϴ�.
	//SetActorLocation(P);

	AddMovementInput(direction);
	// �̵��� ������ ���� ���͸� �ʱ�ȭ�Ͽ� ���� �����ӿ��� �̵��� ������ �����մϴ�.
	direction = FVector::ZeroVector;


}

void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
{
	Jump();
}


// 'Move' �Լ��� ĳ������ �̵��� ó���մϴ�.
void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	// �Էµ� ��(value)�� FVector2D �������� ��ȯ�մϴ�.
	// FVector2D�� 2D ���ͷ�, X�� ������/�ڷ� �̵�, Y�� �¿� �̵��� �ǹ��մϴ�.
	FVector2D value = inputValue.Get<FVector2D>();

	// ���� ���͸� ������Ʈ�մϴ�.
	// X���� ������/�ڷ� �̵�, Y���� �¿� �̵��� ó���մϴ�.
	direction.X = value.X;
	direction.Y = value.Y;
}

// 'Turn' �Լ��� Yaw ���� �������� ĳ���͸� ȸ����Ű�� �Լ��Դϴ�.
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	// �Էµ� ���� float �������� ��ȯ�մϴ�.
	float value = inputValue.Get<float>();

	// Yaw ȸ�� �Է¿� ��ȯ�� ���� �߰��մϴ�.
	// �� ���� ĳ������ �¿� ȸ���� �����մϴ�.
	AddControllerYawInput(value);
}

// 'LookUp' �Լ��� Pitch ���� �������� ī�޶� ȸ����Ű�� �Լ��Դϴ�.
void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	// �Էµ� ���� float �������� ��ȯ�մϴ�.
	float value = inputValue.Get<float>();

	// Pitch ȸ�� �Է¿� ��ȯ�� ���� �߰��մϴ�.
	// �� ���� ī�޶��� ���Ʒ� ȸ���� �����մϴ�.
	AddControllerPitchInput(value);
}

void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	bUsinGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
	// ��ź�� ������� ���� ����
	//me->OnUsingGrenade(bUsinGrenadeGun);
}

void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsinGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
	// ��ź�� ������� ���� ����
	//me->OnUsingGrenade(bUsinGrenadeGun);
}

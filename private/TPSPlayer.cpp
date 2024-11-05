// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h> // SpringArmComponent를 사용하기 위한 헤더 파일 포함
#include <Camera/CameraComponent.h>           // CameraComponent를 사용하기 위한 헤더 파일 포함
#include "EnhancedInputSubsystems.h"          // Enhanced Input 시스템을 사용하기 위한 헤더 파일 포함
#include "EnhancedInputComponent.h"           // Enhanced Input 컴포넌트를 사용하기 위한 헤더 파일 포함
#include "InputActionValue.h"                 // 입력 액션의 값을 처리하기 위한 헤더 파일 포함
#include "Bullet.h"                 // 발사 액션의 값을 처리하기 위한 헤더 파일 포함
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
	// 이 캐릭터가 매 프레임마다 Tick() 함수를 호출하도록 설정합니다.
	// 성능 향상이 필요할 경우 이 옵션을 끌 수 있습니다.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 스켈레탈메시 데이터를 불러오고 싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	
	// 메시 로드에 성공했을 경우, 메시 컴포넌트에 스켈레탈 메시를 설정합니다.
	if (TempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// 메쉬 컴포넌트의 상대 위치와 회전 값을 설정합니다.
		// 캐릭터의 위치를 조정하여 지면에 맞게 배치하고, 회전값을 설정합니다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// TPS(Third-Person Shooter) 카메라를 캐릭터에 부착하기 위해
	// SpringArm 컴포넌트를 생성하고 설정합니다.

	// SpringArm 컴포넌트를 생성하고, "SpringArmComp"라는 이름을 부여합니다.
	// SpringArm은 카메라와 캐릭터 사이의 거리를 유지하며 회전하는 역할을 합니다.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	// SpringArm을 캐릭터의 루트 컴포넌트에 부착합니다.
	// 이렇게 하면 SpringArm이 캐릭터와 함께 움직입니다.
	springArmComp->SetupAttachment(RootComponent);

	// SpringArm의 상대 위치를 설정합니다.
	// 캐릭터의 앞쪽 70 유닛, 위쪽 90 유닛 위치에 카메라를 배치하도록 합니다.
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));

	// 카메라와 캐릭터 사이의 거리를 설정합니다.
	// TargetArmLength 값은 400 유닛으로, 카메라가 캐릭터로부터 떨어진 거리를 의미합니다.
	springArmComp->TargetArmLength = 400;

	// SpringArm이 캐릭터의 회전에 따라 회전하도록 설정합니다.
	// 즉, 플레이어가 마우스를 움직이면 카메라도 함께 회전합니다.
	springArmComp->bUsePawnControlRotation = true;

	// TPS 카메라 컴포넌트를 생성하고, SpringArm에 부착합니다.
	// 이 카메라는 플레이어가 보는 시점을 결정합니다.
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);

	// TPS 카메라가 SpringArm의 회전을 따르지 않도록 설정합니다.
	// SpringArm이 회전할 때 카메라가 함께 회전하지 않도록 설정합니다.
	tpsCamComp->bUsePawnControlRotation = false;

	// 캐릭터가 Yaw 축(수직축)을 기준으로 회전하도록 설정합니다.
	// 마우스의 좌우 움직임에 따라 캐릭터가 회전하게 됩니다.
	bUseControllerRotationYaw = true;

	// 2단 점프
	JumpMaxCount = 2;

	//4. 총 스켈레톤 메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	//4.1 부모 컴포넌트를 메쉬 컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocKet"));
	//4.2 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	//4.3 데이터 로드가 성공했다면
	if(TempGunMesh.Succeeded()) {
		//4.4스켈레톤 메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		//4,5 위치 조정하기
		gunMeshComp->SetRelativeLocation(FVector(-17, 10, -3));
		gunMeshComp->SetRelativeRotation(FRotator(0, 90, 0));

	}

	//5.스나이퍼 건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	//5.1 부모 컴포넌트를 메쉬 컴포넌트로 설정
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocKet"));
	// 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 데이터 로드가 성공했다면
	if (TempSniperMesh.Succeeded()) {
		// 메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		// 위치 조정하기
		sniperGunComp->SetRelativeLocation(FVector(-42, 7, 1));
		sniperGunComp->SetRelativeRotation(FRotator(0, 90, 0));
		// 크기 조절
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

	//초기 속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	// 현재 캐릭터의 컨트롤러를 APlayerController로 캐스팅합니다.
	// APlayerController는 플레이어가 조종하는 캐릭터의 입력을 처리하는 역할을 합니다.
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		// Enhanced Input 시스템을 로컬 플레이어 서브시스템에서 가져옵니다.
		// 서브시스템은 입력 매핑 컨텍스트를 관리합니다.
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			// 서브시스템에 TPS 입력 매핑 컨텍스트를 추가합니다.
			// 이 컨텍스트는 특정 입력에 대해 정의된 행동들을 바인딩합니다.
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	// 1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 2. 일반 조준 ui 크로스헤어 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);

	// 3. 일반 조준 ui 등록
	_crosshairUI->AddToViewport();

	// 기본으로 스나이퍼 사용하도록설정
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
	// 게임오버시 일시정지
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

	// Enhanced Input 컴포넌트로 캐스팅합니다.
	// EnhancedInputComponent는 새롭게 강화된 입력 시스템에서 사용하는 컴포넌트입니다.
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		// 플레이어 입력에 대한 행동을 바인딩합니다.
		// 예를 들어, Turn과 LookUp 함수에 특정 입력이 트리거될 때 호출되도록 설정합니다.

		// 'ia_Turn' 액션에 대해 트리거 이벤트가 발생할 때 'Turn' 함수가 호출되도록 바인딩합니다.
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);

		// 'ia_LookUp' 액션에 대해 트리거 이벤트가 발생할 때 'LookUp' 함수가 호출되도록 바인딩합니다.
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);

		// 'ia_Move' 액션에 대해 트리거 이벤트가 발생할 때 'Move' 함수가 호출되도록 바인딩합니다.
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);

		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);

		//총알 발사 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);

		//총 교체 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);

		// 스나이퍼 조준 모드 이벤트 처림 함수 바인딩
		PlayerInput->BindAction(ia_sniper, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_sniper, ETriggerEvent::Completed , this, &ATPSPlayer::SniperAim);

		// 달리기 입력 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);

	}
}

void ATPSPlayer::InputRun()
{
	

	auto movement = GetCharacterMovement();
	// 현재 달리기 모드라면
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		// 걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	}
	else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

void ATPSPlayer::SniperAim()
{
	// 기본으로 스나모드가 아니라면 처리하지 않는다
	if (bUsinGrenadeGun) {
		return;
	}
	// pressed 입력 처리
	if (bSniperAim == false) {
		// 조준 모드 활성
		bSniperAim = true;
		// 조준 ui 등록
		_sniperUI->AddToViewport();
		// 시야각 설정
		tpsCamComp->SetFieldOfView(45.0f);
		//일반 조준 모드 제거
		_crosshairUI->RemoveFromViewport();
	}
	else {
		// 조준 모드 비활성
		bSniperAim = false;
		// 조준 ui 제거
		_sniperUI->RemoveFromViewport();
		// 시야각 조정
		tpsCamComp->SetFieldOfView(90.0f);
		// 일반 조준 모드 설정
		_crosshairUI->AddToViewport();
	}
}


void ATPSPlayer::InputFire(const struct FInputActionValue& inputValue)
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);


	// 카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);


	// 공격애니 메이션 재생
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	// 유탄 사용시
	if(bUsinGrenadeGun) {
		// 총알 발사 처리
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else {
		//스나이퍼 사용시
		FVector startPos = tpsCamComp->GetComponentLocation();
		// 라인 트레이스 종료 위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		// linetrace 의 충돌 변수를 담은 변수
		FHitResult hitInfo;
		// 충돌 옵션 설정 변수
		FCollisionQueryParams params;
		// 자기 자신은 충돌에서 제외
		params.AddIgnoredActor(this);
		// 체널 필터를 이용한 lientrace 충돌 검출( 충돌 정보, 시작위치, 종료위치, 검출태널 ,충돌옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		//linetrace가 부딪혓을때
		if (bHit) {
			// 충돌 처리 -> 총알 파편 효과 재생
			FTransform bulletTrans;
			// 부딧힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			// 총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
			
			auto hitComp = hitInfo.GetComponent();
			// 만약 컴포넌트에 물리가 적용되어 있다면
			if (hitComp&&hitComp->IsSimulatingPhysics()) {
				// 조준한 방향 필요
				FVector dir = (endPos = startPos).GetSafeNormal();
				// 날려 버릴 힘 (f=ma)
				FVector force = dir * hitComp->GetMass() * 500000;
				// 그 반대 방향으로 날려 보내고 싶다
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			//부딧힌 대상이 적인지 판단
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


	// 매 프레임마다 실행될 코드가 있을 경우 여기에 추가합니다.
	// 캐릭터의 이동 방향을 월드 좌표계에서 로컬 좌표계로 변환합니다.
	direction = FTransform(GetControlRotation()).TransformVector(direction);

	//// 현재 캐릭터의 위치를 가져옵니다.
	//FVector P0 = GetActorLocation();

	//// 방향 벡터에 이동 속도와 DeltaTime을 곱하여 이동 벡터를 계산합니다.
	//FVector vt = direction * walkSpeed * DeltaTime;

	//// 현재 위치에 이동 벡터를 더하여 새로운 위치를 계산합니다.
	//FVector P = P0 + vt;

	//// 계산된 위치로 캐릭터를 이동시킵니다.
	//SetActorLocation(P);

	AddMovementInput(direction);
	// 이동이 끝나면 방향 벡터를 초기화하여 다음 프레임에서 이동이 없도록 설정합니다.
	direction = FVector::ZeroVector;


}

void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
{
	Jump();
}


// 'Move' 함수는 캐릭터의 이동을 처리합니다.
void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	// 입력된 값(value)을 FVector2D 형식으로 변환합니다.
	// FVector2D는 2D 벡터로, X는 앞으로/뒤로 이동, Y는 좌우 이동을 의미합니다.
	FVector2D value = inputValue.Get<FVector2D>();

	// 방향 벡터를 업데이트합니다.
	// X축은 앞으로/뒤로 이동, Y축은 좌우 이동을 처리합니다.
	direction.X = value.X;
	direction.Y = value.Y;
}

// 'Turn' 함수는 Yaw 축을 기준으로 캐릭터를 회전시키는 함수입니다.
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	// 입력된 값을 float 형식으로 변환합니다.
	float value = inputValue.Get<float>();

	// Yaw 회전 입력에 변환된 값을 추가합니다.
	// 이 값은 캐릭터의 좌우 회전을 제어합니다.
	AddControllerYawInput(value);
}

// 'LookUp' 함수는 Pitch 축을 기준으로 카메라를 회전시키는 함수입니다.
void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	// 입력된 값을 float 형식으로 변환합니다.
	float value = inputValue.Get<float>();

	// Pitch 회전 입력에 변환된 값을 추가합니다.
	// 이 값은 카메라의 위아래 회전을 제어합니다.
	AddControllerPitchInput(value);
}

void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	bUsinGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
	// 유탄총 사용할지 여부 전달
	//me->OnUsingGrenade(bUsinGrenadeGun);
}

void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsinGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
	// 유탄총 사용할지 여부 전달
	//me->OnUsingGrenade(bUsinGrenadeGun);
}

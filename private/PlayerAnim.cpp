// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"
#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconde)
{

	Super::NativeUpdateAnimation(DeltaSeconde);

	// 플레이어 이동속도를 가져와 speed 에할당 하고 싶다
	// 1 소유 폰 얻기
	auto ownerPawn = TryGetPawnOwner();
	// 2 플레이어로 캐스팅 하기
	auto player = Cast<ATPSPlayer>(ownerPawn);
	// 캐스팅 성공시
	if (player) {

		// 3 이동 속도 가 필요
		FVector velocity = player->GetVelocity();
		// 4 플레이어의 전방 벡터가 필요
		FVector forwardVector = player->GetActorForwardVector();
		// 5 speed에 값 할당하기
		speed = FVector::DotProduct(forwardVector, velocity);
		// 6 좌우 속도 할당하기
		FVector rightVector = player->GetActorRightVector();
		direction = FVector::DotProduct(rightVector, velocity);


		// 플레이어가 현재 공주에 있는지 여부를 기억하고싶다
		auto movement = player->GetCharacterMovement();
		inInAir = movement->IsFalling();

	}
}

void UPlayerAnim::PlayAttackAnim() {

	Montage_Play(attackAnimMontage);

}


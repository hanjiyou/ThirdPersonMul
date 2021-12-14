// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMPProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h" //用于访问基本游戏进程的函数
#include "UObject/ConstructorHelpers.h" //提供一些有用的构造函数，以便设置组件


// Sets default values
AThirdPersonMPProjectile::AThirdPersonMPProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	//球体碰撞组件
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(37.5f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	if (GetLocalRole() == ROLE_Authority) {
		SphereComponent->OnComponentHit.AddDynamic(this, &AThirdPersonMPProjectile::OnProjectileImpact);
	}
	RootComponent = SphereComponent;
	//静态网格组件
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshName"));
	StaticMesh->SetupAttachment(RootComponent);
	if (DefaultMesh.Succeeded()) {
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0, 0, -35.5f));
		StaticMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	}
	//爆炸特效
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded()) {
		ParticleSystem = DefaultExplosionEffect.Object;
	}
	//子弹移动组件
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	//伤害
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

// Called when the game starts or when spawned
void AThirdPersonMPProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThirdPersonMPProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThirdPersonMPProjectile::Destroyed()
{
	FVector spawnEffectLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ParticleSystem, spawnEffectLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

}

void AThirdPersonMPProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor) {
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
	}
	Destroyed();
}


# Aura 项目架构快照（截至第 19 章基线）

> 本文档记录 `Aura_5_8` 当前已实现的 C++ 与核心蓝图架构，作为双 AI（Antigravity / GPT）查阅代码结构的快速地图。

---

## 1. 技术栈与模块依赖

- **引擎版本**：Unreal Engine 5.8
- **模块名**：`Aura`（[Aura.Build.cs](../Source/Aura/Aura.Build.cs)）
- **Public 依赖**：`Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `GameplayAbilities`
- **Private 依赖**：`GameplayTags`, `GameplayTasks`, `NavigationSystem`, `Niagara`, `AIModule`
- **关键启用插件**：`GameplayAbilities`, `MotionWarping`, `ModelContextProtocol`, `LiveCodingToolset`, `MVVMToolset`
- **AssetManager**：`UAuraAssetManager`（在 `DefaultEngine.ini` 注册，启动时调用 `FAuraGameplayTags::InitializeNativeGameplayTags()` 与 `UAbilitySystemGlobals::Get().InitGlobalData()`）
- **自定义 EffectContext**：`UAuraAbilitySystemGlobals::AllocGameplayEffectContext()` 返回 `FAuraGameplayEffectContext`（扩展 `bIsBlockedHit`、`bIsCriticalHit` 并实现自定义 `NetSerialize`）。

---

## 2. 所有权与网络复制拓扑

| 实体 | 持有组件 | 复制模式 | 初始化时机 | 说明 |
| :--- | :--- | :--- | :--- | :--- |
| **玩家 (`AAuraCharacter`)** | `ASC` 与 `AttributeSet` 挂在 `AAuraPlayerState` | `Mixed`（`NetUpdateFrequency = 100`） | 服务端 `PossessedBy` + 客户端 `OnRep_PlayerState` 均调用 `InitAbilityActorInfo()` | `OwnerActor = PlayerState`，`AvatarActor = AuraCharacter`；仅服务端 `PossessedBy` 赋予初始技能；本地客户端在 `InitAbilityActorInfo` 初始化 `AAuraHUD::InitOverlay` |
| **敌人 (`AAuraEnemy`)** | `ASC` 与 `AttributeSet` 直接挂在 `AAuraEnemy` | `Minimal` | `BeginPlay` 调用 `InitAbilityActorInfo()` | `OwnerActor = AvatarActor = AuraEnemy`；仅服务端初始化默认属性（通过 `CharacterClassInfo`）与运行 `BehaviorTree` |
| **玩家控制器 (`AAuraPlayerController`)** | `SplineComponent`、EnhancedInput | `bReplicates = true` | `BeginPlay` / `SetupInputComponent` | 负责 `CursorTrace` 高亮敌人、点地自动寻路（`AutoRun`）、按 `Shift` 或指向敌人时触发 `LMB` 技能；`Client, Reliable` RPC `ShowDamageNumber` 显示飘字 |
| **投射物 (`AAuraProjectile`)** | `SphereComponent` + `ProjectileMovement` | Actor Replicated | 服务端生成并携带 `DamageEffectSpecHandle` | 命中重叠后仅服务端应用 GE 并销毁；通过 `bHit` 与 `Destroyed()` 保证客户端补播命中特效/音效 |

---

## 3. 核心 GAS 数据流

### 3.1 技能输入与激活流
```text
EnhancedInput (IA_LMB / IA_1..4 + UAuraInputConfig)
  -> AAuraPlayerController (AbilityInputTagPressed / Held / Released)
    -> UAuraAbilitySystemComponent (匹配 DynamicSpecSourceTags 中的 InputTag)
      -> TryActivateAbility() / AbilitySpecInputPressed / Released
        -> UAuraGameplayAbility (如 UAuraProjectileSpell / UAuraMeleeAttack / UAuraSummonAbility)
```

### 3.2 伤害计算与受击反馈流
```text
UAuraDamageGameplayAbility::CauseDamage / AAuraProjectile::OnSphereOverlap
  -> 通过 SetByCaller 写入 Damage.* 伤害值 (TMap<FGameplayTag, FScalableFloat>)
  -> 应用 GE_Damage (挂载 UExecCalc_Damage)
    -> UExecCalc_Damage::Execute_Implementation:
         1. 遍历 DamageTypesToResistances 计算抗性减免
         2. 判定 BlockChance（成功则伤害减半，写入 EffectContext.bIsBlockedHit）
         3. 结合 CurveTable (DamageCalculationCoefficients) 计算 ArmorPenetration 与 EffectiveArmor 减伤
         4. 计算 CriticalHitChance vs CriticalHitResistance（暴击则 2x + CriticalHitDamage，写入 bIsCriticalHit）
         5. 输出 Additive 到 Meta Attribute: IncomingDamage
  -> UAuraAttributeSet::PostGameplayEffectExecute():
       - 消费 IncomingDamage -> 扣减并 Clamp Health
       - 若 Health <= 0 -> ICombatInterface::Die() -> MulticastHandleDeath (布娃娃 + 溶解材质)
       - 若存活 -> TargetASC->TryActivateAbilitiesByTag(Effects.HitReact)
       - 调用 ShowFloatingText -> PC->ShowDamageNumber (Client RPC)
```

### 3.3 UI 解耦流（MVC / WidgetController）
```text
UAuraAttributeSet / UAuraAbilitySystemComponent (AttributeValueChangeDelegate / EffectAssetTags)
  -> UOverlayWidgetController / UAttributeMenuWidgetController (由 AAuraHUD 懒加载单例管理)
    -> Dynamic Multicast Delegates (OnHealthChange, AttributeInfoDelegate, MessageWidgetRowDelegate)
      -> UAuraUserWidget 子类 (WBP_Overlay, WBP_AttributeMenu 等蓝图监听刷新)
```
> 敌人头顶血条（`WBP_EnemyHealthBar`）直接将 `AAuraEnemy` 作为 `WidgetController` 传入，监听 `AAuraEnemy::OnHealthChanged / OnMaxHealthChanged`。

---

## 4. 属性与 Native GameplayTags 清单

### 4.1 `UAuraAttributeSet` 属性分组
- **Primary（主属性，Replicated）**：`Strength`, `Intelligence`, `Resilience`, `Vigor`
- **Secondary（次级属性，Replicated，由 Infinite GE + MMC 派生）**：`Armor`, `ArmorPenetration`, `BlockChance`, `CriticalHitChance`, `CriticalHitDamage`, `CriticalHitResistance`, `HealthRegeneration`, `ManaRegeneration`, `MaxHealth`（`UMMC_MaxHealth`）, `MaxMana`（`UMMC_MaxMana`）
- **Resistance（抗性，Replicated）**：`FireResistance`, `LightningResistance`, `ArcaneResistance`, `PhysicalResistance`
- **Vital（生命/法力，Replicated）**：`Health`, `Mana`
- **Meta（仅服务端结算，不复制）**：`IncomingDamage`
- **Tag 映射表**：`TagsToAttributes`（`TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>>`），用于 `UAttributeMenuWidgetController` 自动化遍历广播。

### 4.2 GameplayTags 清单
- **Native Tags（`FAuraGameplayTags`，C++ 注册）**：
  - `Attributes.Primary.*` (`Strength`, `Intelligence`, `Resilience`, `Vigor`)
  - `Attributes.Secondary.*` (`Armor`, `ArmorPenetration`, `BlockChance`, `CriticalHitChance`, `CriticalHitDamage`, `CriticalHitResistance`, `HealthRegeneration`, `ManaRegeneration`, `MaxHealth`, `MaxMana`)
  - `Attributes.Resistance.*` (`Fire`, `Lightning`, `Arcane`, `Physical`)
  - `InputTag.*` (`LMB`, `RMB`, `1`, `2`, `3`, `4`)
  - `Damage`, `Damage.Fire`, `Damage.Lightning`, `Damage.Arcane`, `Damage.Physical`（已建立 `DamageTypesToResistances` 映射）
  - `Effects.HitReact`（C++ 成员名 `Effects_HitReact`）
  - `Abilities.Attack`, `Abilities.Summon`
  - `CombatSocket.*` (`Weapon`, `LeftHand`, `RightHand`, `Tail`)
  - `Montage.Attack.1` ~ `Montage.Attack.4`
- **Config Tags（`Config/DefaultGameplayTags.ini`）**：
  - `Attributes.Vital.Health`, `Attributes.Vital.Mana`
  - `Event.Montage.Attack.Melee`, `Event.Montage.FireBall`
  - `GameplayCue.Meleeimpact`
  - `Message.HealthCrystal`, `Message.HealthPotion`, `Message.ManaCrystal`, `Message.ManaPotion`

---

## 5. C++ 源码目录索引（`Source/Aura/Public/`）

- **根级**：`AuraAbilityTypes.h`、`AuraAssetManager.h`、`AuraGameplayTags.h`
- **`AbilitySystem/`**：
  - `AuraAbilitySystemComponent.h`、`AuraAbilitySystemGlobals.h`、`AuraAbilitySystemLibrary.h`、`AuraAttributeSet.h`
  - `Ability/`：`AuraGameplayAbility.h`、`AuraDamageGameplayAbility.h`、`AuraMeleeAttack.h`、`AuraProjectileSpell.h`、`AuraSummonAbility.h`
  - `AbilityTask/`：`TargetDataUnderMouse.h`
  - `Data/`：`AttributeInfo.h`、`CharacterClassInfo.h`
  - `ExecCalc/`：`ExecCalc_Damage.h`
  - `ModMagCalc/`：`MMC_MaxHealth.h`、`MMC_MaxMana.h`
- **`Actor/`**：`AuraEffectActor.h`、`AuraProjectile.h`
- **`AI/`**：`AuraAIController.h`、`BTService_FindNearestPlayer.h`、`BTTask_Attack.h`
- **`Aura/`**：`AuraGameModeBase.h`
- **`Character/`**：`AuraCharacterBase.h`、`AuraCharacter.h`、`AuraEnemy.h`
- **`Input/`**：`AuraInputComponent.h`、`AuraInputConfig.h`
- **`Interaction/`**：`CombatInterface.h`、`EnemyInterface.h`
- **`Player/`**：`AuraPlayerController.h`、`AuraPlayerState.h`
- **`UI/`**：
  - `HUD/AuraHUD.h`
  - `WidgetController/`：`AuraWidgetController.h`、`OverlayWidgetController.h`、`AttributeMenuWidgetController.h`
  - `Widgets/`：`AuraUserWidget.h`、`DamageTextComponent.h`

---

## 6. 核心蓝图与内容目录（`Content/`）

- **地图**：`Content/Maps/StartupMap.umap`
- **角色与敌人**：`Content/Blueprints/Character/`（`AURA/Bp_Aura_Character` + `ABP_Aura`；敌人基类 `Bp_EnemyBase` 及 `Demon`、`Ghoul`、`Goblin_Slingshoot`、`Goblin_Spear`、`Shaman`）
- **GAS 资产**：`Content/Blueprints/AbilitySystem/`（`AURA/` 玩家技能与 GE；`Enemy/` 敌人技能、Cue 与职业属性 GE；`Data/` 含 `DA_AttributeAsset`、`DA_CharacterClassInfo`、`CT_Damage`、`CT_DamageCaculation`）
- **交互道具**：`Content/Blueprints/Actor/`（药水、水晶、火焰区域）
- **UI**：`Content/Blueprints/UI/`（`BP_AuraHUD`、`WBP_Overlay`、`WBP_AttributeMenu`、`Bp_OverlayWidgetController`、`Bp_AttributeMenuWidgetController`）
- **输入**：`Content/Blueprints/Inputs/`（`IMC_AuraContest`、`GA_AuraInputConfig`、`IA_*`）


## Gideon 换皮资产分支（2026-11-24）

当前 `Bp_AuraGameMode.DefaultPawnClass` 指向 `/Game/Reskin/Gideon/Character/BP_Aura_Gideon`，该蓝图以原 `Bp_Aura_Character` 为父类，因此 ABP 中原有的主角类型转换、MotionWarping 接口与 C++/GAS 初始化仍有效。回退只需恢复 GameMode 的原主角引用。

`Content/Reskin/Gideon/` 集中存放新增副本：Meshes、Retarget、Animations、NativeAnimations、Abilities 和 Character。ABP 延用 Aura 的速度/状态机/DefaultSlot；Idle/Jog 来自 Gideon 原生动画副本，火球施法来自 Aura -> Gideon 重定向。`GA_FireBolt_Gideon -> AM_Cast_FireBolt_Gideon -> AN_MontageEvent(Event.Montage.FireBall) -> SpawnProjectile(CombatSocket.Weapon) -> Weapon.TipSocket`，火球伤害与复制仍使用原实现。玩家 ASC Owner 仍是 PlayerState。

原角色、原技能和原骨架未替换。Gideon 原生受击蒙太奇仅配置到新主角属性；本次未新增玩家受击能力。资源自带的 `GideonPlayerCharacter` 仅修复无用 VR 节点，不作为正式玩家父类。

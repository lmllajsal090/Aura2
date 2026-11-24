# Spec + TDD 02：Debuff 机制、技能菜单与进阶技能

> **状态**：`[Draft — 待 Phase 2 完成后细化]`
> **所属阶段**：Phase 2 (M2.3) & Phase 3 (M3.1, M3.2)
> **参考源码**：`DruidMech/GameplayAbilitySystem_Aura`（Debuffs、Spell Menu、Electrocute、ArcaneShards 章节）

---

## 1. 目标与边界（Goal & Non-Goals）

### 目标
1. **Debuff 与范围伤害**：
   - 扩展 `FAuraGameplayEffectContext` 支持 Debuff 状态（`bIsSuccessfulDebuff`、`DebuffDamage`、`DebuffDuration`、`DebuffFrequency`、`DamageType`）及 `RadialDamage` 参数，完整实现自定义 `NetSerialize`。
   - 落地 **火系灼烧（`Debuff.Burn`）** 与 **雷系眩晕（`Debuff.Stun`）** 两种机制迥异的异常状态。
2. **技能菜单（Spell Menu）**：
   - 建立 `UAbilityInfo` DataAsset 与技能状态 Tag（`Locked` / `Eligible` / `Unlocked` / `Equipped`）。
   - 实现 `USpellMenuWidgetController`，支持消耗 `SpellPoints` 解锁/升级技能，并将主动技能动态装配到快捷键槽位（`LMB` / `RMB` / `1~4`）。
3. **差异化技能扩充（3 种典型 GAS 技能形态）**：
   - 投射物进阶：多发/追踪火球（`FireBolt` 强化）
   - 持续引导射线：闪电链（`Electrocute`）
   - 地面选点范围：奥术碎片（`ArcaneShards` + `MagicCircle`）

### 明确不做（Non-Goals）
- 不照搬原课程所有次要技能（如 `FireBlast` 视时间决定）及被动技能槽（被动能力直接升级整合进自研饰品系统 `03_Trinket_System.md`）。

---

## 2. 关键架构设计要点（草案）

- **Debuff 应用模式**：在 `UExecCalc_Damage` 中结合攻击方 `DebuffChance` 与受击方对应元素抗性判定是否挂上 Debuff；命中结果写入 `FAuraGameplayEffectContext`，由 `UAuraAttributeSet::PostGameplayEffectExecute` 在服务端动态构建并应用 Duration + Period GE（挂对应 `Debuff.*` Tag，驱动 `UDebuffNiagaraComponent` 自动显隐）。
- **眩晕禁控**：`Debuff.Stun` 触发时通过 `Player.Block.*` 标签（`BlockInputPressed` / `BlockInputHeld` / `BlockCursorTrace`）拦截输入与移动，敌人则同步 Blackboard `Stunned` 键。
- **技能槽位动态换绑**：服务端修改对应 `FGameplayAbilitySpec` 的 `DynamicSpecSourceTags`（清空旧 InputTag、写入新 InputTag）并调用 `MarkAbilitySpecDirty(Spec)`，确保客户端同步生效。

---

## 3. 验收标准（Acceptance Criteria）

- [ ] `FAuraGameplayEffectContext::NetSerialize` 序列化/反序列化在 Listen Server + Client 下字段完全一致
- [ ] 灼烧 DoT 定时扣血且正确显示飘字；眩晕期间玩家/敌人均无法移动与施法，结束后正常恢复
- [ ] 技能菜单支持随等级自动将 `Locked` 转为 `Eligible`、消费技能点升级、换槽后对应快捷键立即触发新技能
- [ ] 3 个不同形态技能在双端 PIE 下表现与伤害均正常

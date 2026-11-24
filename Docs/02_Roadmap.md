# Aura 开发路线图（第 20 章起自主设计）

> **目标截止时间**：3 月前完成可演示、可讲透架构的实习 Demo 闭环。
> **参考源码**：[DruidMech/GameplayAbilitySystem_Aura](https://github.com/DruidMech/GameplayAbilitySystem_Aura)（仅作为思路对照，核心模块按自有 Spec 设计实现）。
> **状态说明**：`[x]` 已完成 / `[-]` 进行中 / `[ ]` 待开发

---

## 阶段总览与优先级策略

为在有限时间内兼顾 **Aura** 与 **TPS_Shoot** 两个 Demo，从第 20 章往后的原版内容按 **A（必做闭环）/ B（精选架构）/ C（P2 选做）** 分层裁剪，并将原版“被动技能 + 掉落表”升级融合进**自研饰品系统**。

---

### Phase 1：工程基线与 AI 协作底座（已完成）
- [x] 完成前 19 章核心 GAS 战斗闭环（属性、`ExecCalc_Damage`、投射物/近战/召唤、AI 行为树、基础 HUD）
- [x] 清理历史 C++ 命名不规范问题并通过 UE 5.8 UBT 编译验证
- [x] 建立 GitHub 级项目文档体系（`README.md` + `AGENTS.md` + `Docs/Specs/` + ADR）

---

### Phase 2：核心 RPG 成长与战斗闭环（A 档 · 必做）
> 详细设计见：[Docs/Specs/01_Cost_Cooldown_Leveling.md](Specs/01_Cost_Cooldown_Leveling.md) 与 [Docs/Specs/02_Debuffs_And_Skills.md](Specs/02_Debuffs_And_Skills.md)

- [ ] **M2.1 技能蓝耗与冷却（Cost & Cooldown）**
  - 为主动技能补齐 Cost GE / Cooldown GE 与冷却 Tag
  - 实现 `UWaitCooldownChange`（`BlueprintAsyncActionBase`）驱动 HUD 冷却遮罩与倒计时
- [ ] **M2.2 经验、升级与属性加点（XP, Level-Up & Attribute Points）**
  - `ULevelUpInfo` DataAsset + `IncomingXP` Meta Attribute + 敌人死亡 XP 奖励
  - `AAuraPlayerState` 扩展 `XP`、`AttributePoints`、`SpellPoints` 复制与委托广播
  - `IPlayerInterface` 解耦接口 + 升级回满血蓝与升级特效
  - `WBP_AttributeMenu` 消费属性点（`Server` RPC -> Attribute Upgrade GE）
- [ ] **M2.3 Debuff 与范围衰减伤害（精选 1~2 个高价值机制）**
  - 扩展 `FAuraGameplayEffectContext`（支持 Debuff 参数与 `NetSerialize`）
  - 实现火系灼烧（持续 DoT + 特效）与雷系眩晕（`BlockInput` / 行动受限 Tag）
  - 支持 Radial Damage（范围衰减伤害）与击退/死亡冲量

---

### Phase 3：技能菜单与差异化技能扩充（B 档 · 精选）
> 详细设计见：[Docs/Specs/02_Debuffs_And_Skills.md](Specs/02_Debuffs_And_Skills.md)

- [ ] **M3.1 技能菜单核心闭环（Spell Menu）**
  - `UAbilityInfo` DataAsset + 技能状态机（`Locked` / `Eligible` / `Unlocked` / `Equipped`）
  - `USpellMenuWidgetController`：消耗技能点解锁/升级、动态改绑技能快捷键槽位（更新 `DynamicSpecSourceTags`）
- [ ] **M3.2 2~3 个机制差异化的新技能**
  - 投射物进阶：多发/追踪火球（`FireBolt` 强化）
  - 持续引导型：闪电链（`Electrocute`，练持续 Tick 扣蓝与目标牵引）
  - 地面选点范围型：奥术碎片（`ArcaneShards` + `MagicCircle` 地面指示圈）

---

### Phase 4：核心自研亮点 — GAS 饰品系统（王牌模块）
> 详细设计见：[Docs/Specs/03_Trinket_System.md](Specs/03_Trinket_System.md)
> **策略**：将原版“被动技能（Passive Spells）”与“敌人掉落（Loot Tiers）”直接融合升级进饰品系统，避免重复建设，最大化架构辨识度。

- [ ] **M4.1 数据驱动与组件架构**：`UTrinketDataAsset` + `UTrinketComponent`（`PlayerState` 权威持有）+ `FFastArraySerializer` 网络同步
- [ ] **M4.2 GAS 生命周期管理**：装备时动态应用 `Infinite GE` + 赋予被动 `GameplayAbility`，保存 `ActiveGEHandle` / `SpecHandle`，卸下时精准回收零泄漏
- [ ] **M4.3 事件驱动被动与套装**：基于 `GameplayEvent`（`OnCrit` / `OnKill` / `OnTakeDamage`）零耦合触发被动效果 + 套装 Tag 激活规则
- [ ] **M4.4 掉落与 UI 闭环**：敌人掉落饰品拾取物（Loot Tier）+ `UTrinketWidgetController` + `WBP_TrinketMenu`

---

### Phase 5：可控换皮与作品集收口
> 技术检查表见：[Docs/Specs/04_Reskin_Contract.md](Specs/04_Reskin_Contract.md)

- [ ] **M5.1 主角换皮**：按契约核对骨架 Socket、AnimBP、Montage Notify、MotionWarping、碰撞通道与溶解材质
- [ ] **M5.2 地图换皮**：按契约核对 NavMesh、地面 Visibility 碰撞、后处理描边与默认关卡配置
- [ ] **M5.3 演示包装**：双端 PIE 联机验收、录制演示 GIF/视频、完善根目录 `README.md`

---

### P2 选做池（Backlog · 视 3 月前剩余时间裁剪）
- [ ] 轻量单槽 `USaveGame`（序列化玩家等级、加点、已解锁技能与已装备饰品，不强求多槽 MVVM 读档大厅）
- [ ] 多关卡无缝传送与 Checkpoints
- [ ] 清理历史蓝图暴露拼写（配合 `CoreRedirects` 统一 Resave）

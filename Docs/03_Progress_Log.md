# Aura 开发进度与架构决策日志（Progress & ADR Log）

> **跨 AI 交接棒**：无论在 Antigravity 还是 GPT/Codex 中开启新对话，请先读取本文件底部的「当前状态与下一步」。每次完成功能或关键决策后，务必追加记录。

---

## 当前状态与下一步（Next Session Entry Point）

- **当前阶段**：Phase 1 已收尾，准备启动 **Phase 2（M2.1 蓝耗/冷却 + M2.2 经验升级加点）**
- **附加试验**：Gideon 主角换皮已接入并完成回退演练；已通过单人及双人死亡验收，完整多人战斗回归仍待验收，详见本页换皮记录。
- **代码基线**：已完成教程第 19 章 + 安全 C++ 命名规范化（UE 5.8 UBT 编译通过）
- **待讨论 / 下一步**：
  1. 在 UE 编辑器中打开地图跑一次 PIE 验证重命名零报错，并将 Phase 1 成果提交推送到 `origin`（`https://github.com/lmllajsal090/Aura2`）。
  2. 以 **Mode A** 审阅定稿 `Docs/Specs/01_Cost_Cooldown_Leveling.md`，开始推进 Phase 2（或按需先切去为 `TPS_Shoot` 建立同款文档底座）。

---

## 变更日志（Changelog）

### 2026-11-25 — Gideon 布娃娃与全身溶解

- 原导入 Physics Asset 有 7 个刚体、0 个关节约束。用 `GE_Damage` 复现并测得 4 秒时骨骼离骨盆约 4.7 米，关闭布料仍会拉伸。新增 `PA_Gideon_Aura` (18 刚体、17 约束)，死亡时替换物理资产并关闭布料模拟，镜头跟随仍由原角色控制器负责。
- `BP_Aura_Gideon` 继续使用 Aura 主角继承链。沿用属性集触发的死亡 Multicast，倒地时法杖脱手，身体和法杖只与静态场景碰撞。身体 14 个材质槽、法杖 2 个材质槽独立创建动态材质，并共用 `MF_DeathDissolve_Gideon` 遮罩。约 0.75 秒后开始溶解，持续 1.8 秒，完成后隐藏并停用物理。
- 原 Aura 与敌人继续走原死亡表现配置。已通过 UE 5.8 DebugGame Editor 编译及单人 PIE。双人 Listen Server PIE 用地图敌人的实际攻击击杀远程玩家，双端均监测到清零、倒地、法杖脱手和全部槽位溶解。已还原 PIE 联机设置。
- 改前快照、独立验证和回退清单在 `F:\笔记\Aura笔记\.gideon_reskin` 中。

### 2026-11-25 — Gideon 受击蒙太奇预览、脚步和命名

- 原 `HitReact_Front` 是局部空间叠加序列。将它的换皮副本作为普通全身 `DefaultSlot` 蒙太奇播放时，预览身体塌缩。`HitReact_Gideon` 现在使用完整姿势，`AM_HitReact_Gideon` 显式使用 `SKM_Gideon_Aura` 预览。在 0.51 秒帧重新检查模型完整显示，并在 PIE 以项目伤害效果验证播放和骨骼姿势。
- 活跃待机和跑步序列按原项目的动作名后加角色后缀，分别为 `Idle_Gideon`、`Jog_Fwd_Gideon`，位于 `NativeAnimations`。角色和动画蓝图的引用已更新。
- 跑步的四个落脚帧直接引用原 `sfx_Footsteps`，并保留原通知的播放属性。原火球施法通知、飞行和命中声音继续引用既有资源，血花保持继承。原 Aura 主角的受击动画没有声音通知，也未配置死亡音效。

### 2026-11-24 — Gideon 非致命受击动画

- 之前主角已配置 `AM_Gideon_HitReact`，但 `StartupGameplayAbilities` 只有火球。`PostGameplayEffectExecute` 在非致命伤害后以 `Effects.HitReact` 查找能力，未找到可激活的玩家受击能力。
- `BP_Aura_Gideon` 现在启动时赋予 `GA_HitReact_Gideon`。该能力继承 `UAuraGameplayAbility`，由服务端激活，从 Avatar 的战斗接口取得 `AM_Gideon_HitReact`，通过 ASC 播放并同步给客户端。使用 Gideon 原生正面受击动作作为通用动作。
- 受击和火球共用动画插槽。受击可打断火球，相关蒙太奇任务在完成、中断和取消时均结束技能。受击能力运行期间持有 `Effects.HitReact`，避免持续按住施法键时火球立即盖掉受击动作。动作结束后可继续施法。
- 单人 PIE 用项目 `GE_Damage` 实测两次扣血均播放受击。验证了火球中断、受击期间阻止再施法及动作结束后可继续施法。双人 Listen Server 的服务端伤害和客户端受击播放均已验证。完整死亡表现仍待独立验收。

### 2026-11-24 — Gideon 主角换皮试验（有完整备份，可回退）
- 当前默认主角改为 `/Game/Reskin/Gideon/Character/BP_Aura_Gideon`，它继承原 `Bp_Aura_Character`。相机、MotionWarping、输入、PlayerState/ASC、属性初始化和 UI 沿用原框架；未修改 C++、项目配置或地图。
- 新模型使用独立 Mesh 副本及 Mesh-only `WeaponHandSocket`，原 Aura 模型、蓝图、技能、骨架和 Paragon Gideon 原骨架保留。
- `ABP_Aura_Gideon` 沿用 Aura 状态机与 DefaultSlot，待机/跑步使用 Gideon 原生动画副本。火球最终按用户要求使用 Aura 施法动画重定向后的 `AM_Cast_FireBolt_Gideon`；法杖可见，`CombatSocket.Weapon -> TipSocket` 为发射点，保留 `Event.Montage.FireBall`、FacingTarget 转向窗口与音效通知。
- 配置了 Gideon 原生受击蒙太奇，但玩家启动能力仍只有火球，未额外增加原玩家没有的受击 GAS 能力。死亡布娃娃与溶解链路未改；完整死亡表现、脚步滑动及多人完整战斗回归仍待后续验收。
- 修复了本次试验中旋转位置参数导致的侧躺：最终 Mesh Rotation 明确为 Pitch=0 / Yaw=-90 / Roll=0，PIE 站立确认。
- 资源自带 `GideonPlayerCharacter` 中删除了两个无用的旧 VR 重置节点，以消除缺少 XRBase 导致的红色编译错误；该示例的旧输入映射黄色警告保留，正式玩家不使用该示例。
- 验证：新角色/动画/火球蓝图严格编译通过；单人 PIE 点地移动达到约 600 cm/s；真实激活火球后记录到重定向蒙太奇与玩家投射物，首次采样距法杖 TipSocket 约 9 cm；双人 Listen Server 的两端角色、动画实例、PlayerState ASC 与生命/法力复制检查通过。
- 回退实测：执行 `恢复原主角.py` 后 PIE 确认重新生成原 Aura、原 ABP 和原技能，再切回 Gideon。临时敌人 AI 暂停仅存在于测试世界，PIE 已结束；双人测试设置恢复为原单人 Standalone。
- 备份：`F:\笔记\Aura笔记\主角换皮备份_20261124_230346`，完整 Content/Source/Config/Docs/项目文件共 2,982 个文件、3,819,195,325 字节，逐文件 SHA-256 校验。记录与验证证据位于 `F:\笔记\Aura笔记\.gideon_reskin`。
- 既有问题：伤害计算仍可能输出缺失 SetByCaller `Damage` 的错误；本次未修改伤害公式。未提交 Git，保留用户原有暂存区状态。


### 2026-11-24（续）— 仓库工程化升级（对标 GitHub UE + AGENTS.md 最佳实践）
- **完成内容**：
  1. 新增面向人类与面试官的根目录 `README.md`（含 Mermaid GAS 架构图、技术栈表、文档索引与 Git LFS / UE 5.8 快速开始）。
  2. 采用渐进式披露重构 `Docs/`：将原合并文件拆分为精简总看板 `Docs/02_Roadmap.md` + 独立设计目录 `Docs/Specs/`。
  3. 在 `Docs/Specs/` 下建立通用模板 `_TEMPLATE.md` 及 4 份系统草案：`01_Cost_Cooldown_Leveling.md`、`02_Debuffs_And_Skills.md`、`03_Trinket_System.md`、`04_Reskin_Contract.md`。
  4. 同步更新 `AGENTS.md` 文档索引，固化“先 Spec+TDD 定稿再写代码”的工程习惯。

### 2026-11-24 — 工程基线盘点、C++ 命名规范化与跨 AI 文档初始化
- **完成内容**：
  1. **C++ 文件与符号规范化（100% 蓝图安全范围）**：
     - 重命名 `AuraabilitySystemComponent.h/.cpp` -> `AuraAbilitySystemComponent.h/.cpp`，类名 `UAuraabilitySystemComponent` -> `UAuraAbilitySystemComponent`，同步更新 `Config/DefaultEngine.ini` 中的 `ClassRedirects`。
     - 重命名 `AuraCharater.h/.cpp` -> `AuraCharacter.h/.cpp`（类本身已为 `AAuraCharacter`）。
     - 重命名目录 `UI/WidgerController/` -> `UI/WidgetController/` 及全部 `#include` 路径。
     - 规范 Meta Attribute `InComingDamage` -> `IncomingDamage`（UE `FName` 大小写不敏感，蓝图安全）。
     - 规范 C++ Tag 成员名 `FAuraGameplayTags::Effects_HitDamage` -> `Effects_HitReact`（底层注册 Tag 字符串本身已为 `Effects.HitReact`）。
     - 修正私有变量与无反射风险拼写：`CatchDestination` -> `CachedDestination`、`Category="Combvat"` -> `"Combat"`、`"Class Deafults"` / `"Character Class Deafults"` -> `"Class Defaults"` / `"Character Class Defaults"`、`GameplayMechnics` -> `GameplayMechanics`、`USTRUCT(BluePrintType)` -> `USTRUCT(BlueprintType)`，移除 `AuraGameplayTags.h` 重复头文件包含。
  2. **建立跨 AI 通用文档体系**：
     - 创建根目录 `AGENTS.md` 及 `Docs/01_Architecture.md`、`Docs/02_Roadmap_And_Specs.md`、`Docs/03_Progress_Log.md`。
- **遗留说明**：
  - `git status` 中存在此前已有的两条已暂存删除记录（`AD Source/Aura/.../DamageWidgetController.h/.cpp`），本次未改动用户既有暂存区状态。
  - 其余直接暴露给蓝图资产的历史拼写（`bDestoryOnEffectApplication`、`BroadcastInitiaValue`、`ImpatctSound`、`IncrementMinionCount(int32 Ammout)`）暂保留并登记于 `AGENTS.md` 红线清单，待确认后再按需配合 `CoreRedirects` 处理。

---

## 架构决策记录（ADR）

### ADR-001：采用 Repo-First 分层文档驱动双 AI 协作
- **背景**：项目将同时使用 Antigravity 与具备本地读写能力的 GPT Agent 协作开发。
- **决策**：在项目根目录维护标准入口 `AGENTS.md`，并将架构快照、路线图/Spec、进度日志拆分在 `Docs/` 下。
- **结果**：任何 AI 都能按需读取精准上下文，消除“按原教程代码脑补本地符号”的幻觉；文档同时沉淀为面试复盘素材。

### ADR-002：默认 AI 协作模式定为 Mode A（副驾驶/导师模式），按需切 Mode B
- **背景**：项目目标是实习 Demo + 面试能讲清原理，同时提升对 AI 工具的掌控力。
- **决策**：默认采用 Mode A（先方案讨论 -> 给骨架/要点 -> 用户主导核心编写 -> AI Review），仅在用户明确指令时切入 Mode B 直接实现。

### ADR-003：全周期保持 Listen Server 多人同步友好架构
- **背景**：Aura 底层已按 GAS 多人架构搭建，客户端/战斗岗面试高度关注网络同步意识。
- **决策**：后续补齐的升级系统、技能以及自研饰品系统全部维持服务端权威 + 规范网络复制实现。

### ADR-004：第 20 章后功能裁剪与「被动技能 + 掉落」融合进饰品系统
- **背景**：3 月前需同时推进 Aura 与 TPS_Shoot 两个实习 Demo，原教程第 20~38 章体量过大且后段偏向多槽存档/读档大厅。
- **决策**：必做蓝耗/冷却、经验升级加点、1~2 个代表性 Debuff + 范围伤害；精选技能菜单与 3 种差异化技能；将原版被动技能与 Loot Tiers 直接融合升级进自研饰品系统；多槽 MVVM 读档/存档降至 P2。
- **结果**：压缩冗余开发周期，将工程深度集中到面试辨识度最高的饰品架构上。

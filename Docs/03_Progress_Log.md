# Aura 开发进度与架构决策日志（Progress & ADR Log）

> **跨 AI 交接棒**：无论在 Antigravity 还是 GPT/Codex 中开启新对话，请先读取本文件底部的「当前状态与下一步」。每次完成功能或关键决策后，务必追加记录。

---

## 当前状态与下一步（Next Session Entry Point）

- **当前阶段**：Phase 1 已收尾，准备启动 **Phase 2（M2.1 蓝耗/冷却 + M2.2 经验升级加点）**
- **代码基线**：已完成教程第 19 章 + 安全 C++ 命名规范化（UE 5.8 UBT 编译通过）
- **待讨论 / 下一步**：
  1. 在 UE 编辑器中打开地图跑一次 PIE 验证重命名零报错，并将 Phase 1 成果提交推送到 `origin`（`https://github.com/lmllajsal090/Aura2`）。
  2. 以 **Mode A** 审阅定稿 `Docs/Specs/01_Cost_Cooldown_Leveling.md`，开始推进 Phase 2（或按需先切去为 `TPS_Shoot` 建立同款文档底座）。

---

## 变更日志（Changelog）

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

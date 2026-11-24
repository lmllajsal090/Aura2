# AGENTS.md — Aura (UE 5.8 GAS Action RPG)

> **定位**：本文件是 Aura 项目的 **跨 AI 通用协作入口（Single Source of Truth）**，适用于本地所有具备仓库读写能力的 Coding Agent（如 Antigravity、ChatGPT/Codex 等）。
> 开始任何设计讨论或代码修改前，请先阅读本文件，并按需查阅 `Docs/` 下的专题文档。

---

## 1. 项目概览与目标

- **项目路径**：`C:\UEProjects\Aura_5_8`（引擎版本：**Unreal Engine 5.8**）
- **项目性质**：实习求职 Demo（目标在 **3 月前** 完成完整可玩闭环与架构亮点展示）。
- **项目基础**：基于 Stephen Ulibarri 的 GAS RPG 教程完成至 **第 19 章**，后续不再逐集跟学，转为**自主研发 + 选择性参考教程源码**。
- **官方参考源码仓库**：`https://github.com/DruidMech/GameplayAbilitySystem_Aura`
- **三大核心演化目标**：
  1. **系统补齐**：参考原仓库补齐升级/经验系统、属性点/技能点、技能菜单与若干核心技能（含 Debuff / 被动机制）。
  2. **自研架构亮点 — 饰品系统（Trinket System）**：独立设计并实现一套深度融入 GAS 的饰品装备系统，体现对 GAS、数据驱动、解耦与网络同步的工程理解。
  3. **可控换皮（主角 + 地图）**：在严格遵守动画/Socket/碰撞/导航契约的前提下替换主角模型与地图场景。

---

## 2. 文档索引（按需阅读 · 渐进式披露）

| 文档 | 内容 | 何时阅读 |
| :--- | :--- | :--- |
| `README.md` | 面向人类与面试官的项目门面、核心数据流图与快速开始 | 对外展示、阶段收口同步更新 |
| `Docs/01_Architecture.md` | 现有代码架构地图、GAS 数据流、属性/Tag/资产清单、网络拓扑 | 每次新会话首次接手、或需要查找类与数据时 |
| `Docs/02_Roadmap.md` | 第 20 章后的阶段里程碑看板与优先级取舍（A/B/C 档） | 规划下一阶段任务或核对范围边界时 |
| `Docs/03_Progress_Log.md` | 当前进度、最近修改、架构决策记录（ADR）与下一步交接点 | **每次新会话必读**；每次完成功能后必须更新 |
| `Docs/Specs/*.md` | 单系统 Spec + TDD 设计书（含 `_TEMPLATE.md` 模板） | **开发任意新系统前必读对应 Spec，先对齐定稿再写代码** |

---

## 3. AI 协作模式（严格遵守）

### 默认模式：Mode A（副驾驶 / 导师模式）
除非用户明确指令切换，**所有 AI 默认处于 Mode A**：
1. **先对齐设计再谈代码**：收到需求后，先分析架构方案、数据流、网络同步影响与蓝图/C++ 边界，给出 1~2 种权衡方案供用户决策。
2. **给骨架与思路，不越权全包**：确认方案后，优先提供：
   - 头文件接口草案 / 类职责拆分；
   - 关键实现步骤、UE API 要点与避坑提示；
   - 让用户自己编写核心逻辑，AI 负责 **Code Review、编译排错、边界检查与面试追问演练**。
3. **禁止行为**：在 Mode A 下，不要未经确认直接批量改写大量业务 `.cpp` 实现。

### 按需切换：Mode B（主程 + 执行模式）
仅当用户明确提出（例如“切换 B 模式”、“这个直接帮我写”、“你来实现并编译”）时进入 Mode B：
1. 对非平凡模块，仍需先简要确认设计与改动范围再动手。
2. AI 编写完整 C++ 实现并执行 UBT 编译验证。
3. 改动完成后，**必须用简明语言解释核心改动与设计意图**，确保用户完全理解每一行代码（便于后续面试讲解）。

---

## 4. 硬性工程红线（Architecture & Safety Rules）

### 4.1 保持多人同步友好（Replication-First）
本项目底层采用 **Listen Server + Client** 联机友好架构，所有新功能（包括升级系统、新技能与自研饰品系统）**必须遵守**：
- **权威归属**：玩家 `ASC` 和 `AttributeSet` 挂在 `AAuraPlayerState`（`Mixed` 复制模式）；敌人 `ASC` 和 `AttributeSet` 挂在 `AAuraEnemy`（`Minimal` 复制模式）。
- **状态修改权威性**：属性修改、技能赋予/撤销、装备穿脱、经验结算等核心状态变更必须在服务端（`HasAuthority()`）执行，客户端通过 `Server` RPC 请求。
- **网络复制规范**：
  - 属性变更使用 `DOREPLIFETIME_CONDITION_NOTIFY(..., COND_None, REPNOTIFY_Always)` + `GAMEPLAYATTRIBUTE_REPNOTIFY`。
  - 新增列表/槽位类同步状态（如饰品槽位）优先考虑 `FFastArraySerializer` 或结构体数组 + `ReplicatedUsing` 回调，并明确 `OnRep` 驱动 UI 更新。
  - 自定义 `FGameplayEffectContext` 字段必须同步更新 `FAuraGameplayEffectContext::NetSerialize` 和 `Duplicate`。
  - UI 与本地输入逻辑严格守卫 `IsLocalController()`，禁止在服务端或模拟代理上创建 HUD/Widget。

### 4.2 蓝图与资产安全红线（UE C++ 铁律）
- **禁止随意改名或删除已暴露给蓝图的反射符号**（`UCLASS` / `USTRUCT` / `UPROPERTY` / `UFUNCTION` / `GameplayTag` 字符串）。如确需重构，必须先评估 `.uasset` 影响并在 `Config/DefaultEngine.ini` 的 `[CoreRedirects]` 中配置重定向，再通知用户在 UE 编辑器中加载保存验证。
- **当前已存在且暴露给蓝图的历史命名（请严格照用，切勿“顺手纠错”导致蓝图断连）**：
  - `AAuraEffectActor::bDestoryOnEffectApplication`（`Destory`）
  - `UAuraWidgetController::BroadcastInitiaValue()`（`Initia`）
  - `FTaggedMontage::ImpatctSound`（`Impatct`）
  - `ICombatInterface::IncrementMinionCount(int32 Ammout)`（参数 Pin 名 `Ammout`）
  - `UOverlayWidgetController` 委托：`OnHealthChange` / `OnMaxHealthChange` / `OnManaChange` / `OnMaxManaChange`（注意无 `d`，而 `AAuraEnemy` 上是 `OnHealthChanged` / `OnMaxHealthChanged`）
  - `UAuraAbilitySystemLibrary::GetLivePlayerWithRadius`
  - `Config/DefaultGameplayTags.ini` 中的 `GameplayCue.Meleeimpact`
- **严禁直接编辑二进制 `.uasset` / `.umap` 文件**；蓝图侧配置变更应给出清晰、可一步步照做的编辑器操作清单（或经确认通过 Unreal MCP 辅助）。

### 4.3 C++ 与蓝图职责边界
- **C++ 负责**：GAS 底层框架、`AttributeSet` / `MMC` / `ExecCalc`、`UDataAsset` 数据结构、网络同步、组件与 `WidgetController` 数据绑定、可复用的 Ability 基类与 `AbilityTask`、AI 基础 Task/Service。
- **蓝图负责**：`GE_*` / `GA_*` 具体数值与资产配置、角色/敌人资源组装、`ABP_*` 动画状态机与 Montage 通知、Niagara/音效表现、UMG 控件排版与动效。

### 4.4 代码规范与防幻觉
- 遵循 Unreal 命名规范（`A` Actor、`U` UObject、`F` Struct、`E` Enum、`I` Interface、`b` Bool、`TObjectPtr<>` 成员指针）。
- 修改代码前务必先读取目标文件确认真实签名，**禁止按记忆套用标准 Aura 教程源码的命名或函数签名**。
- 修改 C++ 后应尽量使用 UE 5.8 UBT 编译验证，并确保无新增编译警告/错误。

---

## 5. 构建与验证命令

- **引擎安装路径**：`C:\Program Files\Epic Games\UE_5.8`
- **命令行编译 Editor 目标（PowerShell）**：
  ```powershell
  & "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AuraEditor Win64 Development -Project="C:\UEProjects\Aura_5_8\Aura.uproject" -WaitMutex -FromMsBuild
  ```
- **注意**：如果 UE 编辑器正在运行，请勿在外部触发全量 UBT 覆写 DLL（或优先使用 Live Coding / 先关闭编辑器再编译）。

---

## 6. 跨 AI 交接协议（Handoff Protocol）

每次完成一个功能模块、重要 Bug 修复或架构决策后，请主动更新：
1. **`Docs/03_Progress_Log.md`**：追加日期、完成项、关键决策（ADR）与下一步待办。
2. **`Docs/01_Architecture.md`**：仅当新增核心类、属性、GameplayTag 或数据流变更时同步更新。
3. **`Docs/02_Roadmap.md` & `Docs/Specs/*.md`**：勾选已完成里程碑，并同步更新对应系统 Spec 的状态与设计变更点。

# Spec + Checklist 04：主角与地图换皮技术契约

> **状态**：`[Approved Baseline — 换皮执行时逐项核对]`
> **所属阶段**：Phase 5 / M5.1 & M5.2
> **核心原则**：换皮服务于作品集视觉辨识度，**绝不能破坏现有 C++ / GAS / AI 契约**。替换前先核对下方硬性接口，再做美术资产替换。

---

## 1. 主角换皮技术契约（替换 `Bp_Aura_Character` 模型/骨架）

### 1.1 骨架与挂点（Sockets）
- [ ] **武器挂点**：角色骨架必须保留 `WeaponHandSocket`（`AAuraCharacterBase` 构造函数通过 `Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"))` 硬编码绑定）。
- [ ] **战斗出点 Socket**：
  - 武器骨架需配置 `WeaponTipSocketName`（对应 `CombatSocket.Weapon`）
  - 如技能使用徒手/左右手施法，角色骨架需配置 `LeftHandSocketName` / `RightHandSocketName`（对应 `CombatSocket.LeftHand` / `CombatSocket.RightHand`）

### 1.2 动画蓝图与 Montage 通知
- [ ] 新 `ABP` 继承或对齐 `ABP_Aura` 的基础 Locomotion（Idle/Run 混合）、`DefaultSlot`（用于施法与受击 Montage 叠加）。
- [ ] 所有施法/攻击 Montage 必须包含：
  - `AN_MontageEvent` 通知（发送正确的 `Event.Montage.*` Tag，否则 `WaitGameplayEvent` 永远不触发投射物/伤害）
  - `MotionWarping` Notify 窗口（`WarpTargetName = FacingTarget`，否则俯视角施法转向失效）
- [ ] 新角色的 `HitReactMontage` 已在蓝图 Defaults 中重新指定。

### 1.3 碰撞通道与材质溶解
- [ ] `CapsuleComponent`：`ECC_Camera` = Ignore，`bGenerateOverlapEvents` = false。
- [ ] `Mesh`：`ECC_Camera` = Ignore，`ECC_Projectile` = Overlap，`bGenerateOverlapEvents` = true。
- [x] 原角色继续使用 `AAuraCharacterBase::Dissolve()` 单槽溶解。Gideon 在 `AAuraCharacter` 中配置死亡专用 `DeathPhysicsAsset`、`BodyDeathMaterials` 和 `WeaponDeathMaterials`。身体与法杖按索引配置全部槽位，材质均实现 `Dissolve` 参数。

---

## 2. 地图换皮技术契约（替换/新建主关卡）

### 2.1 寻路与输入射线
- [ ] **导航网格**：必须放置覆盖全可走区域的 `NavMeshBoundsVolume` 并重新 Build Paths（验证 `AutoRun` 点击寻路、敌人追击与萨满召唤落点）。
- [ ] **地面碰撞**：所有地面 StaticMesh / Landscape 必须对 `ECC_Visibility` 响应 **Block**（`CursorTrace` 与 `TargetDataUnderMouse` 强依赖此通道）。
- [ ] **俯视角防遮挡**：拱门、树冠、高墙等可能挡住摄像机的物件需配置 `ECC_Camera = Ignore`（或后续加淡出材质）。

### 2.2 高亮描边与关卡配置
- [ ] **敌人红边高亮**：新地图需放置无界（Unbound）`PostProcessVolume`，并挂载现有的 CustomDepth 描边后处理材质（对应 `CUSTOM_DEPTH_RED = 250`）。
- [ ] **PlayerStart & 出生点**：正确放置 `PlayerStart` 并测试双端 PIE 出生位置不重叠卡墙。
- [ ] **GameMode 与默认地图**：确认新地图的 World Settings 正确覆写 `Bp_AuraGameMode`；验证稳定后再更新 `Config/DefaultEngine.ini` 的 `GameDefaultMap` 与 `EditorStartupMap`。

---

## 3. 换皮验收标准

- [ ] 主角移动、点地自动寻路、Shift 原地施法、全部主动技能与饰品被动表现正常
- [ ] 敌人 AI 寻路、攻击、召唤、受击高亮描边与死亡溶解均正常
- [ ] Listen Server + Client 双端 PIE 下无模型错位、无动画不同步、无碰撞穿透


## 4. 2026-11-24 Gideon 可回退试验记录

- [x] 换皮资产使用独立目录；新玩家继承原 Aura 蓝图，保留 GAS、相机、控制器和 MotionWarping。
- [x] 独立 Mesh 保留 WeaponHandSocket，法杖保留 TipSocket；火球最终使用法杖发射。
- [x] Mesh 旋转以显式轴名设置为 Pitch=0、Yaw=-90、Roll=0；PIE 站立确认。
- [x] Gideon 原生待机/跑步接入 Aura 状态机与 DefaultSlot。
- [x] Aura 火球蒙太奇重定向至 Gideon，事件通知、音效和 FacingTarget 窗口保留；运行中捕获玩家火球。
- [x] 新玩家/动画/技能蓝图严格编译；原资产检查产生的临时脏标记已通过磁盘原文件重载清除。
- [x] 双人 Listen Server 角色、动画实例、ASC Owner、生命/法力复制检查；测试后恢复原 PIE 设置。
- [x] 真正切回原 Aura 并启动 PIE，确认回退有效，再切回 Gideon。
- [x] Gideon 受击 GAS 能力已接入。单人测试扣血、动画和施法中断恢复，双人测试客户端受击蒙太奇播放。
- [x] 受击完整姿势和预览修正，按 Aura 命名约定整理当前动作资产，恢复原版脚步通知。
- [x] 受伤致死、布娃娃倒地、法杖脱手和 16 个材质槽完整溶解通过单人及双人 PIE。
- [ ] 脚步滑动与多人完整战斗回归验收。

本次不视为整个 M5.1 或地图换皮完成。完整备份与回退脚本见 `F:\笔记\Aura笔记\主角换皮备份_20261124_230346`，详细证据见 `.gideon_reskin` 工作记录目录。

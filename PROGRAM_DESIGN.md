# 卡牌原型程序设计说明

## 1. 目标

这个原型的目标不是把玩法一次性做满，而是先把以下几件事搭稳：

- 主牌区点击匹配
- 备用牌堆翻牌
- Undo / Reset
- 关卡数据外置
- 关卡合法性和可解性检查

代码结构上，当前版本优先保证两件事：

- 后续加关卡时，尽量不改场景代码
- 后续加新操作和新回退类型时，改动范围可控

## 2. 当前结构

### 2.1 模块划分

当前工程按“数据、规则、表现、装配”分层：

- 数据定义
  - `Classes/data/LevelDefinition.h`
  - 定义关卡文件中的布局、牌堆、卡牌描述结构

- 数据加载与检查
  - `Classes/data/LevelLoader.*`
  - `Classes/logic/LevelValidator.*`
  - `Classes/logic/LevelSolver.*`
  - `Classes/logic/LevelInspector.*`
  - 负责读取 JSON、做结构校验、做可解性检查、输出巡检结果

- 运行时状态与规则
  - `Classes/logic/GameState.*`
  - 负责运行时卡牌状态、匹配规则、翻牌规则、Undo 所需状态恢复

- 可回退命令
  - `Classes/logic/GameCommand.*`
  - 把一次用户操作封装成一个可执行、可回退的命令

- 视图与表现
  - `Classes/view/CardView.*`
  - `Classes/view/CardAssetCatalog.h`
  - 负责卡牌节点显示和资源路径管理

- 场景装配与输入
  - `Classes/view/CardGameScene.*`
  - `Classes/view/GameOverlayView.*`
  - `Classes/logic/LevelSessionLoader.*`
  - 负责关卡启动、输入分发、动画编排、按钮和通关弹层

- 启动入口
  - `Classes/app/AppDelegate.*`
  - 负责应用启动和场景挂接，不承载业务逻辑

### 2.2 职责边界

建议长期保持下面的边界，不要混写：

- 改关卡内容：先改 JSON
- 改规则：改 `GameState`
- 改 Undo：改 `GameCommand + GameDelta`
- 改卡牌显示：改 `CardView`
- 改按钮或通关弹层：改 `GameOverlayView`
- 改输入分发或动画编排：改 `CardGameScene`
- 改关卡启动和巡检流程：改 `LevelSessionLoader`

## 3. 运行流程

### 3.1 启动流程

当前启动链路如下：

1. `CardGameScene` 初始化场景。
2. `LevelSessionLoader::load(...)` 读取目标关卡。
3. `LevelSessionLoader` 调用 `LevelInspector`：
   - 扫描关卡目录内的 JSON 文件
   - 对目标关卡做加载、合法性校验和可解性检查
4. 如果关卡文件可用：
   - 用 `GameState::createFromLevelDefinition(...)` 构造运行时状态
5. 如果关卡文件不可用：
   - fallback 到 `GameState::createDemoState()`
6. `CardGameScene` 根据状态创建 `CardView`，并按布局刷新位置。

### 3.2 交互流程

一次正常操作的链路如下：

1. 用户点击卡牌或按钮。
2. `CardGameScene` 判断输入意图。
3. 创建对应 `GameCommand`。
4. `GameCommand` 调用 `GameState` 执行状态变更。
5. `GameState` 产出一份 `GameDelta`，记录 Undo 所需数据。
6. `CardGameScene` 把命令压入 `_undoStack`。
7. 场景重新计算所有卡牌目标位置，并用统一刷新链路播放 `MoveTo` 动画。

Undo 的链路与此相反：

1. 从 `_undoStack` 弹出最后一个命令。
2. 调用命令的 `undo()`。
3. `undo()` 只依赖执行阶段保存的 `GameDelta` 恢复状态。
4. 场景再次刷新所有卡牌位置，播放回退动画。

这个设计的核心点是：动画不保存“怎么退”，只保存“退回后的状态”。只要状态恢复正确，界面就能回到正确位置。

## 4. 关卡数据设计

### 4.1 当前数据来源

当前示例关卡文件：

- `Resources/res/levels/demo_level.json`

关卡文件包含两类信息：

- 卡牌数据
  - `cards`
  - `tableauSlots`
  - `stockPile`
  - `wastePile`

- 布局数据
  - `cardSize`
  - `stockBasePosition`
  - `wasteBasePosition`
  - `stockPileDepthOffset`
  - `wastePileDepthOffset`
  - `coveredCardOffset`
  - `controlMenuPosition`
  - `tableauPositions`

### 4.2 为什么要数据化

把关卡和布局写进 JSON，而不是继续硬编码在场景里，主要有三个收益：

- 调关卡时不需要改 C++ 逻辑
- 关卡制作和程序开发可以并行
- 可以在启动阶段做自动校验，提前发现无效或无解关卡

### 4.3 如何确认关卡可消除

当前工程不是靠人工目测确认，而是走工具链：

- `LevelValidator`
  - 检查重复 id、丢失引用、索引越界、遮挡关系错误等结构问题

- `LevelSolver`
  - 依据当前规则搜索关卡是否能清空主牌区

- `LevelInspector`
  - 串联加载、校验、求解，并提供单关或目录级巡检入口

这意味着后续新增关卡时，可以先让工具判断“是否合法、是否可解”，再交给场景运行。

## 5. 扩展方式

### 5.1 新增一张卡牌

普通卡牌扩展优先改 JSON，不新增 C++ 类。

最小改动路径如下：

1. 在 `Resources/res/levels/demo_level.json` 的 `cards` 中增加卡牌定义。
2. 根据所属区域，把卡牌 id 放入：
   - `tableauSlots`
   - `stockPile`
   - `wastePile`
3. 如果是主牌区卡牌，确认 `tableauIndex` 正确。
4. 如果有遮挡关系，补齐：
   - `blockers`
   - `children`
5. 如果新增了新的主牌区槽位，补齐 `layout.tableauPositions`。
6. 运行程序，查看巡检日志是否出现：
   - 校验错误
   - `clearable: false`

可以把“新增一张卡牌”理解成“补一条数据记录”，而不是“补一段业务逻辑”。

### 5.2 新增一种可回退功能

当前 Undo 的扩展点是明确的，按这个顺序做：

1. 在 `Classes/logic/GameState.h` 的 `GameActionType` 中增加动作类型。
2. 在 `GameDelta` 中补充这类操作回退所需的数据。
3. 在 `Classes/logic/GameState.cpp` 中增加执行接口和回退接口。
4. 在 `Classes/logic/GameCommand.*` 中增加对应命令类。
5. 在 `Classes/view/CardGameScene.cpp` 中把输入入口接到这个新命令上。

这里有一个约束建议：

- 执行时，把 Undo 需要的状态一次性记全
- Undo 时，不要依赖“重新推断当时状态”

这样做的好处是，功能变多后，Undo 仍然稳定，不容易出现“回退一半”或“回退错对象”的问题。

## 6. 维护建议

### 6.1 哪些地方最值得继续优化

从可维护性和可扩展性看，后续优先级较高的事项是：

- 把更多布局常量继续收口到 JSON
- 为多关卡建立统一目录和命名规范
- 增加批量巡检入口，检查整包关卡
- 让关卡日志输出更适合策划直接阅读
- 在文档中沉淀新增关卡和新增命令的操作模板

### 6.2 工程层面对中文注释的建议

这个项目可以保留“少量中文注释”，但建议控制范围：

- 适合写中文注释的地方
  - 模块职责
  - 数据边界
  - Undo 设计约束
  - 容易误改的业务规则

- 不建议写中文注释的地方
  - 显而易见的赋值和流程语句
  - 第三方库代码
  - 会和代码本身重复的描述

原则是：注释用来解释“为什么这样设计”，不是解释“这一行代码在干什么”。

## 7. 面向后续需求的结论

当前版本已经具备继续承接需求的基础：

- 能继续加关卡
- 能继续改布局
- 能继续加新规则
- 能继续加新 Undo 类型
- 能在启动阶段做关卡巡检

如果后续继续迭代，建议继续坚持“数据驱动 + 规则集中 + 命令回退 + 视图解耦”这条线，不要把新增逻辑重新堆回 `CardGameScene`。

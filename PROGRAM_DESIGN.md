# 卡牌程序设计说明

## 1. 结构概览

当前实现按三层组织：

- 数据层
  - 负责关卡配置、卡牌初始数据、布局数据
  - 主要文件：[LevelDefinition.h](./Classes/tools/LevelDefinition.h)、[LevelLoader.cpp](./Classes/tools/LevelLoader.cpp)、[demo_level.json](./Resources/res/levels/demo_level.json)

- 逻辑层
  - 负责规则判断、状态变更、Undo 数据记录
  - 主要文件：[GameState.h](./Classes/cardgame/GameState.h)、[GameState.cpp](./Classes/cardgame/GameState.cpp)、[GameCommand.h](./Classes/cardgame/GameCommand.h)、[GameCommand.cpp](./Classes/cardgame/GameCommand.cpp)

- 视图层
  - 负责显示、输入、动画、弹层
  - 主要文件：[CardGameScene.cpp](./Classes/CardGameScene.cpp)、[CardView.cpp](./Classes/cardgame/CardView.cpp)、[GameOverlayView.h](./Classes/cardgame/GameOverlayView.h)、[GameOverlayView.cpp](./Classes/cardgame/GameOverlayView.cpp)

- 场景装配层
  - 负责关卡启动和运行时入口装配
  - 主要文件：[LevelSessionLoader.h](./Classes/cardgame/LevelSessionLoader.h)、[LevelSessionLoader.cpp](./Classes/cardgame/LevelSessionLoader.cpp)

维护原则：

- 关卡数据调整，优先修改 JSON
- 玩法规则调整，修改 `GameState`
- Undo 能力扩展，修改 `GameCommand` 和 `GameDelta`
- UI 叠层调整，修改 `GameOverlayView`
- 关卡启动流程调整，修改 `LevelSessionLoader`
- 输入和动画调整，修改 `CardGameScene` 或 `CardView`

## 2. 维护规则

### 2.1 关卡内容维护

关卡内容统一在 [demo_level.json](./Resources/res/levels/demo_level.json) 中维护。适合放在 JSON 中的内容包括：

- 卡牌 id、花色、点数
- 卡牌初始区域
- 主牌区槽位
- 备用牌堆和手牌区初始顺序
- 卡牌坐标
- 卡牌遮挡关系

不建议把这些数据直接写回场景代码，否则布局和关卡内容会重新耦合。

### 2.2 玩法规则维护

玩法规则集中在 [GameState.cpp](./Classes/cardgame/GameState.cpp) 中维护，例如：

- 是否允许匹配
- 是否允许翻牌
- 某张牌移除后是否触发翻牌

规则不应散落在 `CardGameScene` 中。场景只负责调用，不负责定义规则。

### 2.3 表现维护

界面和动画修改主要落在以下文件：

- [CardGameScene.cpp](./Classes/CardGameScene.cpp)
- [CardView.cpp](./Classes/cardgame/CardView.cpp)
- [GameOverlayView.cpp](./Classes/cardgame/GameOverlayView.cpp)

这部分负责：

- 卡牌位置刷新
- MoveTo 动画
- 卡牌正反面显示

其中：

- `CardGameScene` 负责输入分发和动画编排
- `GameOverlayView` 负责底部按钮和通关弹层

### 2.4 场景启动流程维护

关卡启动流程集中在 [LevelSessionLoader.cpp](./Classes/cardgame/LevelSessionLoader.cpp)。

这部分负责：

- 批量关卡巡检日志
- 指定关卡读取
- 关卡失败时 fallback 到 demo 状态

如果后续需要支持多关卡切换、关卡列表或编辑器预览，优先扩展这里，不要继续堆到 `CardGameScene`。

## 3. 扩展：新增一张卡牌

普通卡牌扩展不需要新增 C++ 类，直接修改 JSON 即可。

### 步骤 1：新增卡牌定义

在 [demo_level.json](./Resources/res/levels/demo_level.json) 的 `cards` 数组中增加一条记录，例如：

```json
{
  "id": 13,
  "suit": "Spades",
  "rank": 9,
  "zone": "Tableau",
  "faceUp": true,
  "tableauIndex": 8,
  "blockers": [],
  "children": []
}
```

### 步骤 2：接入初始区域

根据卡牌所属区域，更新对应数组：

- 主牌区：`tableauSlots`
- 备用牌堆：`stockPile`
- 手牌区：`wastePile`

如果是主牌区卡牌，`tableauIndex` 必须和槽位索引一致。

### 步骤 3：补充布局坐标

如果新增了主牌区槽位，需要同步更新 `layout.tableauPositions`，例如：

```json
{ "x": 630, "y": 1140 }
```

### 步骤 4：补充遮挡关系

如存在覆盖关系，需要配置：

- `blockers`
  - 当前卡牌被哪些牌压住
- `children`
  - 当前卡牌移除后，哪些牌可能翻开

这组数据同时影响点击、翻牌和关卡可解性。

### 步骤 5：验证可解性

启动时会调用 [LevelSolver.cpp](./Classes/tools/LevelSolver.cpp) 做可解性检查。

如果日志输出 `clearable: false`，说明新增卡牌后当前关卡已经无解，需要回查牌序或遮挡关系。

## 4. 扩展：新增一种可回退功能

当前 Undo 结构如下：

1. 一次用户操作对应一个 `GameCommand`
2. `GameCommand` 调用 `GameState`
3. `GameState` 执行时生成一份 `GameDelta`
4. Undo 时基于 `GameDelta` 做反向恢复

新增可回退功能时，按这个结构扩展，不直接修改 Undo 按钮逻辑。

### 步骤 1：增加动作类型

文件：[GameState.h](./Classes/cardgame/GameState.h)

在 `GameActionType` 中增加新的动作枚举，例如：

```cpp
SwapWasteTopWithReserve
```

### 步骤 2：补充回退数据

文件：[GameState.h](./Classes/cardgame/GameState.h)

在 `GameDelta` 中增加这类操作回退所需的数据，例如：

- 移动卡牌 id
- 原始区域
- 原始索引
- 目标区域原始状态
- 连带翻开的卡牌列表

原则：

- Undo 不依赖推断
- 执行时一次性记录完整回退信息

### 步骤 3：增加状态执行和回退接口

文件：[GameState.cpp](./Classes/cardgame/GameState.cpp)

新增成对接口，例如：

```cpp
bool swapWasteTopWithReserve(GameDelta& outDelta);
bool undoSwapWasteTopWithReserve(const GameDelta& delta);
```

约束建议：

- `xxx(...)` 负责修改状态并填充 `outDelta`
- `undoXxx(...)` 仅基于 `delta` 恢复状态

### 步骤 4：增加命令类

文件：

- [GameCommand.h](./Classes/cardgame/GameCommand.h)
- [GameCommand.cpp](./Classes/cardgame/GameCommand.cpp)

为新操作增加一个命令类，保持与现有 `DrawStockCommand`、`MatchTableauCommand` 一致的执行模式。

### 步骤 5：在场景中接入

文件：[CardGameScene.cpp](./Classes/CardGameScene.cpp)

在合适的输入入口执行：

```cpp
executeCommand(cardgame::GameCommandPtr(new YourNewCommand(...)));
```

接入后，该功能会自动进入现有 `_undoStack` 流程。

## 5. 后续维护建议

建议继续保持以下边界：

- 新关卡、新坐标、新卡牌：优先放 JSON
- 新规则：放 `GameState`
- 新 Undo 类型：走 `GameCommand + GameDelta`
- 新显示效果：放 `CardView` 或 `CardGameScene`

按这个边界维护，后续需求增加时改动范围会更可控。

## 6. 建议的后续增强项

后续如果继续扩展，优先级较高的事项有：

- 将盖牌偏移配置移入 JSON
- 将牌堆叠放偏移移入 JSON
- 增加关卡校验工具，检查非法引用和重复 id
- 支持批量校验所有关卡是否可解

这些改动能显著降低后续关卡制作和维护成本。

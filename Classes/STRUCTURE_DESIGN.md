# Classes 代码结构设计说明

## 1. 设计目标

`Classes` 目录按分层方式组织，目标是把启动入口、数据定义、业务逻辑、界面表现拆开，降低后续修改时的耦合度。

当前设计主要解决以下问题：

- 关卡数据不再和场景代码硬编码耦合
- 玩法规则集中管理，不散落在点击处理里
- Undo 扩展有明确入口
- 视图层只负责显示和输入，不直接定义规则

## 2. 目录划分

### `app`

应用启动层，只负责引擎启动和主场景挂接。

- `AppDelegate.h`
- `AppDelegate.cpp`

职责：

- 初始化 Director / GLView
- 设置分辨率和帧率
- 创建并启动主场景

约束：

- 不在这里写玩法逻辑
- 不在这里写关卡数据处理

### `data`

数据层，负责描述基础数据结构和外部数据读取。

- `CardTypes.h`
- `CardData.h`
- `LevelDefinition.h`
- `LevelLoader.h`
- `LevelLoader.cpp`

职责：

- 定义卡牌基础枚举和运行时卡牌数据
- 定义关卡文件结构
- 从 JSON 读取关卡数据

约束：

- 这里只描述“数据是什么”
- 不负责定义匹配、翻牌、Undo 规则

### `logic`

逻辑层，负责规则、状态变更、关卡检查和可回退命令。

- `GameState.h`
- `GameState.cpp`
- `GameCommand.h`
- `GameCommand.cpp`
- `LevelSessionLoader.h`
- `LevelSessionLoader.cpp`
- `LevelInspector.h`
- `LevelInspector.cpp`
- `LevelSolver.h`
- `LevelSolver.cpp`
- `LevelValidator.h`
- `LevelValidator.cpp`

职责：

- 管理运行时卡牌状态
- 定义抽牌、匹配、翻牌、Undo 恢复规则
- 封装可回退命令
- 校验关卡合法性
- 判断关卡是否可解
- 串联关卡加载和巡检流程

约束：

- 逻辑层不负责卡牌节点绘制
- 逻辑层不直接操作按钮或弹层 UI

### `view`

视图层，负责界面节点、输入分发和动画编排。

- `CardGameScene.h`
- `CardGameScene.cpp`
- `CardView.h`
- `CardView.cpp`
- `GameOverlayView.h`
- `GameOverlayView.cpp`
- `CardAssetCatalog.h`

职责：

- 创建场景和卡牌节点
- 处理点击输入
- 根据状态刷新卡牌位置
- 播放 MoveTo 动画
- 管理底部按钮和通关弹层
- 统一管理卡牌资源路径

约束：

- 视图层调用逻辑层接口，不自行定义业务规则
- 视图层根据状态显示结果，不维护第二套业务状态

## 3. 依赖方向

当前建议的依赖方向如下：

- `app -> view`
- `view -> logic`
- `logic -> data`

数据层不反向依赖逻辑层和视图层。

这样做的原因是：

- 低层稳定，高层可替换
- 规则调整时不需要动显示层
- 显示调整时不需要动数据定义

## 4. 当前核心链路

### 启动链路

1. `app/AppDelegate` 启动应用
2. 创建 `view/CardGameScene`
3. `CardGameScene` 调用 `logic/LevelSessionLoader`
4. `LevelSessionLoader` 结合 `data/LevelLoader`、`logic/LevelValidator`、`logic/LevelSolver` 获取关卡结果
5. 场景创建视图并刷新布局

### 操作链路

1. 用户点击卡牌或按钮
2. `view/CardGameScene` 识别输入
3. 创建 `logic/GameCommand`
4. `GameCommand` 调用 `logic/GameState`
5. `GameState` 修改状态并生成 `GameDelta`
6. 场景根据新状态统一刷新全部视图

### Undo 链路

1. 场景从 `_undoStack` 取出最后一个命令
2. 命令调用 `undo()`
3. `GameState` 依据 `GameDelta` 恢复状态
4. 场景重新刷新布局和动画

## 5. 维护建议

后续开发时建议按下面的边界修改：

- 新关卡、坐标、牌序调整：优先改 `data` 和 `Resources/res/levels/*.json`
- 新规则：改 `logic/GameState.*`
- 新回退类型：改 `logic/GameCommand.*` 和 `logic/GameState.*`
- 新校验规则：改 `logic/LevelValidator.*`
- 新可解性策略：改 `logic/LevelSolver.*`
- 新界面表现：改 `view/*`

## 6. 结构价值

这套结构的核心价值是让修改范围更可控：

- 改数据时，不容易误伤 UI
- 改 UI 时，不容易改坏规则
- 扩展 Undo 时，有固定接入点
- 关卡制作可以继续走 JSON 驱动

如果后续继续增加功能，建议保持这套分层，不要把新逻辑重新堆回场景类。

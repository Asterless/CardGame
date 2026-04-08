# 卡牌原型项目说明

## 项目简介

这是一个基于 Cocos2d-x 3.17 的卡牌原型项目，当前实现了以下核心功能：

- 主牌区点击匹配消除
- 备用牌堆翻牌到手牌区
- Undo / Reset
- 主牌区清空后的通关弹层
- 关卡数据 JSON 化
- 关卡合法性校验
- 关卡可解性检查

当前版本主要用于玩法验证和后续需求承接，不是完整商业版本。

## 开发环境

- 引擎：Cocos2d-x 3.17
- 平台：Win32
- 工程：`proj.win32/card.sln`
- 编译工具：Visual Studio 2022 Build Tools / MSBuild

当前工程已经补充 `/utf-8` 编译参数，支持在源码中保留少量中文注释。

## 运行方式

### 编译

在项目根目录执行：

```powershell
MSBuild .\proj.win32\card.sln /m "/p:Configuration=Debug;Platform=Win32" /t:card
```

### 可执行文件

编译输出：

```text
proj.win32\Debug.win32\card.exe
```

## 当前目录说明

- `Classes/CardGameScene.*`
  - 场景编排、输入分发、牌位刷新

- `Classes/cardgame/GameState.*`
  - 核心玩法规则、状态变更、翻牌逻辑、Undo 数据生成

- `Classes/cardgame/GameCommand.*`
  - 命令模式封装，每个用户操作对应一个可回退命令

- `Classes/cardgame/CardView.*`
  - 单张卡牌的显示逻辑

- `Classes/cardgame/GameOverlayView.*`
  - 底部按钮和通关弹层 UI

- `Classes/cardgame/LevelSessionLoader.*`
  - 关卡加载入口，负责串联巡检、关卡选择和 fallback

- `Classes/tools/LevelDefinition.h`
  - 关卡数据结构定义

- `Classes/tools/LevelLoader.*`
  - 关卡 JSON 读取

- `Classes/tools/LevelValidator.*`
  - 关卡合法性校验

- `Classes/tools/LevelSolver.*`
  - 关卡可解性检查

- `Classes/tools/LevelInspector.*`
  - 单关 / 批量关卡巡检入口

- `Resources/res/levels/demo_level.json`
  - 当前示例关卡

- `PROGRAM_DESIGN.md`
  - 结构设计、扩展方式、维护建议

## 玩法说明

### 主牌区

- 主牌区中的明牌可点击
- 被压住的牌不可点击
- 上层牌被清除后，下层牌自动翻开

### 手牌区

- 手牌区顶部牌作为当前比较基准
- 主牌区卡牌只要与顶部牌点数相差 `1` 即可匹配
- 不限制花色

### 备用牌堆

- 当主牌区没有可消除牌时，可以从备用牌堆翻一张新牌到手牌区

### 回退

- 支持连续 Undo
- 每一步 Undo 都会按相反方向将牌移动回原位置

## 关卡配置

当前关卡定义文件：

- [demo_level.json](./Resources/res/levels/demo_level.json)

关卡文件主要包含两部分：

- 牌数据
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

建议：

- 调关卡内容，优先改 JSON
- 不要把新关卡重新写回场景代码

## 关卡检查机制

程序启动时会做两类检查：

### 1. 合法性校验

由 [LevelValidator.cpp](./Classes/tools/LevelValidator.cpp) 负责，当前会检查：

- 重复卡牌 id
- 丢失引用
- `tableauIndex` 越界
- 卡牌是否被重复放入多个区域
- `blockers` / `children` 双向关系是否一致
- 遮挡关系是否存在环

### 2. 可解性检查

由 [LevelSolver.cpp](./Classes/tools/LevelSolver.cpp) 负责。

如果关卡规则下无法清空主牌区，日志会输出 `clearable: false`。

## 扩展建议

### 新增一张卡牌

优先修改：

- [demo_level.json](./Resources/res/levels/demo_level.json)

通常需要同步更新：

- `cards`
- `tableauSlots` / `stockPile` / `wastePile`
- `tableauPositions`
- `blockers` / `children`

### 新增一种可回退功能

按以下顺序扩展：

1. 在 [GameState.h](./Classes/cardgame/GameState.h) 中增加动作类型
2. 在 `GameDelta` 中增加回退数据
3. 在 [GameState.cpp](./Classes/cardgame/GameState.cpp) 中增加执行和回退接口
4. 在 [GameCommand.h](./Classes/cardgame/GameCommand.h) / [GameCommand.cpp](./Classes/cardgame/GameCommand.cpp) 中新增命令类
5. 在 [CardGameScene.cpp](./Classes/CardGameScene.cpp) 中接入操作入口

## 维护原则

- 规则放在 `GameState`
- 回退放在 `GameCommand + GameDelta`
- 显示放在 `CardView`
- 界面叠层放在 `GameOverlayView`
- 关卡启动流程放在 `LevelSessionLoader`
- 输入和动画编排放在 `CardGameScene`
- 关卡和布局优先数据化，不重新硬编码

## 当前状态

当前工程已具备后续继续承接需求的基础能力：

- 可继续增加关卡
- 可继续增加玩法命令
- 可继续扩展 Undo
- 可继续扩展布局配置

如果需要更细的结构说明，请参考：

- [PROGRAM_DESIGN.md](./PROGRAM_DESIGN.md)

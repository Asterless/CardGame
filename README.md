# 卡牌原型项目说明

## 项目简介

这是一个基于 Cocos2d-x 3.17 的卡牌原型项目，当前已经实现：

- 主牌区点击匹配
- 备用牌堆翻牌到手牌区
- Undo / Reset
- 主牌区清空后的结束弹层
- 关卡数据 JSON 化
- 关卡合法性校验
- 关卡可解性检查

当前版本用于玩法验证和工程结构演示，不是完整商业版本。

## 开发环境

- 引擎：Cocos2d-x 3.17
- 平台：Win32
- 工程：`proj.win32/card.sln`
- 编译工具：Visual Studio 2022 Build Tools / MSBuild

工程已补充 `/utf-8` 编译参数，允许在项目代码中保留必要的中文注释。

## 编译方式

在项目根目录执行：

```powershell
MSBuild .\proj.win32\card.sln /m "/p:Configuration=Debug;Platform=Win32" /t:card
```

编译输出：

```text
proj.win32\Debug.win32\card.exe
```

## 目录说明

- `Classes/CardGameScene.*`
  - 场景装配、输入分发、卡牌刷新、动画编排

- `Classes/cardgame/GameState.*`
  - 核心玩法规则、运行时状态、翻牌逻辑、Undo 状态恢复

- `Classes/cardgame/GameCommand.*`
  - 命令模式封装，每个用户操作对应一个可回退命令

- `Classes/cardgame/CardView.*`
  - 单张卡牌显示逻辑

- `Classes/cardgame/CardAssetCatalog.h`
  - 卡牌资源路径集中管理

- `Classes/cardgame/GameOverlayView.*`
  - 底部按钮和结束弹层

- `Classes/cardgame/LevelSessionLoader.*`
  - 关卡启动入口，串联巡检、加载和 fallback

- `Classes/tools/LevelDefinition.h`
  - 关卡数据结构定义

- `Classes/tools/LevelLoader.*`
  - 关卡 JSON 读取

- `Classes/tools/LevelValidator.*`
  - 关卡合法性校验

- `Classes/tools/LevelSolver.*`
  - 关卡可解性检查

- `Classes/tools/LevelInspector.*`
  - 单关 / 目录级巡检入口

- `Resources/res/levels/demo_level.json`
  - 当前示例关卡

## 当前玩法

### 主牌区

- 只有明牌可点击
- 被压住的牌不可点击
- 上层牌移除后，下层牌自动翻开

### 手牌区

- 手牌区顶部牌是当前比较基准
- 主牌区卡牌和顶部牌点数相差 `1` 即可匹配
- 不限制花色

### 备用牌堆

- 当没有可匹配牌时，可以从备用牌堆翻一张牌到手牌区

### 回退

- 支持连续 Undo
- 每次 Undo 都会恢复运行时状态，并按刷新链路播放回退动画

## 关卡数据

当前关卡定义文件：

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

建议后续继续保持“关卡和布局优先数据化”，不要把新关卡重新硬编码回场景。

## 关卡检查机制

程序启动时会做两类检查：

- `LevelValidator`
  - 检查重复 id、丢失引用、索引越界、区域重复放置、遮挡关系错误等问题

- `LevelSolver`
  - 判断当前规则下是否能够清空主牌区

如果关卡无解，巡检日志会输出 `clearable=false`。

## 扩展说明

### 新增一张卡牌

优先修改：

- `Resources/res/levels/demo_level.json`

通常需要同步更新：

- `cards`
- `tableauSlots` / `stockPile` / `wastePile`
- `tableauPositions`
- `blockers` / `children`

### 新增一种可回退功能

按以下顺序扩展：

1. 在 `GameState.h` 中增加动作类型
2. 在 `GameDelta` 中增加回退数据
3. 在 `GameState.cpp` 中增加执行和回退接口
4. 在 `GameCommand.*` 中新增命令类
5. 在 `CardGameScene.cpp` 中接入输入入口

## 相关文档

- [程序设计说明](./PROGRAM_DESIGN.md)
- [项目面试说明](./INTERVIEW_PROJECT_BRIEF.md)

# 卡牌原型项目说明

## 项目简介

这是一个基于 Cocos2d-x 3.17 的卡牌玩法原型。当前仓库只保留一套受控构建入口：CMake。

已实现内容包括：

- 主牌区点击匹配
- 备牌堆翻牌到手牌区
- `Undo` / `Reset`
- 主牌区清空后的结束弹层
- 关卡数据 JSON 化
- 关卡合法性校验
- 关卡可解性检查

当前版本用于玩法验证和工程结构演示，不是完整商业版本。

## 构建约定

- 引擎：Cocos2d-x 3.17
- 编译器：Visual Studio 2022
- 构建系统：CMake
- C++ 标准：C++14
- 默认 Windows 开发目标：x64

`proj.win32/` 目录现在只保存 Windows 平台入口源码和资源文件，例如 `main.cpp`、`game.rc`、`resource.h`，不再作为独立工程入口维护。

## 配置与编译

在仓库根目录执行：

```powershell
cmake --preset x64-debug
cmake --build --preset x64-debug --target card
```

如果需要 32 位构建：

```powershell
cmake --preset win32-debug
cmake --build --preset win32-debug --target card
```

发布构建可使用：

```powershell
cmake --preset x64-release
cmake --build --preset x64-release --target card
```

默认输出路径示例：

- `build/x64-debug/bin/card/Debug/card.exe`
- `build/win32-debug/bin/card/Debug/card.exe`

## 目录说明

- `Classes/view/CardGameScene.*`
  - 场景装配、输入分发、卡牌刷新、动画编排
- `Classes/logic/GameState.*`
  - 核心玩法规则、运行时状态、翻牌逻辑、Undo 状态恢复
- `Classes/logic/GameCommand.*`
  - 命令模式封装，每个用户操作对应一个可回退命令
- `Classes/view/CardView.*`
  - 单张卡牌显示逻辑
- `Classes/view/CardAssetCatalog.h`
  - 卡牌资源路径集中管理
- `Classes/view/GameOverlayView.*`
  - 底部按钮和结束弹层
- `Classes/logic/LevelSessionLoader.*`
  - 关卡启动入口，串联巡检、加载和 fallback
- `Classes/data/LevelDefinition.h`
  - 关卡数据结构定义
- `Classes/data/LevelLoader.*`
  - 关卡 JSON 读取
- `Classes/logic/LevelValidator.*`
  - 关卡合法性校验
- `Classes/logic/LevelSolver.*`
  - 关卡可解性检查
- `Classes/logic/LevelInspector.*`
  - 单关 / 目录级巡检入口
- `Classes/app/AppDelegate.*`
  - 应用启动入口
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

### 备牌堆

- 当没有可匹配牌时，可以从备牌堆翻一张牌到手牌区

### 回退

- 支持连续 `Undo`
- 每次 `Undo` 都会恢复运行时状态，并按刷新链路播放回退动画

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

建议继续保持“关卡和布局优先数据化”，不要把新关卡重新硬编码回场景。

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

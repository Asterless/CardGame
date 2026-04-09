#ifndef CARDGAME_GAME_COMMAND_H
#define CARDGAME_GAME_COMMAND_H

#include "logic/GameState.h"
#include "support/Optional.h"

#include <memory>

namespace cardgame
{
    class GameCommand
    {
    public:
        virtual ~GameCommand() = default;

        virtual bool execute(GameState &state) = 0;
        virtual bool undo(GameState &state) = 0;

        const Optional<GameDelta> &getDelta() const;

    protected:
        // execute 时记录一次 delta，undo 时只依赖这份数据恢复，避免回退逻辑猜状态。
        Optional<GameDelta> _delta;
    };

    class DrawStockCommand : public GameCommand
    {
    public:
        bool execute(GameState &state) override;
        bool undo(GameState &state) override;
    };

    // 主牌区匹配命令：负责把一张可匹配牌移到手牌区顶部，并记录回退信息。
    class MatchTableauCommand : public GameCommand
    {
    public:
        explicit MatchTableauCommand(int cardId);

        bool execute(GameState &state) override;
        bool undo(GameState &state) override;

    private:
        int _cardId;
    };

    using GameCommandPtr = std::shared_ptr<GameCommand>;
}

#endif

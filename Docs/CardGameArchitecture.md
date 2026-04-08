# Card Match Prototype Architecture

## Overview

This prototype keeps data, rules, view rendering, and undo history separate so the scene stays thin and future features can grow without rewriting the whole demo.

- `GameState`
  Owns all card data and the rule checks for stock draw, tableau match, reveal, and undo state restoration.
- `GameCommand`
  Wraps one reversible action. `DrawStockCommand` and `MatchTableauCommand` both produce a `GameDelta`, and the same delta is used for undo.
- `CardView`
  Renders a single card node and knows nothing about rules.
- `CardAssetCatalog`
  Centralizes all card art paths so resource reorganization does not leak into gameplay code.
- `CardGameScene`
  Works as the composition root. It receives taps, runs commands, updates the undo stack, and asks every `CardView` to animate to the layout that the model now describes.

## Resource Structure

Card art now lives under `Resources/res/cards/` so every target platform packages the same runtime paths.

- `Resources/res/cards/frame/card_front.png`
  Shared front frame.
- `Resources/res/cards/suits/*.png`
  Suit icons.
- `Resources/res/cards/numbers/big/{red|black}/*.png`
  Center rank art.
- `Resources/res/cards/numbers/small/{red|black}/*.png`
  Corner rank art.

## Current Data Flow

1. The scene receives a tap.
2. The scene decides which command to create.
3. The command updates `GameState`.
4. The scene pushes the command onto `_undoStack`.
5. The scene recalculates every card target position and runs a simple `MoveTo`.
6. Undo pops the last command and calls `undo()` on it.

## How To Add A New Card

For this demo, cards are seeded in `GameState::createDemoState()`.

To add one more card:

1. Create a new `CardData`.
2. Assign its initial zone:
   `Tableau`, `Stock`, or `Waste`.
3. If it starts in tableau, give it a `tableauIndex`.
4. If it starts covered, set `faceUp = false` and register its `blockers`.
5. If removing it should reveal another card, append that child id into `children`.
6. Add the card id into the corresponding pile container.

If the card count grows beyond this handcrafted layout, the next step should be to move tableau coordinates from `CardGameScene` into a dedicated layout config object or data table.

## How To Add A New Undo Type

Add a new reversible action by following the existing command pattern.

1. Extend `GameActionType` and `GameDelta` if the new action needs extra restore data.
2. Add `execute` and `undo` methods to `GameState`.
3. Implement a new `GameCommand` subclass that delegates to those methods.
4. Create that command inside the scene when the corresponding user input happens.

Because animation is layout-driven, most new undo types do not need custom reverse animation code. As long as undo restores the model, `refreshAllViews()` automatically plays the cards back to their previous positions.

#include "logic/LevelValidator.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>

namespace cardgame
{
    namespace
    {
        void addError(std::vector<std::string> &outErrors, const std::string &message)
        {
            outErrors.push_back(message);
        }

        bool containsId(const std::set<int> &ids, int value)
        {
            return ids.find(value) != ids.end();
        }

        bool containsValue(const std::vector<int> &values, int target)
        {
            return std::find(values.begin(), values.end(), target) != values.end();
        }

        using CardLookup = std::map<int, std::size_t>;

        CardLookup buildCardLookup(const LevelDefinition &definition)
        {
            CardLookup lookup;
            for (std::size_t index = 0; index < definition.cards.size(); ++index)
            {
                lookup[definition.cards[index].id] = index;
            }

            return lookup;
        }

        void validateCardDefinitions(const LevelDefinition &definition, std::set<int> &outCardIds, std::vector<std::string> &outErrors)
        {
            for (const auto &card : definition.cards)
            {
                if (card.id < 0)
                {
                    addError(outErrors, "Card id must be non-negative.");
                    continue;
                }
                if (!outCardIds.insert(card.id).second)
                {
                    std::ostringstream stream;
                    stream << "Duplicate card id: " << card.id;
                    addError(outErrors, stream.str());
                }
            }
        }

        void validateContainers(const LevelDefinition &definition, const std::set<int> &cardIds, std::map<int, int> &outPlacementCounts, std::vector<std::string> &outErrors)
        {
            for (const int cardId : definition.tableauSlots)
            {
                if (cardId < 0)
                {
                    continue;
                }
                if (!containsId(cardIds, cardId))
                {
                    std::ostringstream stream;
                    stream << "tableauSlots references missing card id: " << cardId;
                    addError(outErrors, stream.str());
                    continue;
                }
                ++outPlacementCounts[cardId];
            }

            for (const int cardId : definition.stockPile)
            {
                if (!containsId(cardIds, cardId))
                {
                    std::ostringstream stream;
                    stream << "stockPile references missing card id: " << cardId;
                    addError(outErrors, stream.str());
                    continue;
                }
                ++outPlacementCounts[cardId];
            }

            for (const int cardId : definition.wastePile)
            {
                if (!containsId(cardIds, cardId))
                {
                    std::ostringstream stream;
                    stream << "wastePile references missing card id: " << cardId;
                    addError(outErrors, stream.str());
                    continue;
                }
                ++outPlacementCounts[cardId];
            }
        }

        void validateCardRelations(const LevelDefinition &definition, const std::set<int> &cardIds, const std::map<int, int> &placementCounts, std::vector<std::string> &outErrors)
        {
            const CardLookup cardLookup = buildCardLookup(definition);

            for (const auto &card : definition.cards)
            {
                const auto placementIt = placementCounts.find(card.id);
                const int placementCount = placementIt == placementCounts.end() ? 0 : placementIt->second;
                if (placementCount != 1)
                {
                    std::ostringstream stream;
                    stream << "Card id " << card.id << " must appear exactly once across tableauSlots/stockPile/wastePile.";
                    addError(outErrors, stream.str());
                }

                if (card.zone == CardZone::Tableau)
                {
                    if (card.tableauIndex < 0 || card.tableauIndex >= static_cast<int>(definition.tableauSlots.size()))
                    {
                        std::ostringstream stream;
                        stream << "Tableau card id " << card.id << " has invalid tableauIndex: " << card.tableauIndex;
                        addError(outErrors, stream.str());
                    }
                    else if (definition.tableauSlots[card.tableauIndex] != card.id)
                    {
                        std::ostringstream stream;
                        stream << "Tableau card id " << card.id << " does not match tableauSlots[" << card.tableauIndex << "].";
                        addError(outErrors, stream.str());
                    }
                }
                else if (card.tableauIndex != -1)
                {
                    std::ostringstream stream;
                    stream << "Non-tableau card id " << card.id << " must use tableauIndex = -1.";
                    addError(outErrors, stream.str());
                }

                for (const int blockerId : card.blockers)
                {
                    if (!containsId(cardIds, blockerId))
                    {
                        std::ostringstream stream;
                        stream << "Card id " << card.id << " references missing blocker id: " << blockerId;
                        addError(outErrors, stream.str());
                    }
                    else if (blockerId == card.id)
                    {
                        std::ostringstream stream;
                        stream << "Card id " << card.id << " cannot block itself.";
                        addError(outErrors, stream.str());
                    }
                    else
                    {
                        const auto blockerIt = cardLookup.find(blockerId);
                        if (blockerIt != cardLookup.end() && !containsValue(definition.cards[blockerIt->second].children, card.id))
                        {
                            std::ostringstream stream;
                            stream << "Blocker relation mismatch: blocker " << blockerId << " must list child " << card.id << ".";
                            addError(outErrors, stream.str());
                        }
                    }
                }

                for (const int childId : card.children)
                {
                    if (!containsId(cardIds, childId))
                    {
                        std::ostringstream stream;
                        stream << "Card id " << card.id << " references missing child id: " << childId;
                        addError(outErrors, stream.str());
                    }
                    else if (childId == card.id)
                    {
                        std::ostringstream stream;
                        stream << "Card id " << card.id << " cannot list itself as child.";
                        addError(outErrors, stream.str());
                    }
                    else
                    {
                        const auto childIt = cardLookup.find(childId);
                        if (childIt != cardLookup.end() && !containsValue(definition.cards[childIt->second].blockers, card.id))
                        {
                            std::ostringstream stream;
                            stream << "Child relation mismatch: child " << childId << " must list blocker " << card.id << ".";
                            addError(outErrors, stream.str());
                        }
                    }
                }
            }
        }

        bool detectCycleDfs(
            int nodeId,
            const std::map<int, std::vector<int>> &adjacency,
            std::map<int, int> &visitState,
            std::vector<int> &stack,
            std::vector<int> &outCycle)
        {
            visitState[nodeId] = 1;
            stack.push_back(nodeId);

            std::map<int, std::vector<int>>::const_iterator adjacencyIt = adjacency.find(nodeId);
            if (adjacencyIt != adjacency.end())
            {
                const std::vector<int> &nextNodes = adjacencyIt->second;
                for (const int nextId : nextNodes)
                {
                    const int nextState = visitState[nextId];
                    if (nextState == 0)
                    {
                        if (detectCycleDfs(nextId, adjacency, visitState, stack, outCycle))
                        {
                            return true;
                        }
                    }
                    else if (nextState == 1)
                    {
                        std::vector<int>::iterator cycleBegin = std::find(stack.begin(), stack.end(), nextId);
                        if (cycleBegin != stack.end())
                        {
                            outCycle.assign(cycleBegin, stack.end());
                        }
                        outCycle.push_back(nextId);
                        return true;
                    }
                }
            }

            stack.pop_back();
            visitState[nodeId] = 2;
            return false;
        }

        void validateAcyclicRelations(const LevelDefinition &definition, std::vector<std::string> &outErrors)
        {
            const CardLookup cardLookup = buildCardLookup(definition);
            std::map<int, std::vector<int>> blockerAdjacency;
            std::map<int, std::vector<int>> childAdjacency;

            for (const auto &card : definition.cards)
            {
                blockerAdjacency[card.id];
                childAdjacency[card.id];

                for (const int blockerId : card.blockers)
                {
                    if (cardLookup.find(blockerId) != cardLookup.end())
                    {
                        blockerAdjacency[blockerId].push_back(card.id);
                    }
                }

                for (const int childId : card.children)
                {
                    if (cardLookup.find(childId) != cardLookup.end())
                    {
                        childAdjacency[card.id].push_back(childId);
                    }
                }
            }

            std::map<int, int> visitState;
            std::vector<int> stack;
            std::vector<int> cycle;
            for (const auto &entry : cardLookup)
            {
                visitState[entry.first] = 0;
            }

            for (const auto &entry : cardLookup)
            {
                if (visitState[entry.first] != 0)
                {
                    continue;
                }
                if (detectCycleDfs(entry.first, blockerAdjacency, visitState, stack, cycle))
                {
                    std::ostringstream stream;
                    stream << "Cycle detected in blocker relations:";
                    bool isFirstCycleNode = true;
                    for (const int cycleId : cycle)
                    {
                        stream << (isFirstCycleNode ? " " : " -> ") << cycleId;
                        isFirstCycleNode = false;
                    }
                    addError(outErrors, stream.str());
                    break;
                }
            }

            visitState.clear();
            stack.clear();
            cycle.clear();
            for (const auto &entry : cardLookup)
            {
                visitState[entry.first] = 0;
            }

            for (const auto &entry : cardLookup)
            {
                if (visitState[entry.first] != 0)
                {
                    continue;
                }
                if (detectCycleDfs(entry.first, childAdjacency, visitState, stack, cycle))
                {
                    std::ostringstream stream;
                    stream << "Cycle detected in child relations:";
                    bool isFirstCycleNode = true;
                    for (const int cycleId : cycle)
                    {
                        stream << (isFirstCycleNode ? " " : " -> ") << cycleId;
                        isFirstCycleNode = false;
                    }
                    addError(outErrors, stream.str());
                    break;
                }
            }
        }

        void validateLayout(const LevelDefinition &definition, std::vector<std::string> &outErrors)
        {
            if (!definition.layout.tableauPositions.empty() &&
                definition.layout.tableauPositions.size() < definition.tableauSlots.size())
            {
                std::ostringstream stream;
                stream << "layout.tableauPositions must contain at least " << definition.tableauSlots.size() << " entries.";
                addError(outErrors, stream.str());
            }
        }
    }

    bool LevelValidator::validate(const LevelDefinition &definition, std::vector<std::string> &outErrors)
    {
        outErrors.clear();

        std::set<int> cardIds;
        std::map<int, int> placementCounts;

        validateCardDefinitions(definition, cardIds, outErrors);
        validateContainers(definition, cardIds, placementCounts, outErrors);
        validateCardRelations(definition, cardIds, placementCounts, outErrors);
        validateAcyclicRelations(definition, outErrors);
        validateLayout(definition, outErrors);

        return outErrors.empty();
    }
}

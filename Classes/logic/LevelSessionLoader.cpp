#include "logic/LevelSessionLoader.h"

#include "logic/LevelInspector.h"

#include <sstream>

namespace cardgame
{
namespace
{
std::string joinMessages(const std::vector<std::string>& messages)
{
    std::ostringstream stream;
    for (std::size_t i = 0; i < messages.size(); ++i)
    {
        if (i > 0)
        {
            stream << " | ";
        }
        stream << messages[i];
    }

    return stream.str();
}
}

LevelSessionLoadResult LevelSessionLoader::load(const std::string& levelDirectory, const std::string& levelPath)
{
    LevelSessionLoadResult result;

    const std::vector<LevelInspectionResult> scanResults = LevelInspector::inspectDirectory(levelDirectory);
    for (std::size_t i = 0; i < scanResults.size(); ++i)
    {
        const LevelInspectionResult& scanResult = scanResults[i];
        if (!scanResult.loadSucceeded)
        {
            result.logs.push_back("Level scan failed [" + scanResult.path + "]: " + joinMessages(scanResult.errors));
            continue;
        }

        if (!scanResult.valid)
        {
            result.logs.push_back("Level scan invalid [" + scanResult.path + "][" + scanResult.levelId + "]: " + joinMessages(scanResult.errors));
            continue;
        }

        result.logs.push_back(
            "Level scan ok [" + scanResult.path + "][" + scanResult.levelId + "]: clearable=" +
            std::string(scanResult.solvable ? "true" : "false"));
    }

    const LevelInspectionResult inspection = LevelInspector::inspectFile(levelPath);
    if (!inspection.loadSucceeded)
    {
        result.state = GameState::createDemoState();
        result.errors = inspection.errors;
        return result;
    }

    if (!inspection.valid)
    {
        result.state = GameState::createDemoState();
        result.errors = inspection.errors;
        return result;
    }

    result.definition = inspection.definition;
    result.hasLoadedDefinition = true;
    result.solvable = inspection.solvable;
    result.state = GameState::createFromLevelDefinition(inspection.definition);
    return result;
}
}

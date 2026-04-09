#include "logic/LevelInspector.h"

#include "data/LevelLoader.h"
#include "logic/LevelSolver.h"
#include "logic/LevelValidator.h"

#include "cocos2d.h"

#include <algorithm>

namespace cardgame
{
    namespace
    {
        bool isJsonFile(const std::string &path)
        {
            if (path.size() < 5)
            {
                return false;
            }

            const std::string extension = path.substr(path.size() - 5);
            return extension == ".json";
        }

        bool isDirectoryPath(const std::string &path)
        {
            return !path.empty() && (path[path.size() - 1] == '/' || path[path.size() - 1] == '\\');
        }

        std::string ensureTrailingSlash(const std::string &path)
        {
            if (path.empty() || isDirectoryPath(path))
            {
                return path;
            }

            return path + "/";
        }

        std::string resolveDirectoryPath(const std::string &directory)
        {
            cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
            const std::string normalizedDirectory = ensureTrailingSlash(directory);

            if (normalizedDirectory.empty())
            {
                return "";
            }

            if (fileUtils->isDirectoryExist(normalizedDirectory))
            {
                return normalizedDirectory;
            }

            const std::vector<std::string> &searchPaths = fileUtils->getSearchPaths();
            for (std::size_t i = 0; i < searchPaths.size(); ++i)
            {
                const std::string candidate = ensureTrailingSlash(searchPaths[i]) + normalizedDirectory;
                if (fileUtils->isDirectoryExist(candidate))
                {
                    return candidate;
                }
            }

            const std::string defaultRootCandidate = ensureTrailingSlash(fileUtils->getDefaultResourceRootPath()) + normalizedDirectory;
            if (fileUtils->isDirectoryExist(defaultRootCandidate))
            {
                return defaultRootCandidate;
            }

            return "";
        }
    }

    LevelInspectionResult LevelInspector::inspectFile(const std::string &path)
    {
        LevelInspectionResult result;
        result.path = path;

        std::string loadErrorMessage;
        if (!LevelLoader::loadFromJsonFile(path, result.definition, &loadErrorMessage))
        {
            result.errors.push_back(loadErrorMessage);
            return result;
        }

        result.loadSucceeded = true;
        result.levelId = result.definition.levelId;
        result.valid = LevelValidator::validate(result.definition, result.errors);
        if (result.valid)
        {
            result.solvable = LevelSolver::canClearTableau(result.definition);
        }

        return result;
    }

    std::vector<LevelInspectionResult> LevelInspector::inspectDirectory(const std::string &directory)
    {
        std::vector<LevelInspectionResult> results;
        const std::string resolvedDirectory = resolveDirectoryPath(directory);
        if (resolvedDirectory.empty())
        {
            return results;
        }

        std::vector<std::string> paths = cocos2d::FileUtils::getInstance()->listFiles(resolvedDirectory);

        for (std::size_t i = 0; i < paths.size(); ++i)
        {
            const std::string &path = paths[i];
            if (isDirectoryPath(path) || !isJsonFile(path))
            {
                continue;
            }

            results.push_back(inspectFile(path));
        }

        std::sort(results.begin(), results.end(), [](const LevelInspectionResult &lhs, const LevelInspectionResult &rhs)
                  { return lhs.path < rhs.path; });

        return results;
    }
}

#include <cstdlib>
#include <iostream>
#include <string>

#include "core/Game.hpp"
#include "mvc/SFMLGameController.hpp"
#include "mvc/SFMLView.hpp"

namespace
{
    PlayerControlMode parseMode(const std::string& value)
    {
        if (value == "ai" || value == "AI")
        {
            return PlayerControlMode::AI;
        }

        return PlayerControlMode::Human;
    }

    int parsePositiveInt(const std::string& value, int fallback)
    {
        try
        {
            const int parsed = std::stoi(value);
            return parsed > 0 ? parsed : fallback;
        }
        catch (...)
        {
            return fallback;
        }
    }

    unsigned int parsePositiveUnsigned(const std::string& value, unsigned int fallback)
    {
        try
        {
            const int parsed = std::stoi(value);
            return parsed > 0 ? static_cast<unsigned int>(parsed) : fallback;
        }
        catch (...)
        {
            return fallback;
        }
    }

    void printUsage()
    {
        std::cout
            << "Usage:\n"
            << "  ./projet-echec-yalta [--red human|ai] [--blue human|ai] [--green human|ai]\n"
            << "                        [--depth N] [--threads N]\n\n"
            << "Exemples:\n"
            << "  ./projet-echec-yalta\n"
            << "  ./projet-echec-yalta --red human --blue ai --green ai --depth 3 --threads 4\n"
            << "  ./projet-echec-yalta --red ai --blue ai --green ai --depth 2 --threads 8\n";
    }
}

int main(int argc, char** argv)
{
    SFMLControllerConfig config;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];

        if (arg == "--help" || arg == "-h")
        {
            printUsage();
            return EXIT_SUCCESS;
        }

        if (arg == "--red" && i + 1 < argc)
        {
            config.redMode = parseMode(argv[++i]);
            continue;
        }

        if (arg == "--blue" && i + 1 < argc)
        {
            config.blueMode = parseMode(argv[++i]);
            continue;
        }

        if (arg == "--green" && i + 1 < argc)
        {
            config.greenMode = parseMode(argv[++i]);
            continue;
        }

        if (arg == "--depth" && i + 1 < argc)
        {
            config.aiDepth = parsePositiveInt(argv[++i], config.aiDepth);
            continue;
        }

        if (arg == "--threads" && i + 1 < argc)
        {
            config.aiThreadCount = parsePositiveUnsigned(argv[++i], config.aiThreadCount);
            continue;
        }
    }

    Game game;
    SFMLView view;
    SFMLGameController controller(game, view, config);

    controller.run();
    return EXIT_SUCCESS;
}
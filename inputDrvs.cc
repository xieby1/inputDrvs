#include "nix/derivations.hh"
#include "nix/store-api.hh"
#include "nix/shared.hh"
#include "CLI/CLI.hpp"
#include <iostream>

using namespace nix;

// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
std::set<std::string> inputDrvs(std::string drv_path, bool ignoreNoneExist=false) {
    if (!hasEnding(drv_path, ".drv")) {
        std::cerr << "[Skipped] " << drv_path << " seems not a .drv file." << std::endl;
        return {};
    }

    ref<Store> store(openStore("dummy://"));
    ExperimentalFeatureSettings mockXpSettings;

    std::ifstream file(drv_path);
    if (!file.is_open()) {
        if (ignoreNoneExist) {
            return {};
        } else {
            std::cerr << "Failed to open file: " << drv_path << std::endl;
            exit(-1);
        }
    }

    std::ostringstream ss;
    ss << file.rdbuf();

    std::set<std::string> inputdrvs_path;
    Derivation drv = parseDerivation(*store, ss.str(),"whatever", mockXpSettings);
    /* std::cerr << "size: " << drv.inputDrvs.map.size() << std::endl; */
    for (const auto &item : drv.inputDrvs.map) {
        inputdrvs_path.insert("/nix/store/" + std::string(item.first.to_string()));
    }

    return inputdrvs_path;
}
std::set<std::string> inputDrvsFromSet(std::set<std::string> drvs_path) {
    std::set<std::string> inputdrvs_path;
    for (std::string drv_path : drvs_path) {
        inputdrvs_path.merge(inputDrvs(drv_path));
    }
    return inputdrvs_path;
}
std::set<std::string> inputDrvsFromSetRecursive(std::set<std::string> drvs_path) {
    std::set<std::string> haveInspected = {};
    std::set<std::string> toBeInspected = drvs_path;

    while (!toBeInspected.empty()) {
        std::string drv_path = *toBeInspected.begin();
        toBeInspected.erase(toBeInspected.begin());
        toBeInspected.merge(
            inputDrvs(drv_path, true)
        );
        /* std::cerr << "[Inspected] " << drv_path << std::endl; */
        haveInspected.insert(drv_path);
        // TODO: set diff to reduce file reads
    }

    return haveInspected;
}


int main(int argc, char **argv) {
    CLI::App app{"inputDrvs"};

    bool recursive{false};
    app.add_flag("-r,--recursive", recursive, "Recursive find all existing inputDrvs");
    app.allow_extras();

    CLI11_PARSE(app, argc, argv);

    // TODO: remove
    /* std::cerr << "recursive: " << recursive << std::endl; */

    std::vector<std::string> remaining_args = app.remaining(true);

    initNix();

    // convert vector to set
    std::set<std::string> drvs_path = std::set(remaining_args.begin(), remaining_args.end());
    std::set<std::string> inputdrvs_path = recursive ?
        inputDrvsFromSetRecursive(drvs_path) :
        inputDrvsFromSet(drvs_path);
    for (std::string inputdrv_path : inputdrvs_path)
        std::cout << inputdrv_path << std::endl;

    return 0;
}

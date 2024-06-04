#include "nix/derivations.hh"
#include "nix/store-api.hh"
#include "nix/shared.hh"
#include "CLI/CLI.hpp"
#include <iostream>

using namespace nix;

int main(int argc, char **argv) {
    CLI::App app{"inputDrvs"};

    bool recursive{false};
    app.add_flag("-r,--recursive", recursive, "Recursive find all existing inputDrvs");
    app.allow_extras();

    CLI11_PARSE(app, argc, argv);

    // TODO: remove
    std::cerr << "recursive: " << recursive << std::endl;

    std::vector<std::string> remaining_args = app.remaining(true);

    initNix();

    ref<Store> store(openStore("dummy://"));
    ExperimentalFeatureSettings mockXpSettings;
    for (std::string arg : remaining_args) {
        std::ifstream file(arg);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << arg << std::endl;
            exit(-1);
        }
        std::ostringstream ss;
        ss << file.rdbuf();

        Derivation drv = parseDerivation(*store, ss.str(),"whatever", mockXpSettings);
        std::cerr << "size: " << drv.inputDrvs.map.size() << std::endl;
        for (const auto &item : drv.inputDrvs.map) {
            std::cout << item.first.to_string() << std::endl;
        }
    }

    return 0;
}

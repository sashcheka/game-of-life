#include "application.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        life::run_application(argc, argv);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}

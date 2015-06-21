/**
 * @file test.cpp
 * @author Miroslav Cibulka
 * @brief This is sample fail test which checks only
 *        if BFD fails to open file
 */

#include <iostream>
#include <string.h>
#include <bfp.hpp>


int main(int args, const char **argv) {
    auto fd = ::BFP::BFD::get_unique_instance();
    try {
        if (args == 2) {
            fd->Open(*(argv + 1), NULL);
        } else {
            ::std::cout << *argv << " <FILE_NAME>" << ::std::endl;
            return EXIT_FAILURE;
        }
    } catch (::BFP::Exception &ex) {
        ::std::cerr << ex.what();
        return EXIT_SUCCESS;
    }
    delete fd;
    return EXIT_FAILURE;
}
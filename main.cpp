// Hebi - A node for the Hydra Decentralised IRC Bot

/* Main.cpp: Program skeleton
 * Author: frony0
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "irc/connection.hpp"
#include "logger.hpp"

#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);
	log::info << "Starting Hebi" << log::done;
	auto logentry = log::debug << "Args: ";
    for (std::string arg : args)
    {
        logentry << arg;
    }
    logentry << log::done;
    log::info << log::done;

    irc::connection* connPtr = new irc::connection(args[0], stoi(args[1]));
    delete connPtr;
	return EXIT_SUCCESS;
}

// Hebi - A node for the Hydra Decentralised IRC Bot

/* Main.cpp: Program skeleton
 * Author: frony0
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "irc/connection.hpp"
#include "hydra/session.hpp"
#include "logger.hpp"

#include <iostream>
#include <thread>

void work(irc::connection& pConn, hydra::session& pSess)
{
    //pConn.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pSess.connect(std::string("localhost"), HYDRA_PORT);

    while (pConn.running() || true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(60000));
        log::debug << "Main Thread!" << log::done;
        //pConn.stop();
    }
}

void usage(char *pBinary)
{
    std::cout << "Usage: " << pBinary << " <host> <port>" << std::endl;
}

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);
	log::info << "Starting Hebi" << log::done;
	auto logentry = log::debug << "Args:";
    for (std::string arg : args)
    {
        logentry << " " << arg;
    }
    logentry << log::done;
    log::info << log::done;

    if (args.size() < 1)
    {
        usage(argv[0]);
    }
    else
    {
        std::string &host = args[0];
        int port = 6667;
        if (args.size() < 2)
        {
            log::warn << "No port specified, defaulting to 6667" << log::done;
        }
        else
        {
            try 
            {
                port = stoi(args[1]);
            }
            catch (const std::invalid_argument&)
            {
                log::fatal << "Port must be a number" << log::done;
                return EXIT_FAILURE;
            }
            if (port > 65535 || port < 1)
            {
                log::fatal << "Port must be between 1-65535" << log::done;
                return EXIT_FAILURE;
            }
        }
        irc::connection* connPtr = new irc::connection(host, port);
        hydra::session* sessPtr = new hydra::session(HYDRA_PORT);

        work(*connPtr, *sessPtr);

        delete connPtr;
    }
	return EXIT_SUCCESS;
}

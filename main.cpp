// Hebi - A node for the Hydra Decentralised IRC Bot

/* Main.cpp: Program skeleton
 * Author: frony0
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "irc/connection.hpp"
#include "irc/message.hpp"
#include "hydra/session.hpp"
#include "plugin/manager.hpp"
#include "thread.hpp"
#include "logger.hpp"

#include <iostream>
#include <thread>
#include <vector>
#include <memory>

void work(std::shared_ptr<irc::connection> pConn, std::shared_ptr<hydra::session> pSess, std::shared_ptr<plugin::manager> pMngr)
{
    //pConn->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pSess->connect(std::string("localhost"), HYDRA_PORT);

    while (pConn->running() || true)
    {
        std::vector<irc::message> lines = pConn->get();
        for (irc::message msg : lines)
        {
            pMngr->handle(msg);
        }
    }
}

void usage(char *pBinary)
{
    std::cout << "Usage: " << pBinary << " <host> <port>" << std::endl;
}

int main(int argc, char *argv[])
{
	thread::init();

    std::vector<std::string> args(argv + 1, argv + argc);
	logs::info << "Starting Hebi" << logs::done;
	auto logentry = logs::debug << "Args:";
    for (std::string arg : args)
    {
        logentry << " " << arg;
    }
    logentry << logs::done;
    logs::info << logs::done;

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
            logs::warn << "No port specified, defaulting to 6667" << logs::done;
        }
        else
        {
            try 
            {
                port = stoi(args[1]);
            }
            catch (const std::invalid_argument&)
            {
                logs::fatal << "Port must be a number" << logs::done;
                return EXIT_FAILURE;
            }
            if (port > 65535 || port < 1)
            {
                logs::fatal << "Port must be between 1-65535" << logs::done;
                return EXIT_FAILURE;
            }
        }
        std::shared_ptr<hydra::session> sessPtr(new hydra::session(HYDRA_PORT));
        std::shared_ptr<irc::connection> connPtr(new irc::connection(host, port));
        std::shared_ptr<plugin::manager> mngrPtr(new plugin::manager(connPtr, &argc, &argv));

        work(connPtr, sessPtr, mngrPtr);
    }
	return EXIT_SUCCESS;
}

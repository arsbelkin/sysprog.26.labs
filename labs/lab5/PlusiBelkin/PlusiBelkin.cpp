#include <iostream>
#include <mutex>
#include "SRBroker.h"
#include <fstream>

#include "./socketStrategy.h"

using namespace std;


int main()
{
    std::locale::global(std::locale("rus_rus.866"));
    wcin.imbue(std::locale());
	wcout.imbue(std::locale());

    boost::asio::io_context io;
    tcp::acceptor a(io, tcp::endpoint(tcp::v4(), 12345));

    while (true)
    {
        try
        {
            auto socket = std::make_shared<tcp::socket>(io);
            a.accept(*socket);
            thread(SRSocketBelkin::processClient, socket, SRSocketBelkin::sessionID++).detach();
        }
        catch (const std::exception& e)
        {
            std::wcerr << "Exception: " << e.what() << endl;
        }
    }

	SRBrokerBelkin::waitThreads();

    return 0;
}

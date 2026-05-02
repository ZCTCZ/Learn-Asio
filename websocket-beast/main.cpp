#include "Listener.h"

int main()
{
    boost::asio::io_context ioc;
    Listener listener(ioc);
    listener.StartAccept();
    ioc.run();
    return 0;
}

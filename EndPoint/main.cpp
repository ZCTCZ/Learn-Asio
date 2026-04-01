#include "Header/EndPoint.h"

int main()
{
    client_generate_endpoint();
    server_generate_endpoint();
    create_tcp_socket();
    create_tcp_acceptor();
    client_connect_to_server();
    client_dns_connect_to_server();
    use_const_buf();
    use_buf_str();
    return 0;
}

//
// Created by AdminCZ on 2026/3/31.
//

#ifndef ENDPOINT_ENDPOINT_H
#define ENDPOINT_ENDPOINT_H
int client_generate_endpoint();
int server_generate_endpoint();
int create_tcp_socket();
int create_tcp_acceptor();
int client_connect_to_server();
int client_dns_connect_to_server();
#endif //ENDPOINT_ENDPOINT_H

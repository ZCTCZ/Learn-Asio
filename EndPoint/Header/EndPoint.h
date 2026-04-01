//
// Created by AdminCZ on 2026/3/31.
//

#ifndef ENDPOINT_ENDPOINT_H
#define ENDPOINT_ENDPOINT_H
#include <boost/asio/ip/tcp.hpp>
int client_generate_endpoint();
int server_generate_endpoint();
int create_tcp_socket();
int create_tcp_acceptor();
int client_connect_to_server();
int client_dns_connect_to_server();
void use_const_buf();
void use_buf_str();
void use_buf_array();
void write_some_to_socket(boost::asio::ip::tcp::socket& socket);
int send_data_by_write_some();
int send_data_by_send();
int send_data_by_write();
std::string read_some_from_socket(boost::asio::ip::tcp::socket& socket);
int read_data_by_read_some();
int read_data_by_receive();
int read_data_by_read();
#endif //ENDPOINT_ENDPOINT_H

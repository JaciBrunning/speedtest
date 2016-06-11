#include "socket.hpp"

using namespace std;

int Socket::socket_init() {
    #ifdef OS_WIN
        WSADATA wsa_data;
        return WSAStartup(MAKEWORD(1, 1), &wsa_data);
    #else
        return 0;
    #endif
}

Socket::SOCKET Socket::socket_create() {
    Socket::SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    return s;
}

Socket::SOCKET Socket::socket_udp_create() {
    Socket::SOCKET s;
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return s;
}

int Socket::socket_connect(Socket::SOCKET s, string host, int port) {
    Socket::SocketAddress host_addr;
    host_addr.sin_addr.s_addr = inet_addr((char *)host.c_str());
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port);
    return connect(s, (struct sockaddr *)&host_addr, sizeof(host_addr));
}

int Socket::socket_bind(Socket::SOCKET s, int port) {
    Socket::SocketAddress addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    return bind(s, (struct sockaddr *)&addr, sizeof(addr));
}

int Socket::socket_listen(Socket::SOCKET s) {
    return listen(s, 5);
}

Socket::SOCKET Socket::socket_accept(SOCKET s, Socket::SocketAddress *addr) {
    #ifdef OS_WIN
        int len = sizeof(*addr);
    #else
        unsigned int len = sizeof(*addr);
    #endif
    return accept(s, (struct sockaddr *)addr, &len);
}

int Socket::socket_quit() {
  #ifdef OS_WIN
    return WSACleanup();
  #else
    return 0;
  #endif
}

int Socket::socket_close(Socket::SOCKET sock) {
  int status = 0;

  #ifdef OS_WIN
    status = shutdown(sock, SD_BOTH);
    if (status == 0) { status = closesocket(sock); }
  #else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) { status = close(sock); }
  #endif

  return status;
}

string Socket::hostname_to_ip(string hostname) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ((he = gethostbyname((char *)hostname.c_str())) == NULL) {
        return NULL;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) {
        return string(inet_ntoa(*addr_list[i]));
    }
     
    return NULL;
}

int Socket::socket_port(Socket::SocketAddress *addr) {
    return ntohs(addr->sin_port);
}

string Socket::socket_host(Socket::SocketAddress *addr) {
    struct in_addr addr_in = addr->sin_addr;
    return string(inet_ntoa(addr_in));
}

int Socket::ClientSocket::connect() {
    #ifdef SO_NOSIGPIPE
        const int set = 1;
        setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set));
    #endif
    return Socket::socket_connect(_socket, Socket::hostname_to_ip(host), port);
}

int Socket::ClientSocket::close() {
    return Socket::socket_close(_socket);
}

int Socket::ClientSocket::send(const char *buffer, size_t length, int flags) {
    return ::send(_socket, buffer, length, flags);
}

int Socket::ClientSocket::send(const char *buffer, size_t length) {
    #ifdef MSG_NOSIGNAL
        return Socket::ClientSocket::send(buffer, length, MSG_NOSIGNAL);
    #else
        return Socket::ClientSocket::send(buffer, length, 0);
    #endif
}

int Socket::ClientSocket::send(string message) {
    return Socket::ClientSocket::send(message.c_str(), message.length());
}

int Socket::ClientSocket::read(char *buf, size_t length) {
    #ifdef OS_WIN
        return ::recv(_socket, buf, length, 0);
    #else
        return ::read(_socket, buf, length);
    #endif
}

int Socket::ServerSocket::open() {
    int ret = Socket::socket_bind(_socket, port);
    if (ret != 0) {
        return ret;
    }
    ret = Socket::socket_listen(_socket);
    return ret;
}

int Socket::ServerSocket::close() {
    return Socket::socket_close(_socket);
}

Socket::ClientSocket Socket::ServerSocket::accept() {
    Socket::SocketAddress addr;
    Socket::SOCKET sid = Socket::socket_accept(_socket, &addr);
    string host = Socket::socket_host(&addr);
    int port = Socket::socket_port(&addr);
    
    ClientSocket socket(sid);
    socket.host = host;
    socket.port = port;
    return socket;
}
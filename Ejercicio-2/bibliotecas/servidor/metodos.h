#ifndef IMPLEMENTACION_H_INCLUDED
#define IMPLEMENTACION_H_INCLUDED

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>                // inet_ntop

    typedef int socklen_t;
#else
    #include <sys/socket.h>              // socket(), bind(), AF_INET, listen(), accept(), socklen_t, sockaddr_in
    #include <netinet/in.h>              // in_addr, sockaddr_in,
    #include <arpa/inet.h>               // htons(), INADDR_ANY, inet_ntoa(), INET_ADDRSTRLEN, ntohs(), inet_ntop()
    #include <unistd.h>
#endif

#include <string.h>                      // memset()
#include <cerrno>                        // errno
#include <cstring>                       // biblioteca de string
#include <thread>                        // thread
#include <mutex>                         // mutex
#include <iostream>
#include <sstream>                       // Para std::ostringstream
#include <string>                        // Para std::string y std::to_string
#include <sys/types.h>                   // Para tipos como pid_t
#include <cstring>
#include <ctime>                         // Para la hora actual
#include <iomanip>                       // put_time()

#endif // IMPLEMENTACION_H_INCLUDED

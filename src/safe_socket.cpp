//
//  safe_socket.cpp
//
//  Created by Liran on 07/09/2018.
//  Copyright © 2018 LPWS. All rights reserved.
//

#ifdef AVAHI_COMPAT
#include <fcntl.h>
#include <sys/stat.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.hpp"
#include "safe_socket.hpp"

safe_socket::~safe_socket() {
    try {
        ::shutdown(_fd.get_fd(), SHUT_RDWR);
    }
    catch (...) {
        PRINT_ANY_EXCEPTION();
    }
}

int safe_socket::create_socket(SocketType type) {
    return ::socket(AF_INET, static_cast<int>(type), 0);
}

void safe_socket::connect(const address& remote_address) {
    const sockaddr_in sock_addr = remote_address.to_sockaddr_in();
    CHECK_AND_THROW(-1 != ::connect(_fd.get_fd(),
                                    reinterpret_cast<const sockaddr*>(&sock_addr),
                                    sizeof(sock_addr)),
                    "connect failed");
}

void safe_socket::send(const buffer& data) {
    CHECK_AND_THROW(-1 != ::send(_fd.get_fd(),
                                 data.data(),
                                 data.size(),
                                 0),
                    "send failed")
}

buffer safe_socket::recv(size_t bytes_to_read) {
    buffer result(bytes_to_read);
    
    ssize_t bytes_read = ::recv(_fd.get_fd(), result.data(), result.size(), 0);
    CHECK_AND_THROW(-1 != bytes_read, "recv failed");
    
    result.resize(bytes_read);
    return result;
}

#ifdef AVAHI_COMPAT
/* Based upon https://stackoverflow.com/questions/7391079/avahi-dns-sd-compatibility-layer-fails-to-run-browse-callback */
int safe_socket::set_nonblocking(int socket) {
    int flags;
    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* FIXME: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(socket, F_GETFL, 0)))
        flags = 0;
    return fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(_fd, FIOBIO, &flags);
#endif
}
#endif

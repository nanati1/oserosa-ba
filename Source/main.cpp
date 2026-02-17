#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include "Board.h"

#pragma comment(lib, "ws2_32.lib")

SOCKET clients[2];
Board board;

bool alive[2] = { true, true };


void SendAll(const std::string& msg) {
    for (int i = 0; i < 2; i++) {
        if (!alive[i]) continue;

        int r = send(clients[i], msg.c_str(), (int)msg.size(), 0);
        if (r <= 0) {
            alive[i] = false;
            closesocket(clients[i]);
            std::cout << "Client " << i << " disconnected (send)\n";
        }
    }
}

std::string SerializeBoard() {
    std::string s = "BOARD ";
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Stone st = board.Get(x, y);
            s += (st == BLACK ? 'B' :
                st == WHITE ? 'W' : '.');
        }
    }
    s += "\n";
    return s;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(50000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 2);

    std::cout << "Waiting clients...\n";

    for (int i = 0; i < 2; i++) {
        clients[i] = accept(server, nullptr, nullptr);
        std::cout << "Client " << i << " connected\n";
    }

    SendAll("START\n");
    SendAll(SerializeBoard());

    char buf[256];

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);

        SOCKET maxSock = 0;
        for (int i = 0; i < 2; i++) {
			if (!alive[i]) continue;
            FD_SET(clients[i], &readfds);
            if (clients[i] > maxSock) maxSock = clients[i];
        }

        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        if (select((int)maxSock + 1, &readfds, nullptr, nullptr, &tv) > 0) {
            for (int i = 0; i < 2; i++) {
                if (FD_ISSET(clients[i], &readfds)) {
                    int r = recv(clients[i], buf, sizeof(buf) - 1, 0);
                    if (r <= 0) {
                        alive[i] = false;          
                        closesocket(clients[i]);   
                        continue;
                    }

                    buf[r] = 0;

                    int x, y;
                    if (sscanf_s(buf, "PUT %d %d", &x, &y) == 2) {
                        if (board.CanPut(x, y)) {
                            board.Put(x, y);

                            if (!board.HasAnyPuttable())
                                board.Pass();

                            SendAll(SerializeBoard());
                        }
                    }
                }
            }
        }
        Sleep(1);
    }
}
#pragma once

#include "socket_tcp.h"
/*
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

namespace core {
class epoll_service
{
	network::tcp m_socket;
	int m_epoll_fd;
	epoll_event m_events[MAX_EVENTS];

public:
	epoll_service()
		: m_socket(AF_INET)
	{}

	~epoll_service()
	{
		if (-1 != m_epoll_fd)
		{
			::close(m_epoll_fd);
		}
	}

	void Initialize()
	{

	}

	void Finalize()
	{
		if (-1 != m_epoll_fd)
		{
			::close(m_epoll_fd);
			m_epoll_fd = -1;
		}

		m_socket.close();
	}

	bool create()
	{
		m_epoll_fd = ::epoll_create1(0);
		return -1 != m_epoll_fd;
	}

	int get_state()
	{
		return epoll_wait(m_epoll_fd, m_events, MAX_EVENTS, -1);
	}


};

int main() {
	// Create TCP socket
	int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpSocket == -1) {
		std::cerr << "Failed to create TCP socket." << std::endl;
		return 1;
	}

	// Create UDP socket
	int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket == -1) {
		std::cerr << "Failed to create UDP socket." << std::endl;
		return 1;
	}

	// Set up TCP server address
	struct sockaddr_in tcpServerAddr;
	memset(&tcpServerAddr, 0, sizeof(tcpServerAddr));
	tcpServerAddr.sin_family = AF_INET;
	tcpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcpServerAddr.sin_port = htons(8080);

	// Bind TCP socket to server address
	if (bind(tcpSocket, (struct sockaddr*)&tcpServerAddr, sizeof(tcpServerAddr)) == -1) {
		std::cerr << "Failed to bind TCP socket." << std::endl;
		return 1;
	}

	// Set up UDP server address
	struct sockaddr_in udpServerAddr;
	memset(&udpServerAddr, 0, sizeof(udpServerAddr));
	udpServerAddr.sin_family = AF_INET;
	udpServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	udpServerAddr.sin_port = htons(8081);

	// Bind UDP socket to server address
	if (bind(udpSocket, (struct sockaddr*)&udpServerAddr, sizeof(udpServerAddr)) == -1) {
		std::cerr << "Failed to bind UDP socket." << std::endl;
		return 1;
	}

	// Add TCP socket to the epoll instance
	struct epoll_event tcpEvent;
	memset(&tcpEvent, 0, sizeof(tcpEvent));
	tcpEvent.events = EPOLLIN;
	tcpEvent.data.fd = tcpSocket;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, tcpSocket, &tcpEvent) == -1) {
		std::cerr << "Failed to add TCP socket to epoll." << std::endl;
		return 1;
	}

	// Add UDP socket to the epoll instance
	struct epoll_event udpEvent;
	memset(&udpEvent, 0, sizeof(udpEvent));
	udpEvent.events = EPOLLIN;
	udpEvent.data.fd = udpSocket;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, udpSocket, &udpEvent) == -1) {
		std::cerr << "Failed to add UDP socket to epoll." << std::endl;
		return 1;
	}

	struct epoll_event events[MAX_EVENTS];

	while (true) {
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
		if (numEvents == -1) {
			std::cerr << "Failed to wait for events." << std::endl;
			break;
		}
		// Process events
		for (int i = 0; i < numEvents; ++i) {
			if (events[i].data.fd == tcpSocket) {
				// TCP socket event
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				int clientSocket = accept(tcpSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
				if (clientSocket == -1) {
					std::cerr << "Failed to accept TCP connection." << std::endl;
					continue;
				}

				// Handle TCP communication with the client
				char tcpBuffer[BUFFER_SIZE];
				ssize_t bytesRead = recv(clientSocket, tcpBuffer, sizeof(tcpBuffer), 0);
				if (bytesRead == -1) {
					std::cerr << "Failed to receive TCP data." << std::endl;
					close(clientSocket);
					continue;
				}

				// Process the received TCP data
				std::cout << "Received TCP data: " << tcpBuffer << std::endl;

				// Send a response back to the client
				const char* tcpResponse = "TCP Response";
				ssize_t bytesSent = send(clientSocket, tcpResponse, strlen(tcpResponse), 0);
				if (bytesSent == -1) {
					std::cerr << "Failed to send TCP response." << std::endl;
				}

				// Close the client socket
				close(clientSocket);
			}
			else if (events[i].data.fd == udpSocket) {
				// UDP socket event
				char udpBuffer[BUFFER_SIZE];
				struct sockaddr_in clientAddr;
				socklen_t clientAddrLen = sizeof(clientAddr);
				ssize_t bytesRead = recvfrom(udpSocket, udpBuffer, sizeof(udpBuffer), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
				if (bytesRead == -1) {
					std::cerr << "Failed to receive UDP data." << std::endl;
					continue;
				}

				// Process the received UDP data
				std::cout << "Received UDP data: " << udpBuffer << std::endl;

				// Send a response back to the client
				const char* udpResponse = "UDP Response";
				ssize_t bytesSent = sendto(udpSocket, udpResponse, strlen(udpResponse), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
				if (bytesSent == -1) {
					std::cerr << "Failed to send UDP response." << std::endl;
				}
			}
		}
	}

	return 0;
}
} // namespace core
*/
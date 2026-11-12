#include <csignal>
#include <iostream>
#include <cstdlib>
#include "../include/Server.hpp"


int main(int ac, char **av)
{
    if (ac != 3){
        std::cerr << "Usage: " << av[0] << " <porta> <password>" << std::endl;
        return (1);
    }

	std::string port = av[1];
	if (!utils::isValidNumber(port) && port.size() > 4) {
		std::cout << "No valid port!" << std::endl;
		return (1);
	}

    signal(SIGINT, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
	std::string	pass; 
	
	if (ac == 3)
		pass = av[2];

    try {
        Server server(std::atoi(av[1]), pass);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}
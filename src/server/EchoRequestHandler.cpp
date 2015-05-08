//
// Created by David Pena on 5/7/15.
//

#include "EchoRequestHandler.h"

using boost::asio::ip::tcp;

EchoRequestHandler::EchoRequestHandler(socket_ptr sock, std::string echo) {
    this->sock = sock;
    this->echo = echo;
}

/* -------------------- Public -------------------- */

void EchoRequestHandler::launch()
{
    try
    {
        // Make a string to return to the request with the header (%s)
        std::string response_str_format("HTTP/1.0 200 OK\nContent-Type: text/html\n\n"
                                                "<html><body>%s</body></html>");
        // Make an arguments array to pass into the response string
        std::vector<std::string> args;
        args.push_back(this->echo);

        // Format the response string using the request header
        std::string response_str = format_range(response_str_format, args);

        // Echo the request header
        boost::asio::write(*sock, boost::asio::buffer(response_str.c_str(),
                                                      response_str.size()));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

/* -------------------- Private -------------------- */

/**
 * Insert the arguments into the given format string
 */
std::string EchoRequestHandler::format_range(const std::string& format_string,
                                             const std::vector<std::string>& args)
{
    boost::format f(format_string);
    for (std::vector<std::string>::const_iterator it = args.begin();
         it != args.end(); ++it)
    {
        f % *it;
    }
    return f.str();
}
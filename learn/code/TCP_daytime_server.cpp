/* 
    Access a daytime service 
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace std;

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            cerr << "Usage: client <host>" << endl;
            return 1;
        }

        boost::asio::io_service io;

        /* 
            resolver
                -- convert server name (query object) into TCP endpoint
                -- list of endpoints returns an iterator
            resolver::iterator 
                -- default constructor represent end()

        */
        tcp::resolver resolver(io);
        tcp::resolver::query query(argv[1], "daytime");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        /*
            Create and connect the socket
                -- try the list of endpoints
        */
        tcp::socket socket(io);
        boost::asio::connect(socket, endpoint_iterator);

        /*
            connected, do some read/write
                -- when server closes the connection
                -- read_some() will exit with error::eof error, 
        */
        for (;;)
        {
            vector<char> buf(128);
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error); // other eeor

            cout << buf.data() << endl;
        }
    }
    catch (exception &e)
    {
        cerr << e.what() << endl;
    }
}

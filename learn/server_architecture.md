

# Resources 

+ [async io with boost.asio](https://www.youtube.com/watch?v=rwOv_tw2eA4)
    + should use `strand` when writing to socket (in multithreaded situ), since there is just one socket...
    + `buffer`s are essentially _views_ to memory owned by the application, 
        + `buffer` does not own the underlying memory
        + the underlying memory is allocated on stack/heap (owned) by the application.
    + scatter-gather may want to send say `header`, `body`, `data`, ... in one `sock.send(bufs)` efficiently 
    + _who owns client handler_
        + ![](2017-07-26-00-02-29.png)
        + some client handler manager? 
            + delete handler when connection done.. 
            + but poor design... 
        + _answer_
            + if no more work for client handler, then client handler is dead
                + i.e. nothing left to send 
                + i.e. nothing left to read (disconnection from server)
            + so client handler should _own itself_, since it knows best about 
                + buffer allocated for connection
                + local variable, ... connection info, request...
    + _chaining completion handler_ 
        + idea is proceed to next read/write/procedure if previous one is successful, otherwise drop it
+  _chat server_
    + the goal 
        ```cpp 
        asio_generic_sever<chat_handler> server;
        server.start(8888);
        ```
    ```cpp 
    template<typename ConnectionHandler>
    class asio_generic_server {
        using shared_handler_t = shared_ptr<ConnectionHandler>;

        public:
            asio_genes

        private:
            int thread_count_;
            vector<thread> thread_pool_;
            io_service io_service_;
            ip::tcp::acceptor acceptor_;
    }
    ```
        


# Server architecture


### [Asio](http://think-async.com/Asio/asio-1.10.6/doc/)
### [Boost.Asio](http://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio.html) 


#### Overview

+ _core concepts_
    + `io_service`, interface with OS resource 
        ```cpp
        boost::asio::io_service io_service;                 // link to OS's I/O
        boost::asio::ip::tcp::socket socket(io_service);    // I/O object, 
        ```
    + ![](2017-07-25-12-11-33.png)
    + _synchronous connection_
        ```cpp 
        boost::system::error_code ec;
        socket.connect(server_endpoint, ec);               
        /* 
            1. initiates connection
            2. I/O object forwards request to io_service 
            3. io_service calls OS to perform connect operation
            4. OS returns result to io_service
            5. io_service forwarded backup to I/O object 
        */
        ```
    + _asynchronous connection_
        + ![](2017-07-25-12-16-25.png)
        + ![](2017-07-25-12-20-25.png)
        ```cpp
        void completion_handler(const boost::system::error_code& ec);
        socket.async_connect(server_endpoint, completion_handler);
        io_service::run();
        /* 
            1. initiate connect operation by calling I/O object
            2. I/O object forward request to io_service
            3. io_service signal OS for an async connect
            4. Time passes... OS indicate connect completed by placing result on a queue, ready to pick up by io_service
            5. program make call to io_service::run()
            6. io_service dequeue result of operation, translates to error_code, then pass it to completion handler
        ```
+ _Proactor design patter, concurrency without threads_
    + ![](2017-07-25-12-23-15.png)
    + _async operation_ 
        + i.e. async read write on a socket 
    + _async operation processor_
        + execute async operations and queues event on completion event queue when operation completed
        + i.e. `stream_socket_service`
    + _completion event queue_
        + buffers completion events until dequed by async event demultiplexer
    + _completion handler_
        + process result of async operation, 
        + i.e. function objects, 
    + _async event demultiplexer_
        + blocks waiting for events to occur on completion event queue, and return a completed event to its caller
    + _proactor_
        + calls async event demultiplexer to dequeue events 
        + and dispatches the completion handler (i.e. invoke function object) associated with events 
        + i.e. `io_service`
    + _initiator_
        + app-specific code that starts async operation 
        + i..e `basic_stream_socket`
+ _Impl using reactor_
    + _async operation processor_
        + a reactor implemented with `select`, `epoll`, or `kqueue`
        + when it indicates resource is ready to perform an operation, the processor 
            + executes asyn operation and 
            + enqueues associated completion handler on completion event queue
    + _completion event queue_
        + a linked list of completion handlers 
    + _async event demultiplexer_
        + waiting on an evenrt/conditional variable until a completion handler is available in the queue
+ _threads_
    + _safety_  
        + `io_service` is safe 
    + _thread pools_
        + multiple threads may call `io_service::run()`
        + so any thread in the pool may invoke completion handler
    + _internal threads_
        + async completion handlers will only be called from threads that are currently calling `io_service::run()`
+ _threads without explicit locking_
    + _strands_
        + a strictly sequential invocation of event handler (on concurrent invocation, )
        + allows execution of code in multithreaded program without the need for explicit locking 
    + _usage_
        + _implicit_
            + calling `io_service::run()` on a single thread means all event handler execute in an implicit strand
                + since handler are invoked inside `run()` only
        + _explicit_
            + with `io_service::strand()`
            + event handler function object needs to be 
                + wrapped using `io_service::strand::wrap()`
                + or posted/dispatched via `io_service::strand` object
+ _buffers_
    + I/O invovles transfer of data to/from contiguous region of memory, called buffers 
    + _scatter-gather operation_
        + scatter-read receives data into multiple buffers 
        + gather-write transmits multiple buffers
        ```cpp 
        char d1[128];
        vector<char> d2(128);
        boost::array<array, 128> d3;

        boost::array<mutable_buffer, 3> bufs1 = {
            boost::asio::buffer(d1),
            boost::asio::buffer(d2),
            boost::asio::buffer(d3)
        };
        bytes_transferred = sock.receive(bufs1);
        bytes_transferred = sock.send(bufs1);s
        ```
    + _abstraction for a collection of buffers_
        + define a type to represent a single buffer and 
            ```cpp
            typedef pair<void*, size_t> mutable_buffer;
            typedef pair<const void*, size_t> const_buffer;
            ```
        + store them in a container
            + `MutableBufferSequence` and `ConstBufferSequence` define concepts of such container 
            + i.e. `vector`, `list`
    + `mutable_buffer` and `const_buffer` classes 
        + provide opaque representation of contiguous memory
        + `mutable_buffer` can be converted to `const_buffer`
        + protection against buffer overruns
            + ranges not over bound 
        + type-safety violation requested with `buffer_cast`
    + `boost::asio::basic_streambuf` 
        + derived from `std::basic_streambuf`
        + associate input sequence and output sequence with one or more objects of some char array type,
        + methods 
            + `ConstBufferSequence data()`
                + access input sequence of streambuf
            + `const_buffers_type prepare()`
                + return object of type `const_buffers_type` satisfies `ConstBufferSequence` concept, representing all char arrays in input sequence
            + `void commit(size_t )`
                + append `n` chars from start of output sequence to input sequence
                + beginning of output sequence advanced by `n` chars
            + `void consume(size_t n)`
                + remove `n` chars from beginning of input sequence
    + _Bytewise traversal of buffer sequences_ with iterator
        + `buffers_iterator<>`
            + allows traversal of buffer  (`MutableBufferSequence` or `ConstBufferSequence`) as though they were contiguous sequences of bytes
        + read single line from a socket into `string` write 
            ```cpp
            boost::asio::streambuf;

            std::size_t n = boost::asio::read_until(sock, sb, '\n');
            boost::asio::streambuf::const_buffers_type bufs = sb.data();
            std::string line(
                boost::asio::buffers_begin(bufs),
                boost::asio::buffers_begin(bufs) + n);
            ```
+ _streams, short reads, short writes_
    + Many I/O objects are _stream-oriented_
        + no message boundaries, data being transferred is a continuoug sequences of bytes
        + read/write may transfer fewer bytes than requested, (short read/write)
    + _type requirement for stream-oriented IO model_
        + `SyncReadStream`, sync read with `read_some()`
        + `AsyncReadStream`, async read with `async_read_some()`
        + `SyncWriteStream`, sync write with `write_some()`
        + `AsyncWriteStream`, async write with `async_write_some()`
    + _examples_    
        + `ip::tcp::socket::stream<>`
        + `posix::stream_descriptor`
    + _transfer exact number of bytes_
        + When short read/write occurs, 
            + program have to restart operation, and
            + continue to do so until required number of bytes has been transferred
        + use 
            + `read()`, `async_read()`, `write()`, `async_write()`
    + _EOF is an error_
        + can be used to distinguish end of stream from a successful read of size 0
+ _reactor style operations_
    + for use if want to handle IO without using asio
        + i.e. `select` `epoll` manually
    + `null_buffer`, does not return until IO objectis read to perform the operation
+ _line-based operations_
    + protocols line-based
        + elements deliminated by `\r\n` (i.e. HTTP, SMTP, FTP)
    + _solution_ 
        + use `read_until()` and `async_read_until()`
        ```cpp 
        class http_connection {
            void start(){
                boost::asio::async_read_until(socket_, data_, "\r\n",,
                    boost::bind(&http_connection::handle_request_line, this, _1));
            }

            void handle_request_line(boost::system::error_code ec){
                if(!ec)
                {
                    string method, uri, version;
                    char sp1, sp2, cr, lf;
                    istream is(&data_);
                    is.unsetf(std::ios_base::skipws);
                    is >> method >> sp1 >> uri >> sp2 >> version >> cr >> lf;
                }        
            }
            boost::asio::ip::tcp::socket socket_;
            boost::asio::streambuf data_;          
            /* 
                streambuf
                    -- used to store data read from socket before searching for delim
                    -- note there may be additional data after delim, the surplus should be lest in streambuf so that it may be inspected by a subsequent call to read_until()
            */
        }
        ```
        ```cpp 
        class match_char {
            public:
                explict match-char(char c) : c_(c){}

                template<typename Iterator>
                pair<Iterator, bool> operator(
                    Iterator begin, Iterator end
                ) const {
                    Iterator i = begin;
                    while(i != end){
                        if(c_ == *i++)
                            return make_pair(i, true);
                    }
                    return make_pair(i, false);
                }
            private:
                char c_;
        }

        namespace boost::asio {
            template<>
            struct is_match_condition<match_char>
                : pubilc boost::true_type {};
        }

        boost::asio::streambuf b;
        boost::asio::read_until(s, b, match_char('a'));
        ```
+ _custom memory allocation_ 
    + _motivation_
        + async operation rquires allocating objects to store state, and
        + some protocol (ie. half duplex protocol impl HTTP) have a single chain of operation per client 
    + _program should be able to reuse memory for all async operation in a chain_
    + _allocation_ (impl with `new` `delete`)
        + guarantees deallocation occur before the associated handler is invoked 
        + this is so memory can be reused for any new asyn operation started by `handler`
        ```cpp 
        void* pointer = asio_handler_allocate(size, &h);
        asio_handler_deallocate(pointer, size, &h);
        ```
+ _handler tracking_
    + _motivation_ 
        + aid in debuging async programs
    + _usage_ 
        + `#define BOOST_ASIO_ENABLE_HANDLER_TRACKING`
        + `<tag>|<timestamp>|<action>|<description>`
        + `<action>`
            + `>n`, enter handler number `n`
            + `<n`, left handler number `n`
            + `!n`, left ahdnle number `n` due to exception 
            + `~n`,  handler number `n` destroyed without having been invoked, case for unfinished async operation when `io_service` is destroyed
            + `n*m` handler `n` created a new async operation with completion handler `m`
            + `n` handler `n` perform some other operatoin
    + _tooling_
        + [perl script](https://github.com/boostorg/asio/blob/master/tools/handlerviz.pl) post-process debug output

#### Networking 

+ _TCP clients_
        + name resolution using a resolver, 
        + host + service name looked up and converted to endpoints
        ```cpp 
        ip::tcp::resolver resolver(io_service);
        ip::tcp::resolver::query query("www.boost.org", "http");
        ip::tcp::resolver::iterator iter = resolver.resolve(query);
        ip::tcp::resolver::iterator end; // End marker.

        while (iter != end)
        {
            ip::tcp::endpoint endpoint = *iter++;
            std::cout << endpoint << std::endl;
        }
        ```
        + note
            + returns a list of endpoints using `IPv4` and `IPv6` endpoints
    + `connect()` and `async_connect()`
        + try each endpoints in a list until the socket is successfully connected
        ```cpp 
        ip::tcp::socket socket(io_service);
        boost::asio::async_connect(socket, iter,
            boost::bind(&client:handle_connect, this, boost::asio::placeholders::error));
        
        void handle_connect(const error_code& ec){
            if(!ec){
                // read/write operation
            }
        }
        ```
        + given an endpoint
        ```cpp 
        ip::tcp::socket socket(my_io_service);
        socket.connect(endpoint);
        ```
    + `read`, `write`, `async_read`, `async_write`.... 
+ _TCP server_
    + use acceptor to accept incoming connection 
    ```cpp 
    ip::tcp::acceptor acceptor(io_service, my_endpoint);
    ip::tcp::socket socket(io_sevice);
    acceptor.accept(socket);
    ```
+ _socket iostreams_
    + `ip::tcp::iostream stream(host, protocol)`
        + hide away endpoint resolution, 
        + is protocol independent
        ```cpp 
        ip::tcp::iostream stream("www.boost.org", "http");
        if(!stream){
            // cannot connect
        }
        ```
+ _The BSD Socket API and Boost.Asio_
    + inclulow-level socket interface based on BSD socket API

#### C++11 support 

+ _error_
    + `#define ASIO_HAS_STD_SYSTEM_ERROR`
+ _movable IO object_
    + `#define ASIO_HAS_MOVE`
+ _movable IO handler_ 
+ _shared pointer_
    + `#define ASIO_HAS_STD_SHARED_PTR`
+ _atomics_
    + `#define ASIO_HAS_STD_ATOMIC`
+ _array_ 
    + `#define ASIO_HAS_STD_ARRAY`
+ _chrono_
    + `#define ASIO_HAS_STD_CHRONO`

#### SSL 

+ _motivation_ 
    + encrypt communication on top of existing streams



### Boost.Asio API

+ `io_service`
    + goal 
        + implements an extensible, type-safe, polymorphic set of IO services, indexed by service type 
        + manages logical interface to OS on behalf of IO object.
            + i.e. resources shared across a class of IO objects
    + is thread safe 
    + provide core IO functionality including 
        + `boost::asio::ip::{tcp, udp}::{socket, acceptor}`
    + stop `io_service` from running out of work 
        `boost::asio::io_service::work work(io_service)`
+ `io_service::run()`
    + blocks until 
        + all work has finished and there is no more handler to be disaptched
        + or `io_service` stopped
+ `deadline_timer`
    + `async_wait(WaitHandler handler);`
        + initaite async wait against timer, always return immediately 
        + supplied handler called exactly once, when 
            + timer expired 
            + timer cancelled, in which case handler is passed error code `boost::asio::error::operation_aborted`
        + `void handler(const boost::system::error_code& error)`
+ `io_service::strand`
    + provides ability to post/dispatch handlers with guarantee that none of those handlers will execute concurrently 
+ `strand::wrap(Handler)`
    + craete a new handler (function object) that automatically dispatches the wrapped handler on the strand 
    + used to create a new handler object that, when invoked, will automatically pass the wrapped handler to the strand's dispatch function
+ `buffer`
    + represent contiguous region of memory 
    + _construction_ 
        + create from `array`, `vector`, or `boost::array` or POD...
            + with `boost::asio::buffer(data, size)`, note size is deduced from type of `data`, fixed size sequence containers 
        ```cpp 
        char d1[128];
        size_t bytes_transferred = sock.receive(boost::asio::buffer(d1));
        vector<char> d2(128);
        bytes_transferred = sock.receive(boost::asio::buffer(d2));
        array<char, 128> d3;
        bytes_transferred = sock.receive(boost::asio::buffer(d3));
        ```
    + _access_
        + with `buffer_size` and `buffer_cast`
        ```cpp 
        boost::asio::mutable_buffer b1 = ...;
        std::size_t s1 = boost::asio::buffer_size(b1);
        unsigned char* p1 = boost::asio::buffer_case<unsigned char*>(b1);
        
        boost::asio::const_buffer b2 = ...;
        std::size_t s2 = boost::asio::buffer_size(b2);
        const void* p2 = boost::asio::buffer_cast<const void*>(b2);
        ```
    + _usage_ [stackoverflow](https://stackoverflow.com/questions/15060671/boostasiobuffer-getting-the-buffer-size-and-preventing-buffer-overflow)
        + _reading data_
            + use `socket::available()` to determine size of buffer
            ```cpp
            std::vector<char> data(socket_.available());
            boost::asio::read(socket_, boost::asio::buffer(data));
            ```
            + or dynamic length buffer `boost::asio::streambuf()` with `read()` which accepts a `streambuf`
            ```cpp 
            boost::asio::streambuf data;
            boost::asio::read(socket_, data, boost::asio::transfer_at_least(socket_.available()));
            ```
            + variable length protocol, 
                + header is fixed size (containing meta-information)
                + body size is inside header (allocate buffer for body)
            ```cpp 
            // read fixed size header 
            vector<char> data(fixed_header_size);
            boost::asio::read(socket_, boost::asio::buffer(data));

            protocol::header header(data);
            network_to_local(header);       // endianness

            // read body
            data.resize(header.body_length());
            boost::asio::read(socket_, boost::asio::buffer(data));

            protocol::body body(data);
            network_to_local(body);
            ```




### [Python SimpleHTTPserver](https://docs.python.org/2/library/basehttpserver.html#BaseHTTPServer.BaseHTTPRequestHandler.error_message_format)

_server_ 
+ _classes_
    + _A simple HTTPServer_
        + create with 
            + server name + port 
            + a request handler
            ```py
            PORT = 8000
            Handler = SimpleHTTPServer.SimpleHTTPRequestHandler
            httpd = SocketServer.TCPServer(("", PORT), Handler)
            ``` 
        + creates and listens to HTTP socket, dispatch request to handler
    + _request handler_
        + parses header+request+url
            + headers
            + request command (GET)
            + request path
            + input stream
        + call correct handle based on route + Verb
            + base class for handles
        + methods 
            + `handle`
            + `send_error`
            + `send_response`
            + `send_header`, write header to output stream
        + variation 
            + base class 
            + derived class that maps path to directory path
    + _response generator_
        + response
    + _logging_
        + logs traffic
+ _utils_
    + _parser_  
        + parse header+request 
        + parse url
    + _encoder/decoder_
        + encode/decode url
    + _JSON formatter_
        + wrapper around json
    + _error_
        + specifies 
            + format
            + header content (i.e. content-type `text/html`)
        + generates HTTP status code + error message 
+ _relevant terms_
    + _client address_, consisting of host and port
    + 




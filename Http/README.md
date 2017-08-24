

### a simple `Http` library

+ Partial implementation (as much as needed) of HTTP/1.1
    + Request parsing and response writing
    + Uri percent encoding/decoding
+ Compact-trie based router to handle 
    + request-response cycle 
    + middleware stuff

### Todos

- [ ] chunked transfer encoding
- [ ] Multithreaded support 
- [ ] async file serving 
- [ ] HTTPS using openSSL
- [ ] some useful middlewares
    - [ ] body parser
    - [ ] CORS



### Resources

+ configure include path for `c_cpp_properties.json` with `gcc -xc++ -E -v -`
+ [Asio doc](http://think-async.com/Asio/asio-1.10.6/doc/)
+ custom service impl [discuz](https://stackoverflow.com/questions/23887056/trying-to-understand-boost-asio-custom-service-implementationls)
+ [HTTP/1.1 standard](https://www.w3.org/Protocols/rfc2616/rfc2616.html)
+ explanation on [`std::streambuf`](http://en.cppreference.com/w/cpp/io/basic_streambuf)
+ [go.http](https://golang.org/pkg/net/http/)
+ [mozilla doc on HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP/)
+ [sprint.io on CORS](https://spring.io/understanding/CORS)




### a simple `Http` library

+ Partial implementation (as much as needed) of HTTP/1.1
    + Request parsing and response writing
    + Uri percent encoding/decoding
+ Compact-trie based router to handle 
    + request-response cycle 
    + middleware stuff
+ HTTPS with OpenSSL

### Todos

- [ ] chunked transfer encoding
- [ ] Multithreaded support 
- [ ] async file serving 
- [ ] some useful middlewares
    - [ ] body parser
    - [x] CORS



### Resources

+ configure include path for `c_cpp_properties.json` with `gcc -xc++ -E -v -`
+ [Asio doc](http://think-async.com/Asio/asio-1.10.6/doc/)
+ custom service impl [discuz](https://stackoverflow.com/questions/23887056/trying-to-understand-boost-asio-custom-service-implementationls)
+ [HTTP/1.1 standard](https://www.w3.org/Protocols/rfc2616/rfc2616.html)
+ explanation on [`std::streambuf`](http://en.cppreference.com/w/cpp/io/basic_streambuf)
+ [go.http](https://golang.org/pkg/net/http/)
+ [mozilla doc on HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP/)
+ [sprint.io on CORS](https://spring.io/understanding/CORS)



### QA

+ [test HTTPS with curl on mac](https://github.com/curl/curl/issues/283)
+ [stackoverflow: setup server crt key for asio](https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example)
+ [stackoverflow: how to create signed certificate](https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl)
+ [stackoverflow: self-signed certificate with config file](https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl)

```sh 
openssl req -config req.cnf -new -x509 -sha256 -newkey rsa:2048 -keyout key.pem -days 365 -out cert.pem
```
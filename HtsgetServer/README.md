


### Planning 


+ _radix tree_ 
    + space-optimized tree
        + each node that is the only child is merged with its parent
+ _Trie_ 
    + `Node`
        + parent `*Node`
        + children as a map from `char` to `Node*`
        + data `T` 
            + data associated with this node, i.e. the char array from root to this node
    + `Trie`
        + a root `Node`
        + size
    + _operation_ 
        + `merge(iterator pos)`
            + check if `pos` has 1 child, if so merge child to `pos`
        + `insert(key, data) -> pair<iterator, bool>`
            + data appended to linked list at the node 
            + `iterator` points to inserted node
            + may have to split / merge ... 
        + `erase(iterator pos) -> iterator`
            + removes the element 
                + search
                + remove,
                + call `merge` if applicable
        + _search_ `O(k)` where `k` is max length of key string
            + `predecessor`
                + largest string less than a given key 
            + `successor`
                + smallest string larger than a given key
            + `prefixSearch(key) -> string[]`
                + return array of string with common prefix 
            + `find(key) -> pair<Node, bool>`
        + `preorder_traversal()`



+ _RequestRouter_ (mux)
    + responsible for 
        + determine the array of route handlers to execute
    + handle HTTP request
        + consists of path and callbacks (route handler), executed when a request's path matches
    + impl 
        + trie, a search tree to store dynamic set / associative array, where keys are strings
            + `:name` in `router.params`
            + `*filepath` catch all
        + common prefixes, 
    + some methods to impl 
        + `func Handle(pattern string, handler Handler)`
        + `func HandleFunc(pattern string, handler func(ResponseWriter, *Request))`
        + `func ServeFile(w ResponseWriter, r *Request, name string)`
        + `func SetCookie(w ResponseWriter, cookie *Cookie)`
    + _handler_ 
        + responds to an HTTP request
            + write reply headers and data 
+ _connection_ 
    + may have keep track of state for keep-alive ... 
        + [link](https://golang.org/pkg/net/http/#ConnState)
+ _cookie_ 
    + [link](https://golang.org/pkg/net/http/#Cookie)
+ _route_   
    + defined based on matching path 
        + match must be exact
    + if tie exists, use handlers in order of execution
+ _handlers as templates_?
    + Need 
        + construction of middleware should be variadic, difficult to do without templates
    + handlers are any callable given `Request` and `Response`
        + lambdas, functors, so templates reconsile with the types
+ _middlewares_ 
    + function that receives request + response 
    + matching path is relative 
        + act as internal nodes in the routes, 
        + say `use('/usr')` matches `/usr` and `/usr/local`
+ _server_
    + add 
        + timeout for request-response loop 
        + maxheaderbytes, maximum bytes when parsing http header before returning bad request
+ _request_ 
     + represent HTTP request 
        + method 
        + url
            + uri 
            + query parameters
        + protocol version 
        + header, a map between field name and field value
            + `Host` header (or from uri) promoted to `Request.host`    
                + as `host:port`
            + `ContentLength`
            + `TransferEncoding`
        + body 
            + non-empty server side, return EOF if no body present
+ _response_
    + represent response
        + Status `200 OK`
        + StatusCode `200`
        + HTTP versions
        + headers 
        + body 
        + compression ? 
+ _responsewriter_ 
    + used by http handler to construct http response
        + `writeHeader()`
        + `write()` data to connection as part of HTTP reply
            + calls `writeHeader(OK)` before writing 
            + and sets `Content-Type` 
            + reading request body before writing response


### Resources

+ configure include path for `c_cpp_properties.json` with `gcc -xc++ -E -v -`
+ [Asio doc](http://think-async.com/Asio/asio-1.10.6/doc/)
+ custom service impl [discuz](https://stackoverflow.com/questions/23887056/trying-to-understand-boost-asio-custom-service-implementationls)
+ [HTTP/1.1 standard](https://www.w3.org/Protocols/rfc2616/rfc2616.html)
+ explanation on [`std::streambuf`](http://en.cppreference.com/w/cpp/io/basic_streambuf)
+ [go.http](https://golang.org/pkg/net/http/)


### Htsget [protocol](http://samtools.github.io/hts-specs/htsget.html)

+ _a data retrieval API for client/server model_
    - [ ] Support BAM/CRAM data formats
    - [ ] Access to subsets of genomic data (for browsing specific region) to full genomes (calling variants)
    - [ ] Client provide hints of info to be received, server respond with more info but not less
    - [ ] Follow pan-GA4GH standards,
        - [ ] POST, redirects, and non-reads data will follow protobuf3 compatible JSON
+ _essentials_ 
    - [ ] API makes to HTTP(S) endpoints, receive URL-encoded query string param, return JSON outputs
        - [ ] HTTP status 200 for success 
        - [ ] UTF8-encoded JSON in response body, with `application/json` content-type
        - [ ] Server implements chunked transfer encoding 
        - [ ] client/server negotiate HTTP/2 upgrade 
    - [ ] timestamp in response in [ISO 8061](https://www.iso.org/iso-8601-date-and-time-format.html) format (with `<ctime>`)
    - [ ] HTTP response compressed with [RFC 2616](https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html) `transfer-encoding` 
+ _Autentication_ 
    - [ ] Requests authenticated with OAuth2 bearer token, with [RFC 6750](https://tools.ietf.org/html/rfc6750)
        - [ ] client supplies header `Authorization: Bearer xxxx` with each HTTPS request
    - [ ] or just allow non-authenticated requests 
+ _Errors_
    - [ ] Appropriate HTTP code return on error condition 
    - [ ] in case of transient server error (503, other 5xx) client should implement retry logic 
    - [ ] In case of error specific to `htsget` protocol, response body is a JSON of form `{error: errorType, message: msg}`
+ _CORS_
    - [ ] API resources should support [CORS](https://www.w3.org/TR/cors/) for browser support
    - [ ] A request with `Origin` header will be propagated to `Access-Control-Allow-Origin` header of response
+ _Methods_
    - [ ] `GET /reads/<id>`, 
        - [ ] response with json containing a `ticket` allowing caller to obtain data in specified format
        - [ ] Client may need to filter out extraneous records, i.e. want reads overlapping a region but may include reads not overlapping 
        - [ ] Successful request with empty result set still produce valid response in requested format (header + EOF)
        - [ ] `<id>` a string specifying which reads to return, corresponds to `ReadGroupSetIds` in GA4GH API
    - [ ] query parameters implementation
        + `format`: `BAM` or `CRAM`
        + `referenceName`: ref sequence name, i.e. `chr1` 
        + `start`: `uint32_t`, `InvalidInput` if `start` specified and a reference is not, `InvalidRange` if `start > end`
        + `end`: `uint32_t`, `InvalidInput` if `end` specified and a reference is not, `InvalidRange` if `start > end`
        + `fields`: list of fields to include in response (i.e. `fields=QNAME, FLAG, POS` for BAM)
        + `tags`: comma separated list of tags to include, 
        + `notags`
    - [ ] response JSON
        ```js 
        {
            format: "BAM", 
            urls: [ 
                ...{
                    url,          // one URL, either https:// (needs follow-up request) or data:// (data block inlined)
                    headers       // for HTTPS url, for client use for subsequent request to fetch data
                    } 
                ], 
            md5: hex        // hex string of md5 digest of block from concatenating all payload data - url data blocks
        }
        ```
    - [ ] HTTPS data block urls 
        - [ ] percent-encoded path and query, url format [specification](http://www.ietf.org/rfc/rfc2396.txt)
        - [ ] accepts `GET` request 
        - [ ] provide CORS
        - [ ] provide multiple request retries, within reason 
        - [ ] use HTTPS 
        - [ ] Server supply response with `Content-Length` header, chunked transfer encoding, or both. Client should detect response truncation 
        - [ ] client and url endpoint should negotiate HTTP/2 upgrade 
        - [ ] client follow 3xx redirects from URL, 
    - [ ] Inline data block uris
        - [ ] i.e. `data:application/vnd.ga4gh.bam;base64,...` [RFC2397](https://www.ietf.org/rfc/rfc2397.txt)` [data scheme](https://en.wikipedia.org/wiki/Data_URI_scheme)
        - [ ] must use `base64` encoding 
        - [ ] should ignore media type, 
+ _Reliability & performance_
    - [ ] server should divide large payloads to multiple data blocks in `urls` array 
    - [ ] client fetch in parallel `urls`
    - [ ] data blocks <= 1GB 
    - [ ] inline data blocks <= few MB
    



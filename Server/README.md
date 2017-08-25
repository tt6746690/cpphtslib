
# Server impl of Htsget

### Resources

+ _byte serving with `Range` header_ [link](https://developer.mozilla.org/en-US/docs/Web/HTTP/Range_requests)

### Htsget [protocol](http://samtools.github.io/hts-specs/htsget.html)

+ _a data retrieval API for client/server model_
    - [ ] Support BAM/CRAM data formats
    - [x] Access to subsets of genomic data (for browsing specific region) to full genomes (calling variants)
    - [x] Client provide hints of info to be received, server respond with more info but not less
+ _essentials_ 
    - [x] API makes to HTTP(S) endpoints, receive URL-encoded query string param, return JSON outputs
        - [x] HTTP status 200 for success 
        - [x] UTF8-encoded JSON in response body, with `application/json` content-type
        - [ ] Server implements chunked transfer encoding 
        - [ ] client/server negotiate HTTP/2 upgrade 
+ _Autentication_ 
    - [ ] Requests authenticated with OAuth2 bearer token, with [RFC 6750](https://tools.ietf.org/html/rfc6750)
        - [ ] client supplies header `Authorization: Bearer xxxx` with each HTTPS request
    - [x] or just allow non-authenticated requests 
+ _Errors_
    - [x] Appropriate HTTP code return on error condition 
    - [ ] in case of transient server error (503, other 5xx) client should implement retry logic 
    - [x] In case of error specific to `htsget` protocol, response body is a JSON of form `{error: errorType, message: msg}`
+ _CORS_
    - [x] API resources should support [CORS](https://www.w3.org/TR/cors/) for browser support
    - [x] A request with `Origin` header will be propagated to `Access-Control-Allow-Origin` header of response
+ _Methods_
    - [x] `GET /reads/<id>`, 
        - [x] response with json containing a `ticket` allowing caller to obtain data in specified format
        - [ ] Client may need to filter out extraneous records, i.e. want reads overlapping a region but may include reads not overlapping 
        - [x] Successful request with empty result set still produce valid response in requested format (header + EOF)
        - [x] `<id>` a string specifying which reads to return, corresponds to `ReadGroupSetIds` in GA4GH API
    - [x] query parameters implementation
        - [x]  `format`: `BAM` or `CRAM`
        - [x] `referenceName`: ref sequence name, i.e. `chr1` 
        - [x] `start`: `uint32_t`, `InvalidInput` if `start` specified and a reference is not, `InvalidRange` if `start > end`
        - [x] `end`: `uint32_t`, `InvalidInput` if `end` specified and a reference is not, `InvalidRange` if `start > end`
        - [ ] `fields`: list of fields to include in response (i.e. `fields=QNAME, FLAG, POS` for BAM)
        - [ ] `tags`: comma separated list of tags to include, 
        - [ ] `notags`
    - [x] response JSON
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
        - [x] percent-encoded path and query, url format [specification](http://www.ietf.org/rfc/rfc2396.txt)
        - [x] accepts `GET` request 
        - [x] provide CORS
        - [ ] provide multiple request retries, within reason 
        - [x] use HTTPS 
        - [x] Server supply response with `Content-Length` header, chunked transfer encoding, or both. Client should detect response truncation 
        - [ ] client and url endpoint should negotiate HTTP/2 upgrade 
        - [ ] client follow 3xx redirects from URL, 
    - [x] Inline data block uris
        - [x] i.e. `data:application/vnd.ga4gh.bam;base64,...` [RFC2397](https://www.ietf.org/rfc/rfc2397.txt)` [data scheme](https://en.wikipedia.org/wiki/Data_URI_scheme)
        - [x] must use `base64` encoding 
        - [x] should ignore media type, 
+ _Reliability & performance_
    - [x] server should divide large payloads to multiple data blocks in `urls` array 
    - [ ] client fetch in parallel `urls`
    - [ ] data blocks <= 1GB 
    - [ ] inline data blocks <= few MB
    


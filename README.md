### Summary

A server implementation of htsget [protocol](http://samtools.github.io/hts-specs/htsget.html). 


#### FAQ

+ __Dependencies__
    + [Asio](http://think-async.com/Asio)
    + [Catch](https://github.com/philsquared/Catch)
    + [nlohmann/json](https://github.com/nlohmann/json)
    + [OpenSSL](https://github.com/openssl/openssl)
    + [CMake](https://github.com/Kitware/CMake)
    + [Samtools](https://github.com/samtools/samtools)
+ __C++17__
    + flags `-std=c++1z -stdlib=libc++`
    + compiled with `clang-802.0.42` on `macOS`
+ __Build__
    ```sh
    cmake -H. -Bbuild -Wno-error=dev 
    cmake --build build -- -j5
    ```
+ __Run__
    + on localhost
        ```sh 
        ./bin/htsgetserver
        ```
    + check in [browser](http://127.0.0.1:8888/reads/bamtest?format=BAM&referenceName=1&start=10145&end=10150&fields=QNAME,FLAG,POS)

#### Example 

+ _Routes_
    ```sh
    GET
        \       
        1|-/ 
            2|-data/    
                3|-<filename>     
            2|-reads/  
                3|-<id>   
    ```
    + `/reads/<id>`
        + handles incoming request, dispatch tickets in json 
    + `/data/<filename>`
        + handles data transfer, according to byte range
+ _Test with samtools_
    + `samtools view "http://127.0.0.1:8888/reads/bamtest?format=BAM&referenceName=1&start=10145&end=10150"`
    ```sh 
    SOLEXA-1GA-2_2_FC20EMB:5:251:979:328    0       chr1    10145   25      36M     *       0       0       AACCCCTAACCCTAACCCTAACCCTAACCCTAAACT    hhhhHcWhhHTghcKA_ONhAAEEBZE?H?CBC?DA       NM:i:1  X1:i:1  MD:Z:33A2
    SOLEXA-1GA-2_2_FC20EMB:5:102:214:278    0       chr1    10148   25      36M     *       0       0       CCCTAACCCTAACCCTAACCCTAACCCTAACCTAAC    hbfhhhXUYhT_ULZdLRTKNIMIKGLJCHFFJQJN       NM:i:0  X0:i:1  MD:Z:36
    SOLEXA-1GA-2_2_FC20EMB:5:195:284:685    16      chr1    10149   25      36M     *       0       0       CCAAACACTAACCCTAACCCTAACCCTAACCTAACC    ><>B@>?>?D>>?B?D>DBC?E@BDHAKCEKERLOO       NM:i:1  X1:i:1  MD:Z:29A3A2
    SOLEXA-1GA-2_2_FC20EMB:5:35:583:827     0       chr1    10150   25      36M     *       0       0       CTAACCCTAAACCTAACCCTAACCCTAACCTAACCA    hhW_X]MXNOHQQWMILHGIFMJGJLCFGGJAKIEH       NM:i:1  X1:i:1  MD:Z:10A24A0
    ```

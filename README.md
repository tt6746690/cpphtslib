### Summary

A server/client implementation of htsget [protocol](http://samtools.github.io/hts-specs/htsget.html). 


#### Compilation

+ __C++17__
    + flags `-std=c++1z -stdlib=libc++`
    + compiled with `clang-802.0.42` on `macOS`

```sh
cmake -H. -Bbuild 
cmake --build build -- -j3
```

#### Run

```sh 
./bin/htsgetserver
```


#### Dependencies 

+ [Asio](http://think-async.com/Asio)
+ [Catch](https://github.com/philsquared/Catch)
+ [nlohmann/json](https://github.com/nlohmann/json)









### Explore


+ _IGV_ [link](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3603213/) 
    + layers
        + _stream layer_ 
            + providing random access to section of files (`fseek`)
            + different protocols, local, HTTP, FTP...
            + make use of the indexed nature of SAM/BAM files 
        + _data layer_
            + consists of readers for different file formats
        + _application layer_
    + _sam_ [github](https://github.com/igvteam/igv/blob/master/src/org/broad/igv/)
+ _sam file formats_ 
    + [public wiki](http://genome.sph.umich.edu/wiki/SAM)
+ _htsjdk_ [github](https://github.com/samtools/htsjdk/)
    + _seekable streams_ 
        + `read(buffer, offset, length)` 
        + `seek(position)`
        + types
            + `SeekableHTTPStream` [github](https://github.com/samtools/htsjdk/blob/master/src/main/java/htsjdk/samtools/seekablestream/SeekableHTTPStream.java)   
                + `url`, `contentLength`, `URL url`
                + establish HTTP connection 
                + reads to buffer of a given size 
            + `SeekableFileStream` [github](https://github.com/samtools/htsjdk/blob/master/src/main/java/htsjdk/samtools/seekablestream/SeekableFileStream.java)    
                + a `RandomAccessFile` member that handles reading

### htslib Notes 


`kstring.h`
+ _summary_ 
    ```c
    #define KSTRING_T kstring_t
    typedef struct __kstring_t {
        size_t l, m;
        char *s;
    } kstring_t;
    ```
    + `kstring_t` non-opaque (transparent) type 
        + data structure defined in interface, direct manipulation of data member
+ _related functions_ 
    + `int kputd(double d, kstring_t *s)`
+ _inline function_
    + `ks_str(kstring_t *s)`    
        + returns `s->s`
    + `ks_len(kstring_t *s)`
        + returns `s->l`
    + `int ks_resize(kstring_t *s, size_t size)`
        + if `m < size`, use `realloc` to allocate up to `size` for `kstring_t->s`
        + basically called every time trying to increment augment a string, a no-op if have enough memory
    + `char *ks_release(kstring_t *s)`
        + resets fields in `kstring_t`
        + return `kstring_t->s`, give up ownership... 
    + `int kputs(const char *p, kstring_t *s)`
        + call `kputsn`, use `strlen(p)` for second arg
    + `int kputsn(const char *p, int l, kstring_t *s)`
        + call `ks_resize` to make space 
        + update `s->s` 
            + copy string `p` with length `l` to `kstring_t->s + kstring_t->l`, i.e. first unfilled spot
        + update `s->l` and append null terminator
    + `kputc(int c, kstring_t *s)`
        + append `c` to `s`
    + `int kputw(int c, kstring_t *s)`
        + put word ?
    + `int kputuw(unsigned c, kstring_t *s)`
    + `int kputl(long c, kstring_t *s)`
    + `int *ksplit(kstring_t *s, int delimiter, int *n)`
        + return number of components `s` by deliminator 
+ _interface_ 
    + `int kputd(double d, kstring_t *s)`
        + appends a double `d` to `s`
    + `int kvsprintf(kstring_t *s, const char *fmt, va_list ap)`
    + `int ksprintf(kstring_t *s, const char *fmt, ...)`
    + `char *kstrtok(const char *str, const char *sep, ks_tokaux_t *aux)`
    + ...
    + `int kgetline(kstring_t *s, kgets_func *fgets, void *fp);`
        + where `typedef char *kgets_func(char *, int, void *);`
        + uses supplied `fgets` to read a `\n` or `\r\n` terminated line from `fp` (i.e. `hstFile->fp.hfile` for uncompressed SAM)
        + the line read is appended to end of `kstring` wihtout terminator, or EOF on EOF/error


`hts_defs.h`
+ a lot of `#ifndef` macros

`hts.h`
+ _macros_
    + `KSTRING_T` is a struct of type `__kstring_t`
    + `hts_expand` expand a dynamic array of a given type
    + `hts_expand0` expand and zero newly-allocated memory
        + uses `hts_realloc_or_die`
+ _file io_
    + `struct htsFormat`
        + `htsFormatCategory category`
            + sequence data, ratiant, index file, region list, ...
            + `category_maximum` what is 
        + `htsExactFormat format`
            + binary, text, sam, bam, json, bed, ..... 
        + `htsCompression compression`
            + no, gzip, bgzf, custom
        + version, ...
        + `void *specific`: points to the linked list of `hts_opt`
    + `struct htsFile`
       ```c
        typedef struct
        {
            // the bit is_... determines the type of union fp
            uint32_t is_bin : 1, is_write : 1, is_be : 1, is_cram : 1, is_bgzf : 1, dummy : 27; 
            int64_t lineno;
            kstring_t line;
            char *fn, *fn_aux;          // fn -> filename
            union {
                BGZF *bgzf;             // binary_format, bam, bcf, or compressed text_format, sam, vcf
                struct cram_fd *cram;   // cram 
                struct hFILE *hfile;    // uncompressed text_format, sam, vcf
            } fp;
            htsFormat format;           // format info,
        } htsFile;
       ```
    + `struct htsThreadPool`
        + `struct hts_tpool *pool`,  shared thread pool pre-defined...  
        + `int qsize`, size of queue for IO, if # of fd > threads, not good...
+ _format/options_ 
    + `enum sam_fields`, shift bits for accessing flags in header
    + `struct hts_opt`
        + `char* args`: holds `char * c_arg`
        + `enum hts_fmt_optiom/cram_option opt`: holds current option type... 
            + holds option for cram
        + `union {int i; char *s} val`: the value could be either int `i` or string `s`
            + use `std::variant` in cpp
        + `struct hts_opt *next`
            + linked list structure, ... 
+ _Manipulating `hts_opt` and `htsFormat`_ 
    + `int hts_opt_add(hts_opt **opts, const char *c_arg);`: 
        + `char *val` slice `c_args` to `key` and `value`
        + construct `hts_opt` from one `key=value` pair
            + a swtich over `key` see if they match, ... 
            + assign `hts_opt->opt` appropriate `enum` 
            + assign `hts_opt->val.i` a `int/long` type value, pointed to by `char *val`, 
        + append `hts_opt` to `hts_opt **opts`, by looping over... 
    + `int hts_opt_apply(htsFile *fp, hts_opt *opts);`
        + loop over `opts` linked list 
            + use `hts_set_opt` to apply `opt` to `htsFile`
    + `void hts_opt_free(hts_opt *opts)`
        + free `hts_opt` list
    + `int hts_parse_opt_list(htsFormat *opt, const char *str);`
        + tokenize `(key(=value)?,)*(key(=value)?)?`
        + for each `arg = "key=value"`, call `hts_opt_add(htsFormat->specific, arg)`
    + `int hts_parse_format(htsFormat *opt, const char *str);`
        + `scan_keyword`: helper function that splits `str` on `delim`, fill `buf` with `key` and return ptr to `value`
        + process string format `fmt` followed by a comma separated list of key=value options 
            + based on `fmt`, populate `htsFormat` struct 
            + call `hts_parse_opt_list` to populate `opt->specifics` given the rest of options
+ _Tables_ (could have some additional encoding here....)
    + `unsigned char seq_nt16_table[256]` 
        + for converting nucleotide char to 4-bit encoding 
        + `A/C/G/T` -> `1/2/3/4` 
    + `char seq_nt16_str[] = "=ACMGRSVTWYHKDBN"`
        + converts the 4-bit encoded nucleotide to UIPAC ambiguity code letter 
    + `char seq_nt16_int[]`
        + converts 4-bit encoded nucleotide to 2 bits
+ _Constructing `hstFile`_
    + `hts_detect_format` peek start of `struct hFile` and fill out `htsFormat`
    + `hts_format_description` human readable description of `htsFormat`
        + basically use `kput` on members of `htsFormat` struct
    + `hts_set_opt(htsFile *fp, enum hts_fmt_option opt, ...) `
        + sets one of 
            + generic options, i.e. 
                + `HTS_OPT_NTHREADS` with `hts_set_threads` 
                    + set thread number based on format/compressoin of `hstFile->format`
                + `HTS_OPT_BLOCK_SIZE` with `hfile_set_blksize`
                + `HTS_OPT_THREAD_POOL` with `hts_set_thread_pool`
                + `HTS_OPT_CACHE_SIZE` with `hts_set_cache_size`
                    + set thread number based on compressoin of `hstFile->format`
            + cram specific options 
                + with `cram_set_voption`
    + `hts_set_threads` create threads to aid compress/decompression for this file
    + `hts_set_thread_pool` same as using threads, but shared worker threads used, created by `hts_create_threads()`
    + `hts_set_cache_size` for decomressing blocks for bgzf
    + `hts_set_fai_filename` set `.fai` filename for a file open for reading 
    + `int hts_check_EOF(htsFile *fp);`
        + checks if `htsFile` has a valid EOF block 
        + 3 for non-EOF checkable filetype 
        + 2 for unseekable file 
        + 1 for a valid EOF block 
        + 0 if EOF marker is absent 
    + `int hts_process_opts(htsFile *fp, const char *opts)`
        + Construct a linked list of `opts` with `hts_parse_opt_list` 
            + `opts` is linked to `fp->htsFormat.specific`
        + apply the previously constructed `opts` with `hts_opt_apply`
            + populate `fstFile` using `hts_set_opt`
    + `hstFile *hts_open(const char *fn, const char *mode)` 
        + opens a SAM/BAM/CRAM/VCF/BCF/etc file by calling `hst_open_format(fn, mode, NULL)`
        + `fn` or `-` for stdin/stdout 
        + `mode` match `[rwa][bceguxz0-9]*`
            + `r` for reading, first BGZF file gives clue to file format, so will not go further 
            + `w/a`
                + `b`: binary (BAM, BCF) intead of text (SAM, VCF)
                + `c`: cram 
                + `g`: gzip compressed
                + `u`: uncompressed
                + `z`: bgzf compressed 
                + `0-9`: zlib compression level 
            + `e`, close file on exec(2)
            + `x`, create file exclusively 
    + `htsFile *hts_hopen(struct hFILE *fp, const char *fn, const char *mode);`
        + opens existing stream `hFILE *fp` as a BAM/SAM/... file 
        + heap allocate `htsFile` as return type, 
            + set appropriate fields (i.e. `htsFile->format`) based on `hFILE`
            + try incorporating `hFILE` into `htsFile->fp`, while setting appropriate `is_{}` flag
        + split `mode` into `[simple_mode],[opts]` strings
        + `simple_mode`
            + `r`
                + populate `hstFile->format` using `hts_detect_format` by seeking into `hFile` header
                + if format is `json`, call `hopen_json_redirect` and rebuild format with `hts_detect_format`
            + `w` or `a`
                + `is_write` is 1
                + based on `simple_mode`
                    + assign `hFILE->format.format` to one of `enum htsExactFormat`
                    + assign `hFILE->format.compression` to one of `enum htsCompression`
                    + assign `hFILE->format.category` with `format_category` given `hFILE->format.format`
                        + i.e. if `fmt` is `bam` then `category` is `sequence_data`
                + populate `hFILE->format.{version, compression_level, specifics}` 
        + `opt`, process with `hts_process_opts`
        + `hFILE->format.format` switches to populate `hstFile->fp`, oneof `{bgzf, cram, hFILE}`
            + `bgzf_hopen`
                + binary, bam, bcf 
                + text, sam, vcf with compression 
            + `cram_dopen`
                + cram 
            + `hFILE fp` in args 
    + `htsFile *hts_open_format(const char *fn, const char *mode, const htsFormat *fmt);`
        + creates `hstFile` and `hFILE`
        + determine format with 
            + `mode`'s format bit 
            + `htsFormat` 
        + create `hFILE` with `hopen(fn, smode)`
        + create `htsFile` with `hts_hopen(hfile, fn, smode);`
        + apply format with `hts_opt_apply`
    + `int hts_close(htsFile *fp);`
        + closes the file, flushes buffered data 
        + dispatch close for `hstFile->fp` by `hstFile->format`
            + `bgzf_close()` 
            + `cram_close()`
            + `hclose()` 
        + free `htsFile`
        + return return value of the close statement 
+ _Member function for `htsFile`_
    + `const htsFormat *hts_get_format(htsFile *fp);`
         return `hstFile->format` 
    + `const char *hts_format_file_extension(const htsFormat *format);`
        + return `"sam", ...` based on `hstFormat->format`
    + `hFILE *hts_hfile(htsFile *fp)`
        + based on `hstFile->format.format`
            + `bgzf_hfile()` for bam 
            + `cram_hfile()` for cram 
            + `fp->hfile` for text_format or sam 
    + `int hts_getline(htsFile *fp, int delimiter, kstring_t *str)`
        + get a line from `hstFile->fp` stream to a `kstring`
        + if no compression, 
            + use `kgetline(str, hgets, hstFile->fp.hfile)`to append line to `str` from `fp.hfile`
        + if use `gzip` or `bgzf`   
            + use `bgzf_getline(fp->fp.bgzf, '\n', str)` to append line to `str` 
    + `char **hts_readlines(const char *fn, int *_n);`
        + read line from a file given filename `fn` , or string `fn`, return number of line read `_n`
        + use `bgzf_open()`
    + `char **hts_readlist(const char *fn, int is_file, int *_n);`
+ _Indexing_ 
    + `HTS_IDX_*` macros 
        + 




`hts_internal` 
+ _Internal structures_ 
    + `int hfile_set_blksize(hFILE *fp, size_t capacity);`
        + resize buffer within `hFILE`
    + `struct hFILE *bgzf_hfile(struct BGZF *fp);`
        + return `hFILE` connected to `BGZF`
    + `struct hFILE_backend` holds 5 function pointers 
        + `read`, `wrte,` `seek`, `flush`, `close`










### Protocol Buffers 

[cpp tutorial](https://developers.google.com/protocol-buffers/docs/cpptutorial)
[language guide](https://developers.google.com/protocol-buffers/docs/proto)


+ _Installation_ 
    ```sh 
    ./configure 
    make 
    make check 
    sudo make install 
    ```
    + _location_ : `/usr/local`

+ _alternatives_ 
    + raw in-mem in binary form, fragile, not extensible...
    + ad-hoc data encoding , ... parsing at runtime takes time 
    + serilize to xml... however space intensive, and encoding/decoding performance penalty 
+ _protocol format definition_
    + define protocol buffer message types 
    ```cpp 
    package tutorial;               

    message Person {
        required string name = 1;
        reuqired int32 id = 2;
        optional string email = 3;

        enum PhoneType {
            MOBILE = 0;
            HOME = 1;
            WORK = 2;
        }

        message PhoneNumber {
            required string number = 1;
            optional PhoneType type = 2 [default = HOME];
        }
        repeated PhoneNumber phone = 4;
    }

    message AddressBook {
        repeated Person person = 1;
    }
    ```
    + `message`
        + aggregate containing a set of typed fields
            + `bool`, `int32`, `float`, `double`, `string`, or other messages, `enum`
            + `= 1`, ... marker identify the unique tag that field uses in the binary encoding
                + tag `1 - 15` requires one less byte to encode than higher numbers
                + tag `16` and higher for optional elements 
        + generated class under namespace matching the package name
    + field annotation 
        + `required`
            + a value for the field must be required,
            + causes parsing failure... 
        + `optional`
            + may or may not be set,
                + is not set: default value used 
                    + with `[default = foo]`
                    + with system defaults, i.e. `0` for numeric types, `""` for `string`, `false` for `bool`
                    + embedded messages use default prototype of the message
                        + calling accessor to get value of an optional field which has not been explicitly set always returns that field's default value 
        + `repeated`
            + any number of times (including 0)
            + order preserved (dynamic array)
+ _compilation_ 
    ```sh 
    protoc =I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/addressbook.proto
    ```
    + _Generates_  
        + `addressbook.pb.h` and `addressbook.pb.cc`
+ _Protobuf API_    
    ```cpp 
     // name
    inline bool has_name() const;
    inline void clear_name();
    inline const ::std::string& name() const;
    inline void set_name(const ::std::string& value);
    inline void set_name(const char* value);
    inline ::std::string* mutable_name();

    // id
    inline bool has_id() const;
    inline void clear_id();
    inline int32_t id() const;
    inline void set_id(int32_t value);

    // email
    inline bool has_email() const;
    inline void clear_email();
    inline const ::std::string& email() const;
    inline void set_email(const ::std::string& value);
    inline void set_email(const char* value);
    inline ::std::string* mutable_email();

    // phones
    inline int phones_size() const;
    inline void clear_phones();
    inline const ::google::protobuf::RepeatedPtrField< ::tutorial::Person_PhoneNumber >& phones() const;
    inline ::google::protobuf::RepeatedPtrField< ::tutorial::Person_PhoneNumber >* mutable_phones();
    inline const ::tutorial::Person_PhoneNumber& phones(int index) const;
    inline ::tutorial::Person_PhoneNumber* mutable_phones(int index);
    inline ::tutorial::Person_PhoneNumber* add_phones();
    ```
    ```cpp 
    Person person;
    person.set_name("John Doe");
    fstream output("myfile", ios::out | ios::binary);
    person.SearlizeToOstream(&output);
    //...
    fstream input("myfile", ios::in | ios::binary);
    Person person;
    person.ParseFromIstream(&input);
    cout << "Name: " << person.name() << endl;
    ```
    + _creates a class with_
        + accessor fields (getters and setters)
            + _getter_: lower case of field name 
            + _setter_: prefix with `set_`
            + _tester_: prefix with `has_`
            + _clear_: prefix with `clear_`
            + _mutable getter for string_: direct pointer returned
        + additional accessor for `repeated` 
            + check size with `_size`
            + get a specific item using its index 
            + update existing item with index 
            + add another phone number to message which can then edit with `add_`
        + _nested message_ 
            + called `Person_PhoneNumber`, 
            + but there is a `typedef` so as if were a nested class
        + _standard message methods_   
            + `bool IsInitialized() const`
                + if all required fields have been set 
            + `string debugSring() const` 
                + returns human readable representation of message 
            + `void copyFrom(const Person& from)`
                + overwrites message with given message's values 
            + `void Clear()`
                + clears all elements back to empty state 
        + _Parsing and serialization_ 
            + `bool SerializeToString(string* output) const;`
            + `bool ParseFromString(const string& data)`
            + `bool SerializeToOstream(ostream* output) const;`
            + `bool ParseIstream(istream* input)`
    + _good practice_ 
        + wrap generated protobuf in application-specific class 
        + so use wrapper class to craft an interface better suited to unique enviornment of application, hiding some data, methods 
        + should not add behavior by inheriting from the generated class....
+ _Use the protobuf classes_ 
    + reads `AddressBook` from a file, adds one new `Person` to it baesd on user input, and writes new `AddressBook` to file again 
    ```cpp 
    #include <iostream>
    #include <fstream>
    #include <string>
    #include "addressbook.pb.h"
    using namespace std;

    void PromptForAddress(tutorial::Person *person){
        cout << "Enter person ID number: ";
        int id;
        cin >> id;
        person->set_id(id);
        cin.ignore(256, '\n');

        cout << "Enter name: ";
        getline(cin, *person->mutable_name());

        cout << "Enter email address (blank for none): ";
        string email;
        getline(cin, email);
        if (!email.empty()) {
            person->set_email(email);
        }

        while (true) {

        cout << "Enter a phone number (or leave blank to finish): ";
        string number;
        getline(cin, number);
        if (number.empty()) {
            break;
        }

        tutorial::Person::PhoneNumber* phone_number = person->add_phones();
        phone_number->set_number(number);

        cout << "Is this a mobile, home, or work phone? ";
        string type;
        getline(cin, type);
        if (type == "mobile") {
            phone_number->set_type(tutorial::Person::MOBILE);
        } else if (type == "home") {
            phone_number->set_type(tutorial::Person::HOME);
        } else if (type == "work") {
            phone_number->set_type(tutorial::Person::WORK);
        } else {
            cout << "Unknown phone type.  Using default." << endl;
        }
    }

    void ListPeople(const tutorial::AddressBook& address_book){
        for(int i = 0; i < address_book.people_size(); i++){
            const tutorial::Person& person = address_book.people(i);

            cout << "Person ID: " << person.id() << endl;
            cout << "  Name: " << person.name() << endl;
            if (person.has_email()) {
                cout << "  E-mail address: " << person.email() << endl;
            }

            for (int j = 0; j < person.phones_size(); j++) {
                const tutorial::Person::PhoneNumber& phone_number = person.phones(j);

                switch (phone_number.type()) {
                    case tutorial::Person::MOBILE:
                        cout << "  Mobile phone #: ";
                        break;
                    case tutorial::Person::HOME:
                        cout << "  Home phone #: ";
                        break;
                    case tutorial::Person::WORK:
                        cout << "  Work phone #: ";
                        break;
                }
                cout << phone_number.number() << endl;
            }

        }
    }

    int main(int argc, char* argv[]) {


        // verify version of library linked against is compatible with versioon of headers we compiled against 
        GOOGLE_PROTOBUF_VERIFY_VERSION; 

         if (argc != 2) {
            cerr << "Usage:  " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
            return -1;
        }

        tutorial::AddressBook address_book;

        // Read the existing address book.
        fstream input(argv[1], ios::in | ios::binary);
        if (!input) {
            cout << argv[1] << ": File not found.  Creating a new file." << endl;
        } else if (!address_book.ParseFromIstream(&input)) {
            cerr << "Failed to parse address book." << endl;
            return -1;
        }

        // Add an address.
        PromptForAddress(address_book.add_people());
        ListPeople(address_book);

        {
            // Write the new address book back to disk.
            fstream output(argv[1], ios::out | ios::trunc | ios::binary);
            if (!address_book.SerializeToOstream(&output)) {
                cerr << "Failed to write address book." << endl;
                return -1;
            }
        }

        
        // Optional: Delete all global objects allocated by libprotobuf.
        google::protobuf::ShutdownProtobufLibrary();
    }
    ```
+ _Extending protobuf_ 
    + dont change tag numbers of existing fields 
    + dont add/delete required field 
    + may delete optional or repeated field 
    + may add new optional/repeated fields, but with a fresh tag number 
+ _Optimization_ 
    + Reuse message object when possible
    + multi-threaded allocation for small objects with `tcmalloc`




### GA4GH schemas 

[documentation](https://ga4gh-schemas.readthedocs.io/en/latest/)
[reference impl server with 1k genomes](http://1kgenomes.ga4gh.org/)
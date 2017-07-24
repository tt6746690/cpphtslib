




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
            + populate `htsFile` using `hts_set_opt`
    + `hstFile *hts_open(const char *fn, const char *mode)` 
        + opens a SAM/BAM/CRAM/VCF/BCF/etc file by calling `hts_open_format(fn, mode, NULL)`
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



`hfile_internal` 
+ `struct hFILE_backend` 
    ```c 
    struct hFILE_backend {
        /* As per read(2), returning the number of bytes read (possibly 0) or
        negative (and setting errno) on errors.  Front-end code will call this
        repeatedly if necessary to attempt to get the desired byte count.  */
        ssize_t (*read)(hFILE *fp, void *buffer, size_t nbytes) HTS_RESULT_USED;

        /* As per write(2), returning the number of bytes written or negative (and
        setting errno) on errors.  Front-end code will call this repeatedly if
        necessary until the desired block is written or an error occurs.  */
        ssize_t (*write)(hFILE *fp, const void *buffer, size_t nbytes)
            HTS_RESULT_USED;

        /* As per lseek(2), returning the resulting offset within the stream or
        negative (and setting errno) on errors.  */
        off_t (*seek)(hFILE *fp, off_t offset, int whence) HTS_RESULT_USED;

        /* Performs low-level flushing, if any, e.g., fsync(2); for writing streams
        only.  Returns 0 for success or negative (and sets errno) on errors. */
        int (*flush)(hFILE *fp) HTS_RESULT_USED;

        /* Closes the underlying stream (for output streams, the buffer will
        already have been flushed), returning 0 for success or negative (and
        setting errno) on errors, as per close(2).  */
        int (*close)(hFILE *fp) HTS_RESULT_USED;
    };
    ```
    + `int hfile_oflags(const char *mode);`
        + maybe called by `hopen_{}`
    + `hFILE *hfile_init(size_t struct_size, const char *mode, size_t capacity);`
        + called by `hopen_{}` to allocate memory for `struct hFILE`
            + for `mobile` buffer
        + allocate 
            + `struct_size` for `hFILE`
            + `capacity=32768` size to `hFILE->buffer` 
        + set `hFILE->{begin, end}` to `hFILE->buffer`
        + set `hFILE->limit` to `&fp->buffer[capacity]`
        + set 
            + `hFILE->offset` to 0
            + `hFILE->at_eof` to 0 
            + `hFILE->mobile` to 1
    + `hFILE *hfile_init_fixed(size_t struct_size, const char *mode, char *buffer, size_t buf_filled, size_t buf_size);`
        + alternative to `hfile_init`
            + for in memory backends for which base buffer is the only storage
            + so `mobile = 0`
            + `at_eof = 1`
        + `buffer` already allocated via `malloc(buf_size)`  
            + `buf_filled` bytes already filled
        + set `hFILE->{buffer, begin}` to `buffer`
        + set `hFILE->end` to `&buffer[buf_filled]`
        + set `hFILE-limit` to `&buffer[buf_size]`
    + `void hfile_destroy(hFILE *fp);`
        + may be called by `hopen_{}` to undo `hfile_init()`, in case subsequent operation to create stream fails 
        + basically frees `hFILE->buffer`   
    + `int hfile_set_blksize(hFILE *fp, size_t bufsize);`
        + resize buffer within `hFILE`
        + will not erase data if `bufsize` 
    + `struct hFILE *bgzf_hfile(struct BGZF *fp);`
        + return `hFILE` connected to `BGZF`
+ _private member_
    + `static inline int writebuffer_is_nonempty(hFILE *fp)` 
        + test if write buffer is empty... if `hFILE->begin > hFILE->end`
    + `static ssize_t refill_buffer(hFILE *fp)`
        + refills buffer from backend _once_
            + for `mobile` buffer only, since fixed size buffer require no `read`, everything already in address space (virtual memory)
        + return number of additional characters read (may be 0) or negative for error 
        + steps 
            + move unread character (when `hFILE->begin > hFILE->buffer` where `mobile=1`) to start of buffer, update pointers
                + with `memmove()`
            + read in available buffer space at `hFILE->[end, limit)`
                + call `hFILE->backend->read(hFILE, hFILE->end, hFILE->limit - hFILE->end)`
+ _file descriptor backend_
    ```c
    typedef struct
    {
        hFILE base;
        int fd;
        unsigned is_socket : 1;  // for winsock, use send()/recv() and closesocket() if fd is a socket
    } hFILE_fd;
    ```
    + `static ssize_t fd_read(hFILE *fpv, void *buffer, size_t nbytes)`
        + based on `hFILE_fd->is_socket`
            + `recv()` if true
            + `read()` otherwise
    + `static ssize_t fd_write(hFILE *fpv, const void *buffer, size_t nbytes)`
        + based on `hFILE_fd->is_socket`
            = `send()` if true 
            + `write()` otherwise 
    + `static off_t fd_seek(hFILE *fpv, off_t offset, int whence)`
        + use `lseek` on `hFILE_fd->fd`
    + `static int fd_flush(hFILE *fpv)`
        + use either `fdatasync` or `fsync` for flushing data on `hFILE_fd->fd`
    + `static int fd_close(hFILE *fpv)`
        + use `closesocket()` if `is_socket` otherwise `close()` on `hFILE_fd->fd`
    + _bundle backend_ 
        + `static const struct hFILE_backend fd_backend = {fd_read, fd_write, fd_seek, fd_flush, fd_close};`
    + `static hFILE *hopen_fd(const char *filename, const char *mode)`  
        + call `open(filename, hfile_oflags(mode), 0666);`
            + `hfile_oflags` decode `mode` to `fopen` style `mode`
        + initialize `hFILE_fd` with `hfile_init` (memory and initialize with given capacity)
            + assign `hFILE_fd->fd` with `fd` returned by `open`
            + `hFILE_fd->is_socket` is 0
            + assign the file descriptor backend `&fd_backend` to `hFILE_fd->base.backend` 
        + return `hFILE`, i.e. `&hFILE_fd->base`
    + `hFILE *hdopen(int fd, const char *mode)`
        + associate a stream with an existing open file descriptor
            + binary only 
            + used when reading from stdin/stdout
        + basically initialize `hFILE_fd` with `hfile_init` 
        + assign `fd`, `socket`, and `backend` to `hFILE_ref`
        + return `&hFILE_fd->base`
    + `static hFILE *hopen_fd_fileuri(const char *url, const char *mode)`
        + given `url`, starting with either 
            + `file://localhost/`
            + `file:///`
        + remove the prefix and return `hopen_fd` (url includes path only, not prefixes)
    + `static hFILE *hopen_fd_stdinout(const char *mode)`
        + determine `stdin`/`stdout` based on if `mode` is `r`/`w`
        + so `fd` is either `STDIN_FILENO` or `STDOUT_FILENO`
            + set `fd` to be binary with `O_BINARY`
        + return `hdopen(fd, open)`
+ _In-memory backend_
    ```c 
    typedef struct
    {
        hFILE base;
    } hFILE_mem;
    ```
    + `static off_t mem_seek(hFILE *fpv, off_t offset, int whence)`
        + return -1
    + `static int mem_close(hFILE *fpv)`
        + return 0, always succeeds
    + _bundle backend_ 
        + ` static const struct hFILE_backend mem_backend = {NULL, NULL, mem_seek, NULL, mem_close};`
    + `static hFILE *hopen_mem(const char *url, const char *mode)`
        + a lot of encoding/decoding stuff...
+ _hash map `scheme_string` -> `hFILE_scheme_handler`_
    ```c
    KHASH_MAP_INIT_STR(scheme_string, const struct hFILE_scheme_handler *)
    static khash_t(scheme_string) *schemes = NULL;
    ```
    ```c
    struct hFILE_scheme_handler {
        /* Opens a stream when dispatched by hopen(); should call hfile_init()
        to malloc a struct "derived" from hFILE and initialise it appropriately,
        including setting base.backend to its own backend vector.  */
        hFILE *(*open)(const char *filename, const char *mode) HTS_RESULT_USED;

        /* Returns whether the URL denotes remote storage when dispatched by
        hisremote().  For simple cases, use one of hfile_always_*() below.  */
        int (*isremote)(const char *filename) HTS_RESULT_USED;

        /* The name of the plugin or other code providing this handler.  */
        const char *provider;

        /* If multiple handlers are registered for the same scheme, the one with
        the highest priority is used; range is 0 (lowest) to 100 (highest).
        This field is used modulo 1000 as a priority; thousands indicate
        later revisions to this structure, as noted below.  */
        int priority;

        /* Fields below are present when priority >= 2000.  */

        /* Same as the open() method, used when extra arguments have been given
        to hopen().  */
        hFILE *(*vopen)(const char *filename, const char *mode, va_list args)
            HTS_RESULT_USED;
    };
    ```
+ _plugin list and `hFILE_plugin`_ 
    ```c 
    struct hFILE_plugin_list
    {
        struct hFILE_plugin plugin;
        struct hFILE_plugin_list *next;
    };
    static struct hFILE_plugin_list *plugins = NULL;
    static pthread_mutex_t plugins_lock = PTHREAD_MUTEX_INITIALIZER;        // mutex to plugins
    ```
    ```c
    struct hFILE_plugin {
        /* On entry, HTSlib's plugin API version (currently 1).  */
        int api_version;

        /* On entry, the plugin's handle as returned by dlopen() etc.  */
        void *obj;

        /* The plugin should fill this in with its (human-readable) name.  */
        const char *name;

        /* The plugin may wish to fill in a function to be called on closing.  */
        void (*destroy)(void);
    };
    ```
+ _Plugin and hopen() backend dispatcher_ 
    + `static void hfile_exit()`
        + call `kh_destroy()` to remove the hash pair `scheme_string, schemas`
        + loop over `hFILE_plugin_list` and call `hFILE_plugin_list->plugin.destroy()`
        + mutex protected access to list 
    + `static inline int priority(const struct hFILE_scheme_handler *handler)`
        + returns `hFILE_scheme_handler->priority % 1000`
    + `void hfile_add_scheme_handler(const char *scheme, const struct hFILE_scheme_handler *handler)`
        + called by plugins for each URL scheme they wish to handle
        + put `scheme -> handler` to hash map `schemes`
    + `static int init_add_plugin(void *obj, int (*init)(struct hFILE_plugin *), const char *pluginname)`
        + allocate a `hFILE_plugin_list`, set 
            + `obj` this is of type `hFILE_plugin`
            + `name`, ...
        + link `hFILE_plugin_list` to the list `plugins`
    + `static void load_hfile_plugins()`
        + create 2 default `hFILE_scheme_handler` 
            + `data = {hopen_mem, hfile_always_local, "built-in", 80}`, 
            + `file = {hopen_fd_fileuri, hfile_always_local, "built-in", 80}`
        + initializes hashmap `schemes` 
            + and use `hfile_add_scheme_handler` to add `data` and `file` handler to `schemes`
        + if `ENABLE_PLUGINS` macro on
            + initialize `hts_path_itr` with `hts_path_itr_setup`
            + loop over with `hts_path_itr_next()` and 
                + call `load_plugin(&obj, pluginname, "hfile_plugin_init");` to initialize `hFILE_plugin` 
                    + note the function `hfile_plugin_init` is dynamically loaded
                + if correctly loaded, call `init_add_plugin(dlopen_ret_handle, some_hfile_plugin_init, pluginname)`
        + if `ENABLE_PLUGINS` off, then we can selectively enable built-in plugins 
            + `HAVE_LIBCURL`
                + `init_add_plugin(NULL, hfile_plugin_init_libcurl, "libcurl");`
            + `ENABLE_GCS`
                + `init_add_plugin(NULL, hfile_plugin_init_gcs, "gcs");`
            + `ENABLE_S3`
                + `init_add_plugin(NULL, hfile_plugin_init_s3, "s3");`
    + `static hFILE *hopen_unknown_scheme(const char *fname, const char *mode)`
        + open file like `foo:bar`, indicate
            + is a regular file 
            + or missing plugins 
        + Try open as ordinary file with `hopen_fd()` and return `hFILE`
        + otherwise set `errno` for invalid plugin 
    + `static const struct hFILE_scheme_handler *find_scheme_handler(const char *s)`
        + returns appropriate handler given url, or NULL if string is not URL
        + initialize `unknown_scheme` 
            + `unknown_scheme = {hopen_unknown_scheme, hfile_always_local, "built-in", 0};`
        + call `load_hfile_plugins()`
        + queries `scheme` against `s` to find corresponding `scheme_handler`
            + if found return that `scheme_handler` otherwise the `unknown_scheme`
    + `hFILE *hopen(const char *fname, const char *mode, ...)`
        + Open the named file or URL as a stream `hFILE`
        + call `find_scheme_handler` 
            + if found, return `handler->open()` (which is one of `hopen_{}` functions)
            + if not found, try `handler->vopen()`
        + if `fname` is `-`, then return `hopen_fd_stdinout()` (which yields a `hFILE` from stdinout)
        + otherwise use `hopen_fd()` to open file descriptor


`hts_internal.h`
+ `hts_path_itr`
    ```c
    struct hts_path_itr {
        kstring_t path, entry;
        void *dirv;                 // DIR * privately, open directory open_nextdir(itr);
        const char *pathdir, *prefix, *suffix;
        size_t prefix_len, suffix_len, entry_dir_l;
    };
    ```
+ `static DIR *open_nextdir(struct hts_path_itr *itr)`
+ `void hts_path_itr_setup(itr, path, builtin_path, prefix, prefix_len, suffix, suffix_len)`
    + populate members of `struct hts_path_iter`
    + `path` either passed or taken from `getenv("HTS_PATH")`
    + `builtin_path` taken from `PLUGINPATH`
    + loop over `path` `:` separated path to plugins
        + add to `itr->path`
    + then assign `itr->dirv` with `open_nextdir`
+ `const char *hts_path_itr_next(struct hts_path_itr *itr);`
    + returns next plugin in `hts_path_itr`
+ `void *load_plugin(void **pluginp, const char *filename, const char *symbol);`
    + use `dlopen(const char* filename, int flags)` to link a plugin
        + loads the dynamic shared objet file named by `filename`
        + return an opaque handle for the loaded object
    + use `void *dlsym(void *restrict handle, const char *restrict name);` 
        + obtain address of a symbol from a `dlopen` object, 
        + `handle` is return value of `dlopen`
        + `name` is symbol's name as char string, here `name = hfile_plugin_init`
    returns the `symbol` linked and loaded
+ `void *plugin_sym(void *plugin, const char *name, const char **errmsg);`
+ `void close_plugin(void *plugin);`


`hfile.h`
+ `fFILE`
    + a low-level I/O stream handle, an opaque incomplete type
    ```c 
    typedef struct hFILE {
        // @cond internal
        char *buffer, *begin, *end, *limit;
        const struct hFILE_backend *backend;
        off_t offset;
        unsigned at_eof:1, mobile:1, readonly:1;
        int has_errno;
        // @endcond
    } hFILE;
    ```
    + `buffer` pointer to start of IO buffer 
    + `begin` pointer to first not-yet-read (`r`) / unused char (`w`), indicate file `position` ptrs
    + `end` pointer to first unfilled position (`r`) / start of buffer for (`w`) (not used)
    + `limit` pointer to first position past buffer
    + `backend`, contains methods to fill/flush buffer
    + `offset` within stream of buffer position 0
    + `at_eof` for `r`, set if seen EOF 
    + `mobile` buffer is a 
        + _mobile window_ 
            + `buffer` always at start of buffer, `limit` at the end 
        + _fixed full contents_ 
            + for in memory streams, 
            + entire content in memory
            + so `at_eof = 1` , `offset = 0`
            + dont need `read` methods and return EINVAL for `seek()` 
    + `has_errno` error number from last failure on this stream
+ _`hFILE`'s functions_ 
    + `hFILE *hopen(const char *filename, const char *mode, ...) HTS_RESULT_USED;`
        + open named file or url as a stream
    + `hFILE *hdopen(int fd, const char *mode) HTS_RESULT_USED;`
        + associate a stream with an existing open fd
        + must be opend with `b` for binary
    + `static inline int hgetc(hFILE *fp)`
        + read one character from stream, return `char` read or `EOF` or error
        + if `hFILE->end > hFILE->begin`
            + then some char unread, return `*(hFILE->begin++)`
        + otherwise 
            + call and return `hgetc2(hFILE)`
                + call `refill_buffer`
                + if some chars read, then return `*(hFILE->begin++)`
                + otherwise, return EOF
        + _comments_
            + impl very inefficient, since reading one `char` at begining, require shifting all subsequent `char` by 1 with `memmove`. If called consecutively, very slow
                + actually not, `refill_buffer` called only if `hFILE->begin >= hFILE->end`, which happens only rarely,
                + but still could be improved, since right now require a wrapper over `hgetc2`, same problem with `hread2`
            + should use _ring buffer_ instead to avoid shifting all element when one element is consumed
                + good impl for a queue
                + maybe optimized by mapping underlying buffer to 2 contiguous region of virtual memory
    + `ssize_t hgetdelim(char *buffer, size_t size, int delim, hFILE *fp)`
        + read from stream until the delimiter, up to a maximum length
            + return number of bytes read, otherwise negative on error
        + `buffer` into which bytes will be written to, with size of `size`
        + `fp`, the file stream
        + steps 
            + check `size` is valid `ssize_t`,  and the buffer is in read mode (not write mode)
            + look in `hFILE` for `delim` with `memchr`, 
                + if found, then `memcpy` string up to `delim` to `buffer`, and return
                + otherwise, copy as much as possible, and refill `hFILE->buffer` with `refill_buffer`
                    + if `buffer` is full, return anything copied to `buffer` even if `delim` is not found
    + `hgetln(char *buffer, size_t size, hFILE *fp)`
        + read a line from stream, up to a maximum length
        + return `hgetdelim(buffer, size, '\n', hFILE)`
    + `char *hgets(char *buffer, int size, hFILE *fp)`
        + read a line from stream, up to a maximum length 
        + return `hgetln` if succeeds, otherwise return `NULL`
    + `ssize_t hpeek(hFILE *fp, void *buffer, size_t nbytes) HTS_RESULT_USED;`
        + peek at char to be read without removing from buffers 
            + return number of bytes peeked, (less than nbytes if EOF encountered) and negative on error
            + `hFILE->buffer` not erased, may be expanded with `refill_buffer`
        + `buffer` holds peeked bytes, 
        + `nbytes` number of bytes to peek at, limited by size of buffer, <= 4k
        + basically call `refill_buffer` repeatedly if `nbytes` is larger than what is unread in buffer, 
        + then use `mmecpy` to copy maximum allowed size 
    + `hread(hFILE *fp, void *buffer, size_t nbytes)`
        + read a block of char from file 
            + return number of bytes read, or negative on errors
        + `memcpy` `hFILE->end - hFILE->begin` size to `buffer` 
        + if `nbytes` required is more than the amount of chars unread in buffer, call `hread2`
            + precondition: `hFILE->buffer` is empty and `nread` bytes already placed in destination buffer
            + use `hFILE->backend->read` into destination `buffer` directly (without storing in an intermediate `hFILE->buffer`)
                + update `at_eof = 1` if reached EOF 
                + update `buffer_invalidated` if `read` call is successful. `hFILE->buffer` is invalidated in this case 
            + if `buffer_invalidated`
                + update `hFILE->offset`
                + update `hFILE->begin` and `hFILE->end` to `hFILE->buffer`
            + use `refill_buffer` again, `memcpy` from bufer to destination buffer
    + `static ssize_t flush_buffer(hFILE *fp)`
        + flush write buffer `hFILE->[buffer, begin)` out with `hFILE->backend->write` repeatedly until `hFILE->begin` points to start of buffer, i.e. same as `hFILE->buffer`
    + `int hflush(hFILE *fp)`
        + call `flush_buffer`, use `hFILE->backend->flush` to flush the buffer
    + `static inline int hputc(int c, hFILE *fp)`
        + write a `char` to `hFILE->buffer`
        + if buffer not full, write with `*(hFILE->begin++) = c`
        + otherwise, call `hputc2`
            + call `flush_buffer` 
            + then call `*(hFILE->begin++) = c`
    + `hwrite(hFILE *fp, const void *buffer, size_t nbytes)`
        + write a block of chars to file 
            + return `nbytes` written or negative on error 
        + use `memcpy` to write to `hFILE->begin` given `buffer`
        + if `hFILE->buffer` is full before `buffer` is completely copied, call `hwrite2`
            + `ssize_t hwrite2(hFILE *fp, const void *srcv, size_t totalbytes, size_t ncopied)`
            + _precondition_: buffer is full and ncopied bytes from `srcv` already copied to buffer
            + call `flush_buffer`
            + write large blocks directly from source buffer with `hFILE->backend->write` 
                + skipping the buffered step of `src -> hFILE->buffer -> write`
            + then buffer remaining chars 
    + `static inline int hputs(const char *text, hFILE *fp)`
        + write a `string` to `hFILE->buffer`
        + use `memcpy` to write to `hFILE->begin` given `text`
        + if `hFILE->buffer` is full before `text` is completely copied, call `hputs2`
            + call `hwrite2()`
    + `int hflush(hFILE *fp) HTS_RESULT_USED;`
    + `static inline off_t htell(hFILE *fp)`
        + return current stream offset 
        + `hFILE->offset + (hFILE->begin - hFILE->buffer)`
    + `off_t hseek(hFILE *fp, off_t offset, int whence) HTS_RESULT_USED;`
        + reposition read/write stream offset 
        + return resulting offset within stream, or negative if error occurred 
        + steps 
            + `flush_buffer` if write buffer is nonempty
            + if `whence` is `SEEK_CUR`, update `offset` as if `whence` is `SEEK_SET`
            + if `whence` is `SEEK_END` and `mobile=1`, update `offset` as if `whence` is `SEEK_SET`
            + use `hFILE->backend->seek` to return the seeked position
            + if seek succeeded, clear the buffer (update pointers)
    + `int hclose(hFILE *fp)`
        + flush output stream with `hflush`
        + call `hFILE->backend->close()`
        + call `hfile_destroy()` 
    + `void hclose_abruptly(hFILE *fp);`
        + close stream without flushing...
    + `static inline int herrno(hFILE *fp)`
        + return `hFILE->has_errno` the stream's error indicator
    + `static inline void hclearerr(hFILE *fp)`
        + clears `hFILE->has_errno`


`











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
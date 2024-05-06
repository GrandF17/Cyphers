Here is represented implementation of Russian hash function + Key Distribution Protocol implementation based on NMAC / HMAC functions.

Streebog can be found by path: `./libs/streebog.cpp` (functions) and `./libs/streebog.h` (class);

NMAC / HMAC and tiny description can be found by path: `./utils/mac.cpp`

Parameters you NEED to pass to make it work:

`--k="{KEY_FILE}.key"` - path to bit sequence more than 256 bits in row

`--o="{OFFSET}"` - offset in key file (if bits more than 256 while key is 256 bit / 512 while key is 512 bit)

`--kl="{KEY_LEN}"` 32 / 64 bytes ONLY

`--l="{LOG_FILE}"`

`--mode="{MODE}"` 0 (make NMAC vector), 1 (run tests)

`--checksum="{RECOUNT_CHECKSUM}"` 0/1 (bool) eq count or not

# LINUX USAGE

How to build (call from root):

```
g++ main.cpp libs/md5.cpp libs/streebog.cpp -o main -pthread
```

How to run:

```
./main --k="1048576.key" --o="0" --kl="64" --l="log.txt" --mode="0" --checksum="0"
```
### P.S. Can be run only on Unix

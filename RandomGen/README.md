## Random Generator based on Magma Light Cryoptography implementation

Parameters you NEED to pass to make it work:

`--k="{KEY_FILE}.key"` - path to bit sequence more than 256 bits in row

`--o="{OFFSET}"` - offset in key file (if bits more than 256 bits)

`--l="{SEQ_LEN}"` any sequance length you want

## LINUX / WINDOWS

How to build (call from root):

```
g++ main.cpp libs/random.cpp libs/magma.cpp -o main
```

How to run:

```
./main --k="1048576.key" --o="0" --l=1048576
```

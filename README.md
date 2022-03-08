# INF01151
INF01151 – SISTEMAS OPERACIONAIS II N


## Compiling source code

###1º Create the build directory
```bash 
mkdir build
```
###2º execute cmake
```bash 
cmake -S . -B build
```
###3º Compile the generated Makefile
```bash 
make -C build
```

## Running the server
```bash 
./build/INF01151 -s
```
## Running the client
```bash 
./build/INF01151 -c -u a_user_name
```

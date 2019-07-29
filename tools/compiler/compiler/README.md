
## Building XACC with single-source compiler
You will need to install the latest clang and llvm libraries and headers (Version 9). On Ubuntu, this can be done with the following commands
```bash
$ wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
$ echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main" > /etc/apt/sources.list.d/llvm.list
$ apt-get update
$ apt-get install -y libclang-9-dev llvm-9-dev
$ ln -s /usr/bin/llvm-config-9 /usr/bin/llvm-config
```

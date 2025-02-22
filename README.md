# SHA-256 Utility

This is a small command-line utility built in C++ for calculating the SHA-256 hash of a plaintext message or file.

## Installation

Please ensure that there is a C++ compiler installed. [GCC/G++](https://gcc.gnu.org/) is recommended. Alternatively, [MSYS2 can be used](https://code.visualstudio.com/docs/cpp/config-mingw) to install the MinGW toolchain, which includes GCC.

Clone the repository:

```
git clone https://github.com/thomasha1310/sha-256-util.git
```

Navigate to the new directory:

```
cd sha-256-util
```

Compile the project:

```
g++ -o sha256 sha256.cpp
```

The compiled EXE will be located at `sha-256-util\sha256.exe`.

## Usage

The executable must be run through the command-line.

To obtain the hash of a message:

```
.\sha256.exe <message>
.\sha256.exe -m <message>
```

To obtain the hash of a file:

```
.\sha256.exe -f <filepath>
```

For example:

```
> .\sha256.exe "Hello world!"
c0535e4be2b79ffd93291305436bf889314e4a3faec05ecffcbb7df31ad9e51a
> .\sha256.exe -m "Hello world!"
c0535e4be2b79ffd93291305436bf889314e4a3faec05ecffcbb7df31ad9e51a
> .\sha256.exe -f .\sha256.cpp
b6dc66a81508e5503a89f59a5641beb3abbea617a090ae5b19a56e5b07a93bd0
```

## License

This project is licensed under the [MIT License](LICENSE).

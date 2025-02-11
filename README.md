# WebServer

A lightweight C-based static web server for windows.

## Requirements

* MinGW or MSVC
* CMake
* Winsock2

## Installation

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

## Usage

Run the server:
```bash
server.exe
```

Add your website to www folder in project root.

Then, open a browser and navigate to:
```bash
http://localhost:8080
```

## Configuration

Update config.ini file in project root. 

## To-Do / Future Enhancements

* Parallel execution
* Support dynamic content (CGI or embedded scripting)
* Add HTTPS support

## License

[MIT](https://choosealicense.com/licenses/mit/)
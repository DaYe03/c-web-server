# CWEBSERVER

## Description
CWEBSERVER is a simple and straightforward C library that provides API and implementation for an HTTP 1.1 web server (HTTP 1.0 also supported).

## Contents
- `src/`: Contains the source files of the library.
- `demo/`: Contains an example of using the library.

## Build
To build the project, follow these steps:

1. Ensure that you have CMake installed on your system.
2. Create a `build` directory in the project root.
3. Inside the `build` directory, run the following command:
   ```
   cmake ..
   ```
4. After configuration, run the command:
   ```
   make
   ```
5. To run the demo example, execute the command:
   ```
   ./demo
   ```

**Note**: If you change the location or name of the main file, be sure to update the corresponding line in the CMakeLists.txt under `# Add the executable for demo`.

## Library API

### Overview

The CWEBSERVER library provides a set of functions and utilities for building and managing HTTP 1.1 web servers in the C programming language. Below is an overview of the key functionalities offered by the library:

### Core Functionality

1. **Starting and Stopping the Server**: Functions for initializing and shutting down the web server.

2. **Adding Routes**: Functions for defining routes that map specific HTTP methods and URL paths to corresponding callback functions.

3. **Handling Requests and Generating Responses**: Provides flexible methods for accessing request parameters and creating appropriate HTTP responses.

### Details

#### Start and Stop the Server

- `server_t *start_daemon(int port, int max_connections, const char *ip)`: Initializes and starts the web server on the specified port and IP address.

- `void stop_daemon(server_t * server)`: Gracefully shuts down the web server.

#### Adding Routes

- `int add_route(char *path, char *method, callback cb)`: Adds a new route to the web server for handling requests with the specified HTTP method and URL path pattern. The callback function `cb` is invoked to handle requests to this route.

### Handling Requests and Generating Responses

The CWEBSERVER library provides structures and functions to handle incoming HTTP requests and generate appropriate responses.

#### Request Structure

The `request_t` structure represents an HTTP request received by the server. It contains information such as the request method, URL path, HTTP version, headers, and body content.

```c
typedef struct 
{
    char *method;       // HTTP request method (e.g., GET, POST)
    char *path;         // URL path requested by the client
    char *version;      // HTTP protocol version (e.g., HTTP/1.1)
    node_t *headers;    // Linked list of request headers
    struct
    {
        char *data;     // Body data of the request (e.g., JSON payload)
        node_t *params; // Linked list of request parameters (e.g., form data)
        size_t n_params; // Number of parameters in the request
    } body;
} request_t;
```

If the request method is `POST`, the parameters submitted by the client will be found in the `params` field, and the request headers will be stored in the `headers` field. Both the headers and parameters can be accessed and navigated using the `get_header` function, which retrieves the value of a specific header given its key. This provides developers with the ability to parse and utilize request data effectively within their server applications.

#### Response Structure

The `response_t` structure represents an HTTP response to be generated by the server. It includes fields for the HTTP version, status code, headers, and body content.

```c
typedef struct
{
    char *version;      // HTTP protocol version (e.g., HTTP/1.1)
    char *status_code;  // HTTP status code (e.g., 200 OK)
    node_t *headers;    // Linked list of response headers
    char *body;         // Body content of the response
} response_t;
```

Directly modifying the parameters of this struct can lead to issues, so the use of functions like `add_header`, `add_body_res`, `add_file_body`, `add_version_res`, and `add_status_code_res` is required. These functions enable developers to add and manipulate headers, body content, and other parameters of the response structure safely and efficiently. By default, the version is set to HTTP/1.1, and the status code is set to 200.


#### Get Headers and Parameters

- `char *get_header(node_t *headers, char *key)`: Returns the value of the header or param associated with the specified key.

    This function searches for a header in a linked list of headers and returns its value if found. It takes two parameters:
    - `headers`: A pointer to the linked list of headers.
    - `key`: The key (name) of the header to search for.

    If the header with the specified key is found in the list of headers, the function returns its value. Otherwise, it returns `NULL` to indicate that the header was not found.

 
#### Add Header

- `void add_header(node_t **headers, char *key, char *value)`: Adds a new header to the linked list of headers.

    This function adds a new header with the specified key-value pair to the linked list of headers. It takes three parameters:
    - `headers`: A pointer to the pointer to the linked list of headers. This is a double pointer to allow modification of the original list.
    - `key`: The key (name) of the header to add.
    - `value`: The value of the header to add.

    The function dynamically allocates memory for the new header node and inserts it into the linked list. Developers can use this function to add custom headers to HTTP requests or responses.

    Example Usage:
    ```c
    node_t *headers = NULL;
    add_header(&headers, "Content-Type", "text/plain");
    ```


#### Add Body to Response

- `void add_body_res(response_t *res, char *body)`: Adds a body content to the response structure.

    This function sets the body content of the response structure to the specified string. It takes two parameters:
    - `res`: A pointer to the response structure to which the body content will be added.
    - `body`: The body content to be added to the response.

    This function dynamically allocates memory for the body content of the response and assigns the provided string to it. Developers can use this function to add body content to HTTP responses.

    Example Usage:
    ```c
    response_t *response = init_response();
    add_body_res(response, "<html><body><h1>Hello, World!</h1></body></html>");
    ```


#### Set Public Path for File Body

Before adding a file to the body of the response, it is necessary to define the path for the generic `/public` folder, and the files will be contained in `/static`.

- `void set_public_path(char *path)`: Sets the path for the generic `/public` folder where files will be located.

- `path`: The path to the directory where static files are stored.


#### Add File Body to Response

- `void add_file_body(response_t *res, char *path)`: Adds the content of a file to the body of the response structure.

    This function reads the content of the specified file and sets it as the body content of the response structure. It takes two parameters:
    - `res`: A pointer to the response structure to which the file content will be added.
    - `path`: The path to the file whose content will be added to the response.

    This function dynamically allocates memory for the body content of the response and reads the content of the file specified by the path. Developers can use this function to add the content of HTML files, text files, or other resources to HTTP responses.

    Example Usage:
    ```c
    response_t *response = init_response();
    add_file_body(response, "index.html");
    ```

#### Add Version to Response

- `void add_version_res(response_t *res, char *version)`: Adds the HTTP protocol version to the response structure.

    This function sets the HTTP protocol version of the response structure to the specified string. It takes two parameters:
    - `res`: A pointer to the response structure to which the version will be added.
    - `version`: The HTTP protocol version to be added to the response.

    Example Usage:
    ```c
    response_t *response = init_response();
    add_version_res(response, "HTTP/1.1");
    ```


#### Add Status Code to Response

- `void add_status_code_res(response_t *res, char *status_code)`: Adds the HTTP status code to the response structure.

    This function sets the HTTP status code of the response structure to the specified string. It takes two parameters:
    - `res`: A pointer to the response structure to which the status code will be added.
    - `status_code`: The HTTP status code to be added to the response.

    Example Usage:
    ```c
    response_t *response = init_response();
    add_status_code_res(response, "404");
    ```
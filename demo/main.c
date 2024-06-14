#include <server.h>
#include <route.h>
#include <http_data.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

void hello(request_t *req, response_t *res)
{
    // printf("%s\n", get_header(req->headers, "User-Agent"));
    // printf("%s\n", get_header(req->body.params, "UserName"));
    // printf("%s\n", req->body.data);


    add_status_code_res(res, "200");
    add_header(&(res->headers), "Content-Type", "text/html");
    // add_body_res(res, "<h1>Hello World</h1>");
    add_file_body(res, "index.html");
}

int main(int argc, char const *argv[])
{
    set_public_path("../public");
    add_route("GET", "/", &hello);
    server_t *server = start_daemon(8080, 10, NULL);
    // loop waiting for char q
    char c;
    while (1)
    {
        c = getchar();
        if (c == 'q')
        {
            stop_daemon(server);
            break;
        }
    }
    return 0;
}
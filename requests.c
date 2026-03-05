#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"


char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0) {
        for (int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);  // print each full header line
    }
}
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char* compute_post_request(char *host, char *url, char *content_type, char *body_data, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    //char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies_count > 0) {
        for (int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);  // print each full header line
    }
}
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    compute_message(message, body_data);

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0) {
        for (int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);  // print each full header line
    }
}
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}
char* compute_put_request(char *host, char *url, char *content_type, char *body_data, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    //char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "PUT %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies_count > 0) {
        for (int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);  // print each full header line
    }
}
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    compute_message(message, body_data);

    free(line);
    return message;
}
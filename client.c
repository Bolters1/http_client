#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    // char *message;
    char *response;
    // int sockfd;
    char *cookie[1];
    cookie[0] = NULL;
    char *token[1];
    token[0] = NULL;
    char *user_cookie[1];
    user_cookie[0] = NULL;

    // Ex 1.1: GET dummy from main server
    // Ex 1.2: POST dummy and print response from main server
    // Ex 2: Login into main server
    // Ex 3: GET weather key from main server
    /// - Bonusuri
    // Ex 4: GET weather data from OpenWeather API
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    // sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);

    char command[100];
    char admin_name[100];
    char username[100];
    int login_status = 0;
    while (1)
    {
        scanf("%s", command);
        if (strcmp(command, "exit") == 0)
            return 0;
        else if (strcmp(command, "login_admin") == 0)
        {
            if (cookie[0] != NULL)
            {
                printf("already logged in!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char pass[100];
                printf("username= ");
                fflush(stdout);
                scanf("%s", admin_name);
                printf("password= ");
                fflush(stdout);
                scanf("%s", pass);
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "username", admin_name);
                json_object_set_string(root_object, "password", pass);
                char *json_string = json_serialize_to_string(root_value);
                char *request = compute_post_request(
                    "63.32.125.183",
                    "/api/v1/tema/admin/login",
                    "application/json",
                    json_string,
                    NULL, 0);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *cookie_start = strstr(response, "Set-Cookie:");
                if (cookie_start != NULL)
                {
                    cookie_start += strlen("Set-Cookie: ");
                    char *cookie_end = strchr(cookie_start, ';');
                    int cookie_len = cookie_end ? (cookie_end - cookie_start) : strlen(cookie_start);
                    cookie[0] = malloc(cookie_len + 1 + 8);
                    strcpy(cookie[0], "Cookie: ");
                    strncpy(cookie[0] + 8, cookie_start, cookie_len);
                    cookie[0][cookie_len + 8] = '\0';
                }
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: logged in!\n");
                    }

                    json_value_free(root);
                }
                free(json_string);
                free(request);
                free(response);
                json_value_free(root_value);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "add_user") == 0)
        {
            if (cookie[0] == NULL)
            {
                printf("no admin role!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char name[100], pass[100];
                printf("username= ");
                fflush(stdout);
                scanf("%s", name);
                printf("password= ");
                fflush(stdout);
                scanf("%s", pass);
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "username", name);
                json_object_set_string(root_object, "password", pass);
                char *json_string = json_serialize_to_string(root_value);
                char *request = compute_post_request(
                    "63.32.125.183",
                    "/api/v1/tema/admin/users",
                    "application/json",
                    json_string,
                    cookie, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: User added\n");
                    }

                    json_value_free(root);
                }
                // printf("%s",response);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "get_users") == 0)
        {
            if (cookie[0] == NULL)
            {
                printf("no admin role!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char *request = compute_get_request(
                    "63.32.125.183",
                    "/api/v1/tema/admin/users",
                    NULL,
                    cookie, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                }
                JSON_Value *root = json_parse_string(json_start);
                JSON_Object *root_obj = json_value_get_object(root);
                JSON_Array *users = json_object_get_array(root_obj, "users");
                int count = json_array_get_count(users);
                printf("SUCCES: Lista utilizatorilor\n");
                for (int i = 0; i < count; i++)
                {
                    JSON_Object *user = json_array_get_object(users, i);
                    // printf("ID= %.0f\n", json_object_get_number(user, "id"));
                    printf("#%d ", i);
                    printf("%s:", json_object_get_string(user, "username"));
                    printf("%s\n", json_object_get_string(user, "password"));
                }
                // printf("%s",response);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "delete_user") == 0)
        {
            if (cookie[0] == NULL)
            {
                printf("no admin role!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char name[100];
                printf("username= ");
                fflush(stdout);
                scanf("%s", name);
                char url[256];
                sprintf(url, "/api/v1/tema/admin/users/%s", name);
                char *request = compute_delete_request(
                    "63.32.125.183",
                    url,
                    cookie, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: deleted user\n");
                    }

                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "logout_admin") == 0)
        {
            if (cookie[0] == NULL)
            {
                printf("no admin role!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char *request = compute_get_request(
                    "63.32.125.183",
                    "/api/v1/tema/admin/logout",
                    NULL,
                    cookie, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                cookie[0] = NULL;
                strcpy(admin_name, "");
                printf("SUCCES: admin DELOGAT!");
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "login") == 0)
        {
            if (login_status == 1)
                printf("user already logged in :(\n");
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char admin_user[100], name[100], pass[100];
                printf("admin_username= ");
                fflush(stdout);
                scanf("%s", admin_user);
                printf("username= ");
                fflush(stdout);
                scanf("%s", name);
                printf("password= ");
                fflush(stdout);
                scanf("%s", pass);
                // if(strcmp(admin_name, admin_user) != 0){
                //     printf("not same admin name :(\n");
                // }

                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "admin_username", admin_user);
                json_object_set_string(root_object, "username", name);
                json_object_set_string(root_object, "password", pass);
                char *json_string = json_serialize_to_string(root_value);
                strcpy(username, name);
                char *request = compute_post_request(
                    "63.32.125.183",
                    "/api/v1/tema/user/login",
                    "application/json",
                    json_string,
                    NULL, 0);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *cookie_start = strstr(response, "Set-Cookie:");
                if (cookie_start != NULL)
                {
                    cookie_start += strlen("Set-Cookie: ");
                    char *cookie_end = strchr(cookie_start, ';');
                    int cookie_len = cookie_end ? (cookie_end - cookie_start) : strlen(cookie_start);
                    user_cookie[0] = malloc(cookie_len + 1 + 8);
                    strcpy(user_cookie[0], "Cookie: ");
                    strncpy(user_cookie[0] + 8, cookie_start, cookie_len);
                    user_cookie[0][cookie_len + 8] = '\0';
                }
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        login_status = 1;
                        printf("SUCCESS: User logged in\n");
                    }

                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "get_access") == 0)
        {
            if (login_status == 0)
            {
                printf("no user logged in!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char *request = compute_get_request(
                    "63.32.125.183",
                    "/api/v1/tema/library/access",
                    NULL,
                    user_cookie, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "token"))
                    {
                        const char *token_str = json_object_get_string(root_obj, "token");
                        int prefix_len = strlen("Authorization: Bearer ");
                        int token_len = strlen(token_str);
                        token[0] = malloc(prefix_len + token_len + 1);
                        strcpy(token[0], "Authorization: Bearer ");
                        strcat(token[0], token_str);
                    }
                }
                // printf("%s\n",token[0]);
                json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        login_status = 1;
                        printf("SUCCESS: User got access\n");
                    }

                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "logout") == 0)
        {
            if (login_status == 0)
            {
                printf("nu este autentificat ;(\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char *request = compute_get_request(
                    "63.32.125.183",
                    "/api/v1/tema/user/logout",
                    NULL,
                    user_cookie, 1);
                send_to_server(sockfd, request);
                strcpy(username, "");
                response = receive_from_server(sockfd);
                user_cookie[0] = NULL;
                token[0] = NULL;
                strcpy(admin_name, "");
                login_status = 0;
                printf("SUCCES: user DELOGAT!");
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "get_movies") == 0)
        {
            if (token[0] == NULL)
            {
                printf("No library acces token!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char *request = compute_get_request(
                    "63.32.125.183",
                    "/api/v1/tema/library/movies",
                    NULL,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                }
                JSON_Value *root = json_parse_string(json_start);
                JSON_Object *root_obj = json_value_get_object(root);
                JSON_Array *movies = json_object_get_array(root_obj, "movies");
                int count = json_array_get_count(movies);
                printf("SUCCESS: Lista filmelor\n");

                for (int i = 0; i < count; i++)
                {
                    JSON_Object *movie = json_array_get_object(movies, i);
                    const char *title = json_object_get_string(movie, "title");
                    double id = json_object_get_number(movie, "id");
                    printf("#%.0f %s\n", id, title);
                }
                // printf("%s",response);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "add_movie") == 0)
        {
            if (token[0] == NULL)
            {
                printf("No library acces token!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
                char description[100], year[100], title[100], rating[100];
                printf("description= ");
                fflush(stdout);
                fgets(description, sizeof(description), stdin);
                description[strcspn(description, "\n")] = 0; // remove newline

                printf("year= ");
                fflush(stdout);
                fgets(year, sizeof(year), stdin);
                year[strcspn(year, "\n")] = 0;

                printf("title= ");
                fflush(stdout);
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;

                printf("rating= ");
                fflush(stdout);
                fgets(rating, sizeof(rating), stdin);
                rating[strcspn(rating, "\n")] = 0;
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "description", description);
                json_object_set_number(root_object, "year", atoi(year));
                json_object_set_string(root_object, "title", title);
                double rating_val = atof(rating);
                json_object_set_number(root_object, "rating", rating_val);

                char *json_string = json_serialize_to_string(root_value);

                char *request = compute_post_request(
                    "63.32.125.183",
                    "/api/v1/tema/library/movies",
                    "application/json",
                    json_string,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);
                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: movie added\n");
                    }
                    json_value_free(root);
                }
                // printf("%s",response);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "get_movie") == 0)
        {
            if (token[0] == NULL)
            {
                printf("No library acces token!!!\n");
            }
            else
            {
                char id[100];
                printf("id= ");
                fflush(stdout);
                scanf("%s", id);
                printf("%s\n", id);
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char url[256];
                sprintf(url, "/api/v1/tema/library/movies/%s", id);
                char *request = compute_get_request(
                    "63.32.125.183",
                    url,
                    NULL,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    // printf("DEBUG: Raw JSON response for get_movie: %s\n", json_start);
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *movie = json_value_get_object(root);

                    if (json_object_has_value(movie, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(movie, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: Detalii film\n");
                        printf("title: %s\n", json_object_get_string(movie, "title"));
                        printf("year: %.0f\n", json_object_get_number(movie, "year"));
                        printf("description: %s\n", json_object_get_string(movie, "description"));
                        printf("rating: %s\n", json_object_get_string(movie, "rating"));
                    }
                    json_value_free(root);
                }

                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "delete_movie") == 0)
        {
            if (token[0] == NULL)
            {
                printf("No library acces token!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char id[100];
                printf("id= ");
                fflush(stdout);
                scanf("%s", id);
                // printf("%s\n", id);
                char url[256];
                sprintf(url, "/api/v1/tema/library/movies/%s", id);
                char *request = compute_delete_request(
                    "63.32.125.183",
                    url,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);

                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: deleted movie\n");
                    }

                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "update_movie") == 0)
        {
            if (token[0] == NULL)
            {
                printf("No library acces token!!!\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
                char description[100], year[100], title[100], rating[100], id[100];
                printf("id= ");
                fflush(stdout);
                fgets(id, sizeof(id), stdin);
                id[strcspn(id, "\n")] = 0;

                printf("description= ");
                fflush(stdout);
                fgets(description, sizeof(description), stdin);
                description[strcspn(description, "\n")] = 0;

                printf("year= ");
                fflush(stdout);
                fgets(year, sizeof(year), stdin);
                year[strcspn(year, "\n")] = 0;

                printf("title= ");
                fflush(stdout);
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;

                printf("rating= ");
                fflush(stdout);
                fgets(rating, sizeof(rating), stdin);
                rating[strcspn(rating, "\n")] = 0;
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "description", description);
                json_object_set_number(root_object, "year", atoi(year));
                json_object_set_string(root_object, "title", title);
                json_object_set_number(root_object, "rating", atof(rating));
                char *json_string = json_serialize_to_string(root_value);
                char url[256];
                sprintf(url, "/api/v1/tema/library/movies/%s", id);
                char *request = compute_put_request(
                    "63.32.125.183",
                    url,
                    "application/json",
                    json_string,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);
                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: movie updated\n");
                    }
                    json_value_free(root);
                }
                // printf("%s",response);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "get_collections") == 0)
        {
            if (token[0] == NULL)
            {
                printf("ERROR: No library access token\n");
            }
            else
            {
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char *request = compute_get_request(
                    "63.32.125.183",
                    "/api/v1/tema/library/collections",
                    NULL,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                }
                JSON_Value *root = json_parse_string(json_start);
                /* Serverul poate returna un obiect cu "error" sau un array */
                if (json_value_get_type(root) == JSONObject)
                {
                    JSON_Object *root_obj = json_value_get_object(root);
                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        /* Raspunsul poate fi un obiect cu un camp "collections" */
                        JSON_Array *collections = json_object_get_array(root_obj, "collections");
                        if (collections != NULL)
                        {
                            int count = json_array_get_count(collections);
                            printf("SUCCESS: Lista colectiilor");
                            for (int i = 0; i < count; i++)
                            {
                                JSON_Object *col = json_array_get_object(collections, i);
                                int col_id = (int)json_object_get_number(col, "id");
                                const char *col_title = json_object_get_string(col, "title");
                                printf(" #%d: %s", col_id, col_title);
                            }
                            printf("\n");
                        }
                        else
                        {
                            printf("SUCCESS: Lista colectiilor\n");
                        }
                    }
                }
                else if (json_value_get_type(root) == JSONArray)
                {
                    /* Raspunsul e direct un array de colectii */
                    JSON_Array *collections = json_value_get_array(root);
                    int count = json_array_get_count(collections);
                    printf("SUCCESS: Lista colectiilor");
                    for (int i = 0; i < count; i++)
                    {
                        JSON_Object *col = json_array_get_object(collections, i);
                        int col_id = (int)json_object_get_number(col, "id");
                        const char *col_title = json_object_get_string(col, "title");
                        printf(" #%d: %s", col_id, col_title);
                    }
                    printf("\n");
                }
                else
                {
                    printf("ERROR: Raspuns invalid de la server\n");
                }
                json_value_free(root);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "get_collection") == 0)
        {
            if (token[0] == NULL)
            {
                printf("ERROR: No library access token\n");
            }
            else
            {
                char id[100];
                printf("id= ");
                fflush(stdout);
                scanf("%s", id);
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char url[256];
                sprintf(url, "/api/v1/tema/library/collections/%s", id);
                char *request = compute_get_request(
                    "63.32.125.183",
                    url,
                    NULL,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *col = json_value_get_object(root);
                    if (json_object_has_value(col, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(col, "error"));
                    }
                    else
                    {
                        const char *col_title = json_object_get_string(col, "title");
                        const char *col_owner = json_object_get_string(col, "owner");
                        printf("SUCCESS: Detalii colectie title: %s owner: %s", col_title, col_owner);
                        JSON_Array *movies = json_object_get_array(col, "movies");
                        if (movies != NULL)
                        {
                            int count = json_array_get_count(movies);
                            for (int i = 0; i < count; i++)
                            {
                                JSON_Object *movie = json_array_get_object(movies, i);
                                int movie_id = (int)json_object_get_number(movie, "id");
                                const char *movie_title = json_object_get_string(movie, "title");
                                printf(" #%d: %s", movie_id, movie_title);
                            }
                        }
                        printf("\n");
                    }
                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "add_collection") == 0)
        {
            if (token[0] == NULL)
            {
                printf("ERROR: No library access token\n");
            }
            else
            {
                int c;
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
                char title[100];
                printf("title= ");
                fflush(stdout);
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;

                char num_buf[16];
                int num_movies = 0;
                int ids[1000];
                printf("num_movies= ");
                fflush(stdout);
                fgets(num_buf, sizeof(num_buf), stdin);
                num_movies = atoi(num_buf);
                for (int i = 0; i < num_movies; i++)
                {
                    char id_buf[16];
                    printf("movie_id[%d]= ", i);
                    fflush(stdout);
                    fgets(id_buf, sizeof(id_buf), stdin);
                    ids[i] = atoi(id_buf);
                }
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_string(root_object, "title", title);
                char *json_string = json_serialize_to_string(root_value);
                char *request_create_coll = compute_post_request(
                    "63.32.125.183",
                    "/api/v1/tema/library/collections",
                    "application/json",
                    json_string,
                    token, 1);
                send_to_server(sockfd, request_create_coll);
                response = receive_from_server(sockfd);
                close_connection(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                int collection_id = -1;
                int coll_error = 0;
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *resp_val = json_parse_string(json_start);
                    JSON_Object *resp_obj = json_value_get_object(resp_val);
                    if (json_object_has_value(resp_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(resp_obj, "error"));
                        coll_error = 1;
                    }
                    else
                    {
                        collection_id = (int)json_object_get_number(resp_obj, "id");
                    }
                    json_value_free(resp_val);
                }
                json_free_serialized_string(json_string);
                json_value_free(root_value);
                if (!coll_error && collection_id >= 0)
                {
                    for (int i = 0; i < num_movies; i++)
                    {
                        JSON_Value *movie_val = json_value_init_object();
                        JSON_Object *movie_obj = json_value_get_object(movie_val);
                        json_object_set_number(movie_obj, "id", ids[i]);
                        char *movie_json = json_serialize_to_string(movie_val);
                        char url[128];
                        sprintf(url, "/api/v1/tema/library/collections/%d/movies", collection_id);
                        sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                        char *request_add_movie = compute_post_request(
                            "63.32.125.183",
                            url,
                            "application/json",
                            movie_json,
                            token, 1);
                        send_to_server(sockfd, request_add_movie);
                        char *movie_resp = receive_from_server(sockfd);
                        close_connection(sockfd);
                        free(movie_resp);
                        json_free_serialized_string(movie_json);
                        json_value_free(movie_val);
                    }
                    printf("SUCCESS: Colectie adaugata\n");
                }
            }
        }
        else if (strcmp(command, "delete_collection") == 0)
        {
            if (token[0] == NULL)
            {
                printf("ERROR: No library access token\n");
            }
            else
            {
                char id[100];
                printf("id= ");
                fflush(stdout);
                scanf("%s", id);
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char url[256];
                sprintf(url, "/api/v1/tema/library/collections/%s", id);
                char *request = compute_delete_request(
                    "63.32.125.183",
                    url,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);
                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: Colectie stearsa\n");
                    }
                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "add_movie_to_collection") == 0)
        {
            if (token[0] == NULL)
            {
                printf("ERROR: No library access token\n");
            }
            else
            {
                char collection_id[100], movie_id[100];
                printf("collection_id= ");
                fflush(stdout);
                scanf("%s", collection_id);
                printf("movie_id= ");
                fflush(stdout);
                scanf("%s", movie_id);
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char url[256];
                sprintf(url, "/api/v1/tema/library/collections/%s/movies", collection_id);
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);
                json_object_set_number(root_object, "id", atoi(movie_id));
                char *json_string = json_serialize_to_string(root_value);
                char *request = compute_post_request(
                    "63.32.125.183",
                    url,
                    "application/json",
                    json_string,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);
                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: Film adaugat in colectie\n");
                    }
                    json_value_free(root);
                }
                json_free_serialized_string(json_string);
                json_value_free(root_value);
                close_connection(sockfd);
            }
        }
        else if (strcmp(command, "delete_movie_from_collection") == 0)
        {
            if (token[0] == NULL)
            {
                printf("ERROR: No library access token\n");
            }
            else
            {
                char collection_id[100], movie_id[100];
                printf("collection_id= ");
                fflush(stdout);
                scanf("%s", collection_id);
                printf("movie_id= ");
                fflush(stdout);
                scanf("%s", movie_id);
                int sockfd = open_connection("63.32.125.183", 8081, AF_INET, SOCK_STREAM, 0);
                char url[256];
                sprintf(url, "/api/v1/tema/library/collections/%s/movies/%s", collection_id, movie_id);
                char *request = compute_delete_request(
                    "63.32.125.183",
                    url,
                    token, 1);
                send_to_server(sockfd, request);
                response = receive_from_server(sockfd);
                char *json_start = strstr(response, "\r\n\r\n");
                if (json_start != NULL)
                {
                    json_start += 4;
                    JSON_Value *root = json_parse_string(json_start);
                    JSON_Object *root_obj = json_value_get_object(root);
                    if (json_object_has_value(root_obj, "error"))
                    {
                        printf("ERROR: %s\n", json_object_get_string(root_obj, "error"));
                    }
                    else
                    {
                        printf("SUCCESS: Film sters din colectie\n");
                    }
                    json_value_free(root);
                }
                close_connection(sockfd);
            }
        }
    }

    // close_connection(sockfd);

    return 0;
}
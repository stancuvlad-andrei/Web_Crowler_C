#include "animation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <curl/curl.h>

// Structure to hold information about hyperlinks
struct Hyperlink {
    char *url;
    struct Hyperlink *next;
};
struct Hyperlink *hyperlinks = NULL;

// main----------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    // Control the number of arhuments
    if(argc != 2){
        fprintf(stderr, "Error, not enough arguments!");
        exit(1);
    }

    // Control what is the argument
    if(strstr(argv[1], "http") != argv[1]) {
        int fd;
        fd = open(argv[1], O_RDONLY);
        if(fd < 0) {
            fprintf(stderr, "The argument is neither an url nor a file");
            exit(1);
        }else{
            int id = fork();
            if(id == 0){
                animation();
            }else{
                static char buffer[2049];
                int bytesRead;
                freopen("output.txt", "w", stdout);
                while ((bytesRead = read_line(fd, buffer, sizeof(buffer))) > 0) {
                    // Perform the HTTP request
                    perform_http_request(buffer);

                    // Print hyperlinks
                    print_hyperlinks();
                }
                free_hyperlinks();
                close(fd);
                wait(NULL);
                freopen("/dev/tty", "w", stdout);
                printf("Check your output file :)");
            }
        }
    }else {
        int id = fork();
        if(id == 0){
            animation();
        }else{
            freopen("output.txt", "w", stdout);

            // Perform the HTTP request
            perform_http_request(argv[1]);

            // Print hyperlinks and free them
            print_hyperlinks();
            free_hyperlinks();

            wait(NULL);
            freopen("/dev/tty", "w", stdout);
            printf("Check your output file :)");
        }
    }
    return 0;
}

// Function to perform HTTP GET request------------------------------------------------

void perform_http_request(const char *url) {
    CURL *curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL to crawl
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the callback function for handling the HTTP response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Perform the HTTP request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// Callback function for handling HTTP response---------------------------------------

int write_callback(void *contents, int size, int nmemb, void *userp) {
    int realsize = size * nmemb;
    char *html_content = (char *)malloc(realsize + 1);

    if (html_content == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }

    memcpy(html_content, contents, realsize);
    html_content[realsize] = '\0';

    char *start = html_content;
    char *end;

    // Search for <a></a> tags 
    while ((start = strstr(start, "<a")) != NULL) {
        end = strstr(start, "</a>");

        if (end != NULL) {
            // Extract the hyperlink
            char *href_start = strstr(start, "href=\"");
            if (href_start != NULL) {
                href_start += 6; // Move to the actual URL

                char *href_end = strchr(href_start, '\"');

                // Allocate memory for the URL and copy it
                int url_length = href_end - href_start;
                char *url = (char *)malloc(url_length + 1);
                strncpy(url, href_start, url_length);
                url[url_length] = '\0';

                // Store the URL in the list of hyperlinks
                struct Hyperlink *new_link = (struct Hyperlink *)malloc(sizeof(struct Hyperlink));
                new_link->url = url;
                new_link->next = hyperlinks;
                hyperlinks = new_link;
            }
            // Skip "</a>"
            start = end + 4;
        } else {
            break;
        }
    }
    free(html_content);
    return realsize;
}

// Function to read lines from the file argument--------------------------------------

int read_line(int fd, char *buffer, int max_length) {
    int bytesRead = 0;
    char c;

    while (bytesRead < max_length - 1) {
        int result = read(fd, &c, 1);

        if (result > 0) {
            buffer[bytesRead] = c;
            bytesRead++;

            // Stop reading on encountering newline
            if (c == '\n') {
                break;
            }

            // Reached end of file
        } else if (result == 0) {
            break;
        } else {
            perror("Error reading from file");
            break;
        }
    }

    if (bytesRead > 0 && buffer[bytesRead - 1] == '\n') {
        bytesRead--;
        buffer[bytesRead] = '\0';  // Null-terminate the string
    } else {
        buffer[bytesRead] = '\0';  // Null-terminate the string
    }
    return bytesRead;
}

// Function to print hyperlinks-------------------------------------------------------

void print_hyperlinks() {
    struct Hyperlink *current = hyperlinks;

    while (current != NULL) {
        printf("Hyperlink: %s\n", current->url);
        current = current->next;
    }
}

// Function to free memory------------------------------------------------------------

void free_hyperlinks() {
    struct Hyperlink *current = hyperlinks;
    struct Hyperlink *next;

    while (current != NULL) {
        next = current->next;
        free(current->url);
        free(current);
        current = next;
    }
}
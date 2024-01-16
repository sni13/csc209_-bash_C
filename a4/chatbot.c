#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "chatserver.h"
#include "response.h"
#define PORT 3000
   
/**
 * Block read from user
 */
int get_from_user(char *buffer, int size)
{
    int cnt = 0;
    char c;
    if (buffer == NULL || size == 0)
    {
        return 0;
    }
    while (read(STDIN_FILENO, &c, sizeof c) == 1 && cnt < size - 2)
    {
        if (c == '\n')
        {
            buffer[cnt++] = '\r';
            buffer[cnt] = '\n';
            return 1;
        }
        buffer[cnt++] = c;
    }
    buffer[cnt++] = '\r';
    buffer[cnt] = '\n';
    return 1;
}

/***
 * Block read from server
 */
int get_from_server(int sock, char *buffer, int size)
{
    int cnt = 0;
    char c;
    if (buffer == NULL || size == 0)
    {
        return 0;
    }
    while (read(sock, &c, sizeof c) == 1 && cnt < size - 1)
    {
        if (c == '\r')
        {
            buffer[cnt] = 0;
            read(sock, &c, sizeof c);
            return 1;
        }
        buffer[cnt++] = c;
    }
    buffer[cnt] = 0;
    return 1;
}

void remove_spaces(char *s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}

void to_lower(char *s)
{
    char *p;
    for (p = s; *p != 0; p++)
    {
        *p = *p > 0x40 && *p < 0x5b ? *p | 0x60 : *p;
    }
}

char *canonicalize(char *query)
{
    // remove all spaces:
    remove_spaces(query);
    // to lowercase
    to_lower(query);
    // skip leading what, how, who, why, where
    int start = 0;
    char buffer[10];
    // check how, who, why
    memcpy(buffer, query, 3);
    buffer[3] = '\0';
    if (strcmp(buffer, "how") == 0 || strcmp(buffer, "who") == 0 || strcmp(buffer, "why") == 0)  
    {
        start = 3;
    }
    // check what
    memcpy(buffer, query, 4);
    buffer[4] = '\0';
    if (strcmp(buffer, "what") == 0)
    {
        start = 4;
    }
    // check where
    memcpy(buffer, query, 5);
    buffer[5] = '\0';
    if (strcmp(buffer, "where") == 0)
    {
        start = 5;
    }

    query += start;
    start = 0;

    // skip is, 's, isn't, are, 're, aren't, do, does, doesn't, don't
    // check is, 's, do
    memcpy(buffer, query, 2);
    buffer[2] = '\0';
    if (strcmp(buffer, "is") == 0 || strcmp(buffer, "'s") == 0 || strcmp(buffer, "do") == 0)
    {
        start = 2;
    }
    // check are, 're
    memcpy(buffer, query, 3);
    buffer[3] = '\0';
    if (strcmp(buffer, "are") == 0 || strcmp(buffer, "'re") == 0)
    {
        start = 3;
    }
    // check does
    memcpy(buffer, query, 4);
    buffer[4] = '\0';
    if (strcmp(buffer, "does") == 0)
    {
        start = 4;
    }
    // check isn't don't
    memcpy(buffer, query, 5);
    buffer[5] = '\0';
    if (strcmp(buffer, "isn't") == 0 || strcmp(buffer, "don't") == 0)
    {
        start = 5;
    }
    // check aren't
    memcpy(buffer, query, 6);
    buffer[6] = '\0';
    if (strcmp(buffer, "aren't") == 0)
    {
        start = 6;
    } 
    // check doesn't
    memcpy(buffer, query, 7);
    buffer[7] = '\0';
    if (strcmp(buffer, "doesn't") == 0)
    {
        start = 7;
    }    

    query += start;
    start = 0;
    
    // skip the
    memcpy(buffer, query, 3);
    buffer[3] = '\0';
    if (strcmp(buffer, "the") == 0)
    {
        start = 3;
    }

    
    return query + start;
}

char* process_query(char *query)
{
    char *canonical_query = canonicalize(query);
    return response(canonical_query);
} 

char* process (char *message)
{
    int start = 0;
    // skip bot name and colon
    while (message[start] != ':')
    {
        if (message[start] == 0) return NULL;
        start++;
    }
    start += 2;

    // skip leading , . : !
    while (start < MAXMESSAGE)
    {
        if (message[start] == ',' || message[start] == '.' || message[start] == ':' || message[start] == '!') start++;
        else break;
    }
    char *query_candidate = message + start;
    int i;
    for (i = 0; i < MAXMESSAGE; i++)
    {
        if (query_candidate[i] == '?')
        {
              query_candidate[i] = 0;
              return process_query(query_candidate);
        }
        if (query_candidate[i] == 0)
        {
              // no ? in the sentence
              return NULL;
        }
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    // user handle
    char handle[MAXHANDLE];
    // server message
    char message[MAXMESSAGE];
    // user message
    char user_message[MAXMESSAGE];
    // host ip
    struct hostent *hp;
    int sock;
    struct sockaddr_in serv_addr;


    // parse argument
    int port = 3000;
    if (argc < 2 || argc > 3) 
    {
        printf("\n usage: ./chatbot host [port] \n");
        return -1;
    }
 
    if (argc == 3)
    {
        port = atoi(argv[2]);
    }

    // parse host IP
    if ((hp = gethostbyname(argv[1])) == NULL)
    {
        printf("%s: no such host", argv[1]);
        return -1;
    }

    if (hp -> h_addr_list[0] == NULL || hp -> h_addrtype != AF_INET)
    {
        printf("%s: not an internet protocol host name", argv[1]);
        return -1;
    }

    // socket creation and connection
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // host
    memcpy(&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length); 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // protocol: read server banner from server
    get_from_server(sock, message, MAXMESSAGE);
    if (strcmp(message, chatserver_banner) != 0)
    {
        printf("%s\n", message);
        printf("Talking to wrong server\n");
        close(sock);
        return -1;
    }
    
    // send handle
    int offset = 0;
    int len_botname = strlen(botname);
    // check handle size
    if (len_botname + 5 > MAXHANDLE)
    {
        printf("Bot Name Length Exceeded Maximum Handle Size\n");
        close(sock);
        return -1;
    }
    while (offset < len_botname)
    {
        handle[offset] = botname[offset];
        offset++;
    }
    handle[offset++] = 'b';
    handle[offset++] = 'o';
    handle[offset++] = 't';
    handle[offset++] = '\r';
    handle[offset] = '\n';
    send(sock , handle, len_botname + 5, 0);


    //main loop: listen for socket and stdin input
    int max_fd = sock > STDIN_FILENO ? sock : STDIN_FILENO;
    int server_offset = 0;
    printf("%d, %d, %d\n", sock, STDIN_FILENO, max_fd);
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(STDIN_FILENO, &all_fds);
    FD_SET(sock, &all_fds);
    while (1)
    {
        fd_set fds = all_fds;
        int nready;
        if ((nready = select(max_fd+1, &fds, NULL, NULL, NULL)) == -1)
        {
            printf("Error: Select\n");
            close(sock);
            return -1;
        }

        // stdin
        if (FD_ISSET(STDIN_FILENO, &fds))
        {
            // read from stdin
            get_from_user(user_message, MAXMESSAGE);
            send(sock, user_message, MAXMESSAGE, 0);
        }

        // server
        if (FD_ISSET(sock, &fds))
        {
            char c;
            int nread = read(sock, &c, sizeof c);
            if (nread == 0)
            {
                printf("server disconnected\n");
                close(sock);
                return -1;
            }
            if (c == '\n')
            {
                message[server_offset] = 0;
                printf("%s\n", message);
                server_offset = 0;
                char *answer = process(message);
                if (answer != NULL)
                {
                    send(sock, answer, strlen(answer), 0);
                } 
            }
            else
            {
                message[server_offset++] = c;
            }
        }
    }
}

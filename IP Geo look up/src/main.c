#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wininet.h>
#include <winsock2.h>

void set_window_properties() {
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 500, 700, TRUE);
    SetWindowLong(console, GWL_STYLE, 
        GetWindowLong(console, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    SetConsoleTitle("Void GeoLookUp");
}

void show_progress(int current, int total) {
    int bar_width = 50;
    float progress = (float)current / total;
    int pos = bar_width * progress;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    printf("\r[");
    for (int i = 0; i < bar_width; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%", (int)(progress * 100));
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    fflush(stdout);
}

void print_banner() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    printf("\n");
    printf("    +--------------------------------------------------+\n");
    printf("    |  __      __   _     _    ____            _       |\n");
    printf("    |  \\ \\    / /  (_)   | |  / __ \\          | |   |\n");
    printf("    |   \\ \\  / /__  _  __| | | |  | |_ __   __| |    |\n");
    printf("    |    \\ \\/ / _ \\| |/ _` | | |  | | '_ \\ / _` |  |\n");
    printf("    |     \\  / (_) | | (_| | | |__| | | | | (_| |     |\n");
    printf("    |      \\/ \\___/|_|\\__,_|  \\____/|_| |_|\\__,_| |\n");
    printf("    |            Network Intelligence Tool             |\n");
    printf("    |              Created by Mr.Time                  |\n");
    printf("    +--------------------------------------------------+\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void scan_ports(const char* ip) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    fd_set fdset;
    struct timeval tv;
    int total_ports = 1024;
    int open_ports = 0;
    
    printf("\n[*] Starting Advanced Port Scan..\n\n");
    WSAStartup(MAKEWORD(2,2), &wsa);
    
    for(int port = 1; port <= total_ports; port++) {
        show_progress(port, total_ports);
        
        sock = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_addr.s_addr = inet_addr(ip);
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        
        u_long mode = 1;
        ioctlsocket(sock, FIONBIO, &mode);
        
        connect(sock, (struct sockaddr *)&server, sizeof(server));
        
        FD_ZERO(&fdset);
        FD_SET(sock, &fdset);
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
        
        if (select(0, NULL, &fdset, NULL, &tv) == 1) {
            int so_error;
            int len = sizeof(so_error);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len);
            
            if (so_error == 0) {
                open_ports++;
                printf("\n");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                printf("[+] Port %d: OPEN", port);
                
                char* service = "Unknown";
                switch(port) {
                    case 21: service = "FTP"; break;
                    case 22: service = "SSH"; break;
                    case 23: service = "Telnet"; break;
                    case 25: service = "SMTP"; break;
                    case 53: service = "DNS"; break;
                    case 80: service = "HTTP"; break;
                    case 110: service = "POP3"; break;
                    case 143: service = "IMAP"; break;
                    case 443: service = "HTTPS"; break;
                    case 3306: service = "MySQL"; break;
                    case 3389: service = "RDP"; break;
                    case 5432: service = "PostgreSQL"; break;
                    case 8080: service = "HTTP-Proxy"; break;
                }
                printf(" | Service: %s\n", service);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
        }
        closesocket(sock);
    }
    
    printf("\n\n[*] Scan Complete! Found %d open ports.\n", open_ports);
    WSACleanup();
}

void get_geo_info(const char* ip) {
    HINTERNET hInternet = InternetOpen("Void GeoLookUp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if(hInternet) {
        char url[256];
        sprintf(url, "http://ip-api.com/line/%s?fields=status,message,continent,country,countryCode,region,regionName,city,district,zip,lat,lon,timezone,offset,currency,isp,org,as,asname,reverse,mobile,proxy,hosting,query", ip);
        HINTERNET hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if(hConnect) {
            char buffer[2048];
            DWORD bytesRead;
            
            printf("\n+------------------- Target Information -------------------+\n");
            printf("| IP Address: %-45s |\n", ip);
            
            while(InternetReadFile(hConnect, buffer, sizeof(buffer)-1, &bytesRead) && bytesRead > 0) {
                buffer[bytesRead] = 0;
                char* line = strtok(buffer, "\n");
                while(line) {
                    if(strlen(line) > 2) {
                        printf("| %-52s |\n", line);
                    }
                    line = strtok(NULL, "\n");
                }
            }
            printf("+------------------------------------------------------+\n");
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
}

int main() {
    char ip[50];
    set_window_properties();
    
    while(1) {
        system("cls");
        print_banner();
        
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        printf("\n[>] Enter IP address (or 'exit' to quit): ");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        
        scanf("%s", ip);
        
        if(strcmp(ip, "exit") == 0) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
            printf("\n[*] Thanks for using Void GeoLookUp!\n");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            Sleep(1000);
            break;
        }
        
        get_geo_info(ip);
        scan_ports(ip);
        
        printf("\n[*] Press Enter to scan another IP...");
        getchar();
        getchar();
    }
    
    return 0;
}

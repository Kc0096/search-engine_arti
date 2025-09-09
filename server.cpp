#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

struct Entry {
    std::string keyword;
    std::string pdf;
    std::string gfg;
    std::string wiki;
    std::string yt;
};

// Initialize local database with 8 programming topics
std::vector<Entry> initializeDatabase() {
    std::vector<Entry> db;
    
    db.push_back({"JavaScript", 
                  "https://eloquentjavascript.net/Eloquent_JavaScript.pdf",
                  "https://www.geeksforgeeks.org/javascript/",
                  "https://en.wikipedia.org/wiki/JavaScript",
                  "https://www.youtube.com/results?search_query=javascript+tutorial"});
                  
    db.push_back({"Python", 
                  "https://www.py4e.com/book.php",
                  "https://www.geeksforgeeks.org/python-programming-language/",
                  "https://en.wikipedia.org/wiki/Python_(programming_language)",
                  "https://www.youtube.com/results?search_query=python+programming+tutorial"});
                  
    db.push_back({"React", 
                  "https://reactjs.org/docs/getting-started.html",
                  "https://www.geeksforgeeks.org/react-js-introduction/",
                  "https://en.wikipedia.org/wiki/React_(JavaScript_library)",
                  "https://www.youtube.com/results?search_query=react+js+tutorial"});
                  
    db.push_back({"Node.js", 
                  "https://nodejs.org/en/docs/",
                  "https://www.geeksforgeeks.org/nodejs/",
                  "https://en.wikipedia.org/wiki/Node.js",
                  "https://www.youtube.com/results?search_query=nodejs+tutorial"});
                  
    db.push_back({"Data Structures", 
                  "https://opendatastructures.org/ods-java.pdf",
                  "https://www.geeksforgeeks.org/data-structures/",
                  "https://en.wikipedia.org/wiki/Data_structure",
                  "https://www.youtube.com/results?search_query=data+structures+algorithms"});
                  
    db.push_back({"Algorithms", 
                  "https://mitpress.mit.edu/books/introduction-algorithms-third-edition",
                  "https://www.geeksforgeeks.org/fundamentals-of-algorithms/",
                  "https://en.wikipedia.org/wiki/Algorithm",
                  "https://www.youtube.com/results?search_query=algorithms+tutorial"});
                  
    db.push_back({"Machine Learning", 
                  "https://www.statlearning.com/",
                  "https://www.geeksforgeeks.org/machine-learning/",
                  "https://en.wikipedia.org/wiki/Machine_learning",
                  "https://www.youtube.com/results?search_query=machine+learning+tutorial"});
                  
    db.push_back({"Web Development", 
                  "https://developer.mozilla.org/en-US/docs/Learn",
                  "https://www.geeksforgeeks.org/web-development/",
                  "https://en.wikipedia.org/wiki/Web_development",
                  "https://www.youtube.com/results?search_query=web+development+tutorial"});
    
    return db;
}

// Search database for query
std::vector<Entry> search(const std::vector<Entry> &db, const std::string &query) {
    std::vector<Entry> results;
    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);

    for (const auto &e : db) {
        std::string k = e.keyword;
        std::transform(k.begin(), k.end(), k.begin(), ::tolower);
        if (k.find(q) != std::string::npos) results.push_back(e);
    }
    return results;
}

// Read file content as string
std::string getFileContent(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        // Return simple HTML if index.html not found
        if (filename == "index.html") {
            return "<!DOCTYPE html><html><head><title>CodeSearch</title></head><body><h1>CodeSearch Server</h1><p>Please create index.html, style.css, and script.js files in the same directory as the server.</p></body></html>";
        }
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// URL decode helper
std::string urlDecode(const std::string &SRC) {
    std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < (int)SRC.length(); i++) {
        if (int(SRC[i]) == 37) {
            sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i += 2;
        } else if (SRC[i] == '+') ret += ' ';
        else ret += SRC[i];
    }
    return ret;
}

// Determine MIME type based on file extension
std::string getMimeType(const std::string &path) {
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".ico") != std::string::npos) return "image/x-icon";
    return "text/html"; // default
}

// Escape JSON strings
std::string escapeJson(const std::string &input) {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c; break;
        }
    }
    return output;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET server_fd, client;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialize local database instead of loading from file
    auto database = initializeDatabase();
    std::cout << "Loaded " << database.size() << " entries in local database\n";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 10) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "CodeSearch Server running at http://localhost:" << PORT << std::endl;
    std::cout << "Available topics: JavaScript, Python, React, Node.js, Data Structures, Algorithms, Machine Learning, Web Development\n";

    while (true) {
        client = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }

        char buffer[8192] = {0};
        int bytesRead = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            closesocket(client);
            continue;
        }
        buffer[bytesRead] = '\0';

        std::string request(buffer);
        std::string response;

        // Add CORS headers for cross-origin requests
        std::string corsHeaders = "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n";

        if (request.find("OPTIONS") == 0) {
            // Handle preflight CORS requests
            response = "HTTP/1.1 200 OK\r\n" + corsHeaders + "Content-Length: 0\r\n\r\n";
        }
        else if (request.find("GET /search?query=") != std::string::npos) {
            // Handle search API
            size_t queryPos = request.find("query=") + 6;
            size_t endPos = request.find(" ", queryPos);
            if (endPos == std::string::npos) endPos = request.find("&", queryPos);
            if (endPos == std::string::npos) endPos = request.find("\r", queryPos);
            
            std::string query = request.substr(queryPos, endPos - queryPos);
            query = urlDecode(query);

            auto results = search(database, query);
            std::cout << "Search query: '" << query << "' - Found " << results.size() << " results\n";

            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < results.size(); i++) {
                oss << "{";
                oss << "\"keyword\":\"" << escapeJson(results[i].keyword) << "\",";
                oss << "\"pdf\":\"" << escapeJson(results[i].pdf) << "\",";
                oss << "\"gfg\":\"" << escapeJson(results[i].gfg) << "\",";
                oss << "\"wiki\":\"" << escapeJson(results[i].wiki) << "\",";
                oss << "\"yt\":\"" << escapeJson(results[i].yt) << "\"";
                oss << "}";
                if (i != results.size() - 1) oss << ",";
            }
            oss << "]";
            std::string jsonData = oss.str();

            response = "HTTP/1.1 200 OK\r\n" + corsHeaders + "Content-Type: application/json\r\nContent-Length: "
                + std::to_string(jsonData.size()) + "\r\n\r\n" + jsonData;

        } else {
            // Serve static files
            std::string path = "index.html"; // default
            size_t start = request.find("GET ") + 4;
            size_t end = request.find(" ", start);
            if (start != std::string::npos && end != std::string::npos) {
                std::string reqPath = request.substr(start, end - start);
                if (reqPath != "/") {
                    if (reqPath[0] == '/') reqPath.erase(0, 1);
                    path = reqPath;
                }
            }

            size_t qpos = path.find("?");
            if (qpos != std::string::npos) path = path.substr(0, qpos);

            std::string content = getFileContent(path);
            if (content.empty()) {
                std::string notFound = "<h1>404 Not Found</h1><p>The requested file was not found.</p>";
                response = "HTTP/1.1 404 Not Found\r\n" + corsHeaders + "Content-Length: " + std::to_string(notFound.size())
                    + "\r\nContent-Type: text/html\r\n\r\n" + notFound;
            } else {
                response = "HTTP/1.1 200 OK\r\n" + corsHeaders + "Content-Type: " + getMimeType(path)
                    + "\r\nContent-Length: " + std::to_string(content.size())
                    + "\r\n\r\n" + content;
            }
        }

        send(client, response.c_str(), (int)response.size(), 0);
        closesocket(client);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
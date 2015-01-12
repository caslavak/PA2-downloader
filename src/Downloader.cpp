#include "Downloader.h"
using namespace std;

//Downloader::Downloader(int maxDepth, bool ignoreRedirects, const std::string & outputDirectory)
//: options(move(Options(maxDepth, ignoreRedirects, outputDirectory))) {
//    manager = Filemanager(outputDirectory, options.verbose);
//}

Downloader::Downloader(const Options & opts)
: options(opts) {
    manager = Filemanager(options.outputDirectory, options.verbose);
}

void Downloader::getPage(const Address & addr, int currentDepth) {
    int sd = newConnection(addr.host.c_str()); //connect to host

    //send GET header
    string header;
    header += "GET ";
    header += addr.path;
    header += " HTTP/1.0\r\n";
    header += "Host: ";
    header += addr.host;
    header += "\r\n";
    header += "Connection: close\r\n\r\n";
    //cout << header << endl << endl; //TEST
    // cout <<endl;
    if (send(sd, header.c_str(), header.length(), 0) < 0) throw ConnectionErrorException("Unable to send header.");

    string response;

    cout << addr.host + addr.path << endl;

    int statusCode;
    statusCode = getResponse(sd, response);

    size_t start = 0;

    if (statusCode >= 500) { //server errors
        manager.createPage500(addr);

    } else if (statusCode >= 400) { //not found (probably)
        manager.createPage404(addr);

    } else if (statusCode >= 300) { //redirects
        string address = getNewLocation(response);

        Address a = parseAddress(address);
        string tmpPath = a.host + a.path;
        if (tmpPath.back() == '/') {
            tmpPath += "auto_index.html";
        }
        manager.createRedirect(addr, getRelativeFromAbsolute(addr.host + addr.path, tmpPath));

        Page p;
        if (tabu.find(a) == tabu.end()) {
            p.address = a;
            if (options.ignoreRedirects) p.depth = currentDepth;
            else p.depth = currentDepth + 1;
            queue.push_back(p);
            tabu.insert(a);
        }

    } else if (statusCode >= 200) { //OK
        if ((start = response.find("\n\r\n")) == string::npos) {
            if ((start = response.find("\n\0\r\0\n\0")) == string::npos) {
                throw BadHeaderException("Content is probably missing.");
            } else start += 6;
        } else start += 3;

        response = response.substr(start, string::npos);

        //process tags
        try {
            TagA(options).process(response, *this, addr, currentDepth);
            TagOther(options).process(response, *this, addr, currentDepth);
        } catch (...) {
        }
        try {
            manager.createPage(addr.host, addr.path, response);
        } catch (...) {
            manager.createPageSaveError(addr); //if this also throws, it terminates program
        }
    } else throw WTFException("Unknown header");
}

void Downloader::getRawPage(const Address & addr, int currentDepth) {
    int sd = newConnection(addr.host.c_str()); //connect to host
    cout << addr.host << addr.path << endl;

    //send GET header
    string header;
    header += "GET ";
    header += addr.path;
    header += " HTTP/1.0\r\n";
    header += "Host: ";
    header += addr.host;
    header += "\r\n";
    header += "Connection: close\r\n\r\n";

    if (send(sd, header.c_str(), header.length(), 0) < 0) throw ConnectionErrorException("Unable to send header.");

    char buffer[1];
    buffer[0] = 0;
    vector<char> rawData;
    bool newline = false;
    bool readData = false;
    // int x = 0;
    //int a = 0;
    int waits = 0;
    while (1) {
        errno = 0;
        while (recv(sd, buffer, sizeof ( buffer), MSG_DONTWAIT) > 0) {
            waits=0;
            //a++;
            //if ((a % 1000) == 0) cout << "\r" << a << "B";

            //if (x == -1) throw ConnectionErrorException("Error while reading data from the socket.");
            if (readData) {
                rawData.push_back((char) buffer[0]);
            } else {
                if (buffer[0] == '\r' || buffer[0] == '\0' || buffer[0] == 0) continue;
                if (buffer[0] == '\n') {
                    if (newline) {
                        readData = true;
                    } else {
                        newline = true;
                    }
                } else newline = false;
            }
            buffer[0] = 0;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            waits++;
            if (waits > 2000) {
                break; //wait for data at most 0.2 s
            }
            usleep(100);
        } else break;
        
        buffer[0] = 0;
    }
    close(sd);

    try {
        manager.createRawPage(addr.host, addr.path, rawData);
    } catch (...) {
        manager.createPageSaveError(addr);
    }
}

const Address Downloader::parseAddress(const std::string & address, const Address & parentAddress) const {
    Address result;
    regex_t reg;
    regmatch_t match;
    size_t start = 0;
    bool absolute = false;
    const char * ad = address.c_str();

    if (regcomp(&reg, "^(http[s]?://|ftp://|//|/)", REG_EXTENDED | REG_ICASE) != 0) exit(2);
    if (regexec(&reg, ad, 1, &match, 0) == 0) {
        if (match.rm_eo == 1) {
            result.host = parentAddress.host;
            result.path = ad;
            //cout << result;
            regfree(&reg);
            return result;
        }
        start = match.rm_eo;
        ad += match.rm_eo;
        absolute = true;
    }
    regfree(&reg);

    if (absolute) {
        if (regcomp(&reg, "^[^/]+/", REG_EXTENDED | REG_ICASE) != 0) exit(2);
        if (regexec(&reg, ad, 1, &match, 0) == 0) {
            result.host = address.substr(start, match.rm_eo - 1);
            result.path = address.substr(start + match.rm_eo - 1, string::npos);
        } else {
            result.host = address.substr(start, string::npos);
        }
        regfree(&reg);
    } else {
        if (regcomp(&reg, "^\\./", REG_EXTENDED | REG_ICASE) != 0) exit(2);
        if (regexec(&reg, ad, 1, &match, 0) == 0) {
            start += 2;
            ad += 2;
        }
        regfree(&reg);

        int up = 0;
        if (regcomp(&reg, "^(\\.\\./)*", REG_EXTENDED | REG_ICASE) != 0) exit(2);
        if (regexec(&reg, ad, 1, &match, 0) == 0) {
            //cout << "Vynoreni: " << match.rm_eo / 3 << endl;
            up = match.rm_eo / 3;
            ad += match.rm_eo;
            start += match.rm_eo;
        }
        regfree(&reg);

        stringstream ss;
        ss << "([^/]+/){" << up << "}[^/]*$";
        //cout << ss.str() << endl;
        // cout << parentAddress.path << endl;
        if (regcomp(&reg, ss.str().c_str(), REG_EXTENDED | REG_ICASE) != 0) exit(2);
        if (regexec(&reg, parentAddress.path.c_str(), 1, &match, 0) == 0) {
            // cout << "smazat: " << match.rm_so << " - " << match.rm_eo << endl;
            result.host = parentAddress.host;
            result.path = parentAddress.path.substr(0, match.rm_so) + ad;
        } else {
            regfree(&reg);
            throw InvalidAddressException(string("The relative path \"" + address + "\" is invalid.").c_str());
        }
        regfree(&reg);
    }
    return result;
}

int Downloader::newConnection(const char * address) const {
    struct addrinfo * ai = NULL;
    int sd; //socket descriptor
    //resolve host
    if (getaddrinfo(address, "80", NULL, &ai)) { //hard coded port 80 fort HTTP
        throw InvalidAddressException((string("Error while getting address info for ") + string(address)).c_str());
    }

    //open socket
    ;
    if ((sd = socket(ai -> ai_family, SOCK_STREAM, 0)) == -1) {
        freeaddrinfo(ai);
        throw ConnectionErrorException("Error while openning socket.");
    }

    //connect to host
    if (connect(sd, ai -> ai_addr, ai -> ai_addrlen) == -1) {
        close(sd);
        freeaddrinfo(ai);
        throw ConnectionErrorException("Unable to connect to the server.");
    }
    freeaddrinfo(ai);

    return sd;
}

int Downloader::getResponse(int sd, string & response) const {
    char buffer[1]; //larger buffer causes misreadings
    ofstream file;
    int x = 0;
    buffer[0] = 0;
    //int size = 0;
    //fill_n(buffer, 50, 0);

    bool fine = true;
    int waits = 0;
    while (1) {
        
        errno = 0;
        while ((x = recv(sd, buffer, sizeof ( buffer), MSG_DONTWAIT)) > 0) {
            // size += x;
            //if ((size % 1000) == 0) cout << "\r" << size << "B";
            // buffer[x + 1] = '\0'; //add NULL character
            if(buffer[0] != 0) response += buffer[0];
            
            buffer[0] = 0;
            fine = true;
        }
        //cout << "vv " << x << endl;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (fine) waits = 0;
            waits++;
            if (waits > 2000) break; //wait for data at most 0.2 second
            usleep(100);
            fine = false;
        } else {
           // cout << strerror(errno) << endl;
            break;
        }
    }
    close(sd);
    int status = 0;
    try {
        status = getHeaderType(response); //get HTTP status code
    } catch (...) {
        throw BadHeaderException("Unable to get HTTP status code");
    }

    return status;
}

int Downloader::getHeaderType(const string & header) const {
    string status = header.substr(0, header.find("\n")); //cut out first row
    int i = status.find(" ");
    status = status.substr(i + 1, status.find(" ", i + 1) - i); //save only status code

    return stoi(status); //ignore warning, it's not for some reason implemented in Cygwin...
}

string Downloader::getNewLocation(const string & header) const {
    int i = header.find("Location");
    int j = header.find("\n", i);
    string loc = header.substr(i, j); //cut out row with "Location: " tag
    loc = loc.substr(10, j - i - 11); //extract the new address, -1 for \r
    cout << "Redirecting to: " << loc << endl; //INFO
    return move(loc);
}

//addresses must be in format: "something.cz/else/index.xxx"

std::string Downloader::getRelativeFromAbsolute(const std::string & root, const std::string & toConvert) const {
    // cout << root << " - " << toConvert;
    size_t slash1 = 0, slash2 = 0;
    size_t lastSlash1 = 0, lastSlash2 = 0;
    size_t end = 0;
    int depth = 0;
    int offset = 0;
    bool countOffset = true;
    //cout << toConvert << endl;
    while ((slash1 = root.find("/", slash1 + 1)) != string::npos) depth++;
    slash1 = 0;

    while (1) { //cycle through addresses to determine depth and offset
        slash1 = root.find("/", slash1 + 1);
        slash2 = toConvert.find("/", slash2 + 1);
        if (slash1 == string::npos || slash2 == string::npos) break;

        if (countOffset) {
            //cout << root.substr(lastSlash1, slash1 - lastSlash1) << "  " << toConvert.substr(lastSlash2, slash2 - lastSlash2) << endl;
            if (root.substr(lastSlash1, slash1 - lastSlash1) == toConvert.substr(lastSlash2, slash2 - lastSlash2)) offset++; //if matching parts of address are the same, add offset
            else {
                end = lastSlash2;
                countOffset = false;
            }
        }
        lastSlash1 = slash1;
        lastSlash2 = slash2;
    }
    if (countOffset) end = lastSlash2; //for addresses like "base.com/test/index.html" and "base.com/test/kuba.html"

    string result;
    offset = depth - offset;
    for (int i = 0; i < offset; i++) {
        result += "../";
    }
    if (end != 0) end++;
    result += toConvert.substr(end, string::npos);
    size_t qm = 0;
    if ((qm = result.find("?")) != string::npos) { //replace ? with QM
        result.insert(qm, "QM");
        result.erase(qm + 2, 1);
    }
    //  cout << " -> " << result << endl;
    return result;
}


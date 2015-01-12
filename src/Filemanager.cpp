#include <fstream>

#include "Filemanager.h"
#include "Structures.h"
using namespace std;

Filemanager::Filemanager(void) {
}

Filemanager::Filemanager(const string & tRootPath, bool tVerbose)
: rootPath(tRootPath), verbose(tVerbose) {
}

string Filemanager::createPage(const std::string & host, std::string path, const std::string & content) const {
    // cout << "-----" << path << "------" << endl;
    size_t qm = 0;
    if ((qm = path.find("?")) != string::npos) { //replace ? with QM
        path.insert(qm, "QM");
        path.erase(qm + 2, 1);
    }

    string fullPath = rootPath + host + path;
    if (fullPath.back() == '/') {
        //  cout << "******************" << endl;
        fullPath += "auto_index.html";
    }


    if (fileExists(fullPath)) {
        return fullPath.substr(rootPath.length(), string::npos); //if file already exists skip the following steps
    }

    makeDirectory(fullPath);

    ofstream file;
    file.open(fullPath, ios::trunc | ios::binary);

    if (!file.is_open()) throw UnableToAccessFileException(string("File \"" + fullPath + "\" could not be created.").c_str());
    file.write(content.c_str(), content.length());
    if (file.bad()) throw UnableToAccessFileException(string("Error during writing to the file \"" + fullPath + "\".").c_str());
    file.close();
    //fullPath.erase(0,rootPath.length());
    //cout << "Path: " << fullPath.substr(rootPath.length(), string::npos) << endl;
    return fullPath.substr(rootPath.length(), string::npos);
}

std::string Filemanager::createRawPage(const std::string & host, std::string path, const std::vector<char> & content) const {
    //cout << "-----" << path << "------" << endl;
    size_t qm = 0;
    if ((qm = path.find("?")) != string::npos) { //replace ? with QM
        path.insert(qm, "QM");
        path.erase(qm + 2, 1);
    }
    
    string fullPath = rootPath + host + path;
    if (fullPath.back() == '/') {
        //  cout << "******************" << endl;
        fullPath += "auto_index";
    }

    if (fileExists(fullPath)) {
        return fullPath.substr(rootPath.length(), string::npos); //if file already exists skip the following steps
    }

    makeDirectory(fullPath);


    ofstream file;
    file.open(fullPath, ios::trunc | ios::binary);
    if (!file.is_open()) throw UnableToAccessFileException(string("File \"" + fullPath + "\" could not be created.").c_str());

    char buff[1];
    for (vector<char>::const_iterator it = content.cbegin(); it != content.cend(); it++) {
        buff [0] = *it;
        file.write(buff, 1);
    }

    if (file.bad()) throw UnableToAccessFileException(string("Error during writing to the file \"" + fullPath + "\".").c_str());
    file.close();
    // fullPath.erase(0,rootPath.length());
    // cout << "Path (raw): " << fullPath.substr(rootPath.length(), string::npos) << endl;
    return fullPath.substr(rootPath.length(), string::npos);
}

/*********PRIVATES**********/
void Filemanager::makeDirectory(const string & out) const {
    size_t end = 0;
    try {
        if (out.substr() == "") return;
    } catch (...) {
    }
    while ((end = out.find("/", end)) != string::npos) { // equivalent to "system(mkdir -p path/to/create)"
        if (mkdir(out.substr(0, end).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) { //err is signalized by -1
            int err = errno;
            if (err != EEXIST) {
                throw UnableToMakeDirectoryException(string("Error while making directory \"" + out.substr(0, end) + "\", returned error: " + strerror(err)).c_str());
            }
        }
        end++;
    }

    if (verbose) cout << "Directory \"" << out << "\" has been created." << endl;

}
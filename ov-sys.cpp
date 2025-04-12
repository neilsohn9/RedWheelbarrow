#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <algorithm>
#include <openssl/aes.h> // Requires OpenSSL library for encryption

using namespace std;

// Simulated email verification token (in real system, this would be sent via email)
string generateVerificationToken() {
    return "TOKEN" + to_string(rand() % 10000);
}

// Encryption Service using AES (simplified with OpenSSL)
class EncryptionService {
private:
    unsigned char key[AES_BLOCK_SIZE] = "ThisIsASecretKey"; // 16 bytes
    unsigned char iv[AES_BLOCK_SIZE] = "ThisIsAnIV123456";  // 16 bytes

public:
    string encrypt(int value) {
        AES_KEY enc_key;
        AES_set_encrypt_key(key, 128, &enc_key);
        string input = to_string(value);
        unsigned char out[AES_BLOCK_SIZE];
        AES_encrypt((unsigned char*)input.c_str(), out, &enc_key);
        return string((char*)out, AES_BLOCK_SIZE);
    }

    int decrypt(const string& encrypted) {
        AES_KEY dec_key;
        AES_set_decrypt_key(key, 128, &dec_key);
        unsigned char out[AES_BLOCK_SIZE];
        AES_decrypt((unsigned char*)encrypted.c_str(), out, &dec_key);
        return stoi(string((char*)out));
    }
};

// User class
class User {
public:
    string username;
    string password;
    string fullName;
    bool isEmailVerified;
    bool isAdmin;

    User(string u, string p, string f, bool v = false, bool a = false)
        : username(u), password(p), fullName(f), isEmailVerified(v), isAdmin(a) {}
};

// Candidate class
class Candidate {
public:
    int id;
    string name;
    string description;
    int voteCount;

    Candidate(int i, string n, string d) : id(i), name(n), description(d), voteCount(0) {}
};

// Vote class
class Vote {
public:
    string userId;
    string encryptedCandidateId;
    time_t voteDate;

    Vote(string u, string e) : userId(u), encryptedCandidateId(e), voteDate(time(nullptr)) {}
};

// AuditLog class
class AuditLog {
public:
    string userId;
    string action;
    time_t timestamp;
    string details;

    AuditLog(string u, string a, string d) : userId(u), action(a), details(d), timestamp(time(nullptr)) {}
};

// Voting System class
class VotingSystem {
private:
    vector<User> users;
    vector<Candidate> candidates;
    vector<Vote> votes;
    vector<AuditLog> auditLogs;
    EncryptionService encryption;
    map<string, chrono::system_clock::time_point> rateLimitTracker;
    const int MAX_REQUESTS = 10;
    const chrono::minutes RATE_LIMIT_WINDOW = chrono::minutes(1);

    bool isPasswordComplex(const string& password) {
        bool hasDigit = false, hasUpper = false, hasSpecial = false;
        if (password.length() < 8) return false;
        for (char c : password) {
            if (isdigit(c)) hasDigit = true;
            else if (isupper(c)) hasUpper = true;
            else if (!isalnum(c)) hasSpecial = true;
        }
        return hasDigit && hasUpper && hasSpecial;
    }

    void logAudit(const string& userId, const string& action, const string& details) {
        auditLogs.emplace_back(userId, action, details);
        saveAuditLogs();
    }

    bool checkRateLimit(const string& userId) {
        auto now = chrono::system_clock::now();
        if (rateLimitTracker.find(userId) == rateLimitTracker.end()) {
            rateLimitTracker[userId] = now;
            return true;
        }
        auto lastTime = rateLimitTracker[userId];
        if (chrono::duration_cast<chrono::minutes>(now - lastTime) > RATE_LIMIT_WINDOW) {
            rateLimitTracker[userId] = now;
            return true;
        }
        return false; // Rate limit exceeded
    }

    void loadData() {
        ifstream usersFile("users.txt");
        if (usersFile.is_open()) {
            string u, p, f; bool v, a;
            while (usersFile >> u >> p >> f >> v >> a) users.emplace_back(u, p, f, v, a);
            usersFile.close();
        }

        ifstream candidatesFile("candidates.txt");
        if (candidatesFile.is_open()) {
            int i; string n, d;
            while (candidatesFile >> i >> n >> d) candidates.emplace_back(i, n, d);
            candidatesFile.close();
        }

        ifstream votesFile("votes.txt");
        if (votesFile.is_open()) {
            string u, e;
            while (votesFile >> u >> e) votes.emplace_back(u, e);
            votesFile.close();
        }

        ifstream auditFile("audit.txt");
        if (auditFile.is_open()) {
            string u, a, d; time_t t;
            while (auditFile >> u >> a >> t >> d) auditLogs.emplace_back(u, a, d);
            auditFile.close();
        }
    }

    void saveData() {
        ofstream usersFile("users.txt");
        for (const auto& u : users)
            usersFile << u.username << " " << u.password << " " << u.fullName << " " << u.isEmailVerified << " " << u.isAdmin << endl;
        usersFile.close();

        ofstream candidatesFile("candidates.txt");
        for (const auto& c : candidates)
            candidatesFile << c.id << " " << c.name << " " << c.description << endl;
        candidatesFile.close();

        ofstream votesFile("votes.txt");
        for (const auto& v : votes)
            votesFile << v.userId << " " << v.encryptedCandidateId << endl;
        votesFile.close();
    }

    void saveAuditLogs() {
        ofstream auditFile("audit.txt");
        for (const auto& l : auditLogs)
            auditFile << l.userId << " " << l.action << " " << l.timestamp << " " << l.details << endl;
        auditFile.close();
    }

public:
    VotingSystem() { loadData(); }

    void registerUser() {
        string username, password, fullName;
        cout << "Enter username (email): "; cin >> username;
        cout << "Enter password (8+ chars, 1 digit, 1 upper, 1 special): "; cin >> password;
        cout << "Enter full name: "; cin.ignore(); getline(cin, fullName);

        if (!isPasswordComplex(password)) {
            cout << "Password does not meet complexity requirements!" << endl;
            return;
        }

        if (find_if(users.begin(), users.end(), [&](const User& u) { return u.username == username; }) != users.end()) {
            cout << "User already exists!" << endl;
            return;
        }

        users.emplace_back(username, password, fullName);
        string token = generateVerificationToken();
        cout << "Verification token sent to " << username << ": " << token << endl;
        cout << "Enter token to verify: ";
        string inputToken; cin >> inputToken;
        if (inputToken == token) {
            users.back().isEmailVerified = true;
            saveData();
            logAudit(username, "Register", "User registered and verified");
            cout << "Registration successful!" << endl;
        } else {
            users.pop_back();
            cout << "Verification failed!" << endl;
        }
    }

    User* login() {
        string username, password;
        cout << "Enter username: "; cin >> username;
        cout << "Enter password: "; cin >> password;

        if (!checkRateLimit(username)) {
            cout << "Rate limit exceeded. Try again later." << endl;
            return nullptr;
        }

        auto it = find_if(users.begin(), users.end(), [&](const User& u) {
            return u.username == username && u.password == password && u.isEmailVerified;
        });
        if (it != users.end()) {
            logAudit(username, "Login", "User logged in");
            return &(*it);
        }
        cout << "Invalid credentials or email not verified!" << endl;
        return nullptr;
    }

    void vote(User* user) {
        if (!user) throw runtime_error("User not logged in!");

        if (find_if(votes.begin(), votes.end(), [&](const Vote& v) { return v.userId == user->username; }) != votes.end()) {
            cout << "You have already voted!" << endl;
            return;
        }

        cout << "Candidates:\n";
        for (const auto& c : candidates) cout << c.id << ": " << c.name << " - " << c.description << endl;
        cout << "Enter candidate ID: ";
        int candidateId; cin >> candidateId;

        auto it = find_if(candidates.begin(), candidates.end(), [&](const Candidate& c) { return c.id == candidateId; });
        if (it == candidates.end()) {
            cout << "Invalid candidate ID!" << endl;
            return;
        }

        // Simulate CSRF token check
        string csrfToken = "CSRF" + to_string(rand() % 1000);
        cout << "Enter CSRF token (" << csrfToken << "): ";
        string inputCsrf; cin >> inputCsrf;
        if (inputCsrf != csrfToken) {
            cout << "CSRF verification failed!" << endl;
            return;
        }

        votes.emplace_back(user->username, encryption.encrypt(candidateId));
        it->voteCount++;
        saveData();
        logAudit(user->username, "Vote", "Voted for candidate " + to_string(candidateId));
        cout << "Vote recorded successfully!" << endl;
    }

    void showResults() {
        cout << "Voting Results:\n";
        sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) { return a.voteCount > b.voteCount; });
        for (const auto& c : candidates)
            cout << c.name << " - " << c.description << ": " << c.voteCount << " votes" << endl;
    }

    void adminInterface(User* user) {
        if (!user || !user->isAdmin) throw runtime_error("Access denied!");

        while (true) {
            cout << "\nAdmin Menu:\n1. Add Candidate\n2. View Audit Logs\n3. Exit\nChoice: ";
            int choice; cin >> choice;
            try {
                switch (choice) {
                case 1: {
                    int id = candidates.empty() ? 1 : candidates.back().id + 1;
                    string name, desc;
                    cout << "Enter candidate name: "; cin.ignore(); getline(cin, name);
                    cout << "Enter description: "; getline(cin, desc);
                    candidates.emplace_back(id, name, desc);
                    saveData();
                    logAudit(user->username, "AddCandidate", "Added candidate " + name);
                    cout << "Candidate added!" << endl;
                    break;
                }
                case 2: {
                    cout << "Audit Logs:\n";
                    for (const auto& log : auditLogs) {
                        cout << "User: " << log.userId << ", Action: " << log.action
                             << ", Time: " << ctime(&log.timestamp) << "Details: " << log.details << endl;
                    }
                    break;
                }
                case 3: return;
                default: cout << "Invalid choice!" << endl;
                }
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
    }
};

// Main function
int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    VotingSystem system;
    User* currentUser = nullptr;

    while (true) {
        try {
            cout << "\n1. Register\n2. Login\n3. Vote\n4. View Results\n5. Admin Interface\n6. Exit\nChoice: ";
            int choice; cin >> choice;

            switch (choice) {
            case 1: system.registerUser(); break;
            case 2: currentUser = system.login(); break;
            case 3: system.vote(currentUser); break;
            case 4: system.showResults(); break;
            case 5: system.adminInterface(currentUser); break;
            case 6: cout << "Exiting..." << endl; return 0;
            default: cout << "Invalid choice!" << endl;
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    return 0;
}
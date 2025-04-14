#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Constants
#define MAX_USERS 100
#define MAX_CANDIDATES 50
#define MAX_VOTES 100
#define MAX_AUDIT_LOGS 200
#define MAX_STRING 100
#define RATE_LIMIT_WINDOW 60 // 1 minute in seconds
#define MAX_REQUESTS 10

// Structs
typedef struct {
    char username[MAX_STRING];
    char password[MAX_STRING];
    char fullName[MAX_STRING];
    int isEmailVerified;
    int isAdmin;
} User;

typedef struct {
    int id;
    char name[MAX_STRING];
    char description[MAX_STRING];
    int voteCount;
} Candidate;

typedef struct {
    char userId[MAX_STRING];
    char encryptedCandidateId[MAX_STRING];
    time_t voteDate;
} Vote;

typedef struct {
    char userId[MAX_STRING];
    char action[MAX_STRING];
    time_t timestamp;
    char details[MAX_STRING];
} AuditLog;

typedef struct {
    char userId[MAX_STRING];
    time_t lastRequest;
} RateLimit;

// Global data
User users[MAX_USERS];
Candidate candidates[MAX_CANDIDATES];
Vote votes[MAX_VOTES];
AuditLog auditLogs[MAX_AUDIT_LOGS];
RateLimit rateLimits[MAX_USERS];
int userCount = 0, candidateCount = 0, voteCount = 0, auditCount = 0, rateLimitCount = 0;

// Utility Functions
void generateVerificationToken(char* token) {
    sprintf(token, "TOKEN%d", rand() % 10000);
}

void encryptVote(int candidateId, char* output) {
    char key[] = "SimpleKey123"; // Simple XOR key
    sprintf(output, "%d", candidateId);
    for (int i = 0; output[i]; i++) {
        output[i] ^= key[i % strlen(key)];
    }
}

int decryptVote(const char* encrypted) {
    char key[] = "SimpleKey123";
    char temp[MAX_STRING];
    strcpy(temp, encrypted);
    for (int i = 0; temp[i]; i++) {
        temp[i] ^= key[i % strlen(key)];
    }
    return atoi(temp);
}

int isPasswordComplex(const char* password) {
    int hasDigit = 0, hasUpper = 0, hasSpecial = 0;
    if (strlen(password) < 8) return 0;
    for (int i = 0; password[i]; i++) {
        if (isdigit(password[i])) hasDigit = 1;
        else if (isupper(password[i])) hasUpper = 1;
        else if (!isalnum(password[i])) hasSpecial = 1;
    }
    return hasDigit && hasUpper && hasSpecial;
}

void logAudit(const char* userId, const char* action, const char* details) {
    if (auditCount >= MAX_AUDIT_LOGS) {
        printf("Audit log limit reached!\n");
        return;
    }
    strcpy(auditLogs[auditCount].userId, userId);
    strcpy(auditLogs[auditCount].action, action);
    auditLogs[auditCount].timestamp = time(NULL);
    strcpy(auditLogs[auditCount].details, details);
    auditCount++;

    FILE* file = fopen("audit.txt", "a");
    if (file) {
        fprintf(file, "%s %s %ld %s\n", userId, action, auditLogs[auditCount - 1].timestamp, details);
        fclose(file);
    } else {
        printf("Error writing to audit log file!\n");
    }
}

int checkRateLimit(const char* userId) {
    time_t now = time(NULL);
    for (int i = 0; i < rateLimitCount; i++) {
        if (strcmp(rateLimits[i].userId, userId) == 0) {
            if (now - rateLimits[i].lastRequest > RATE_LIMIT_WINDOW) {
                rateLimits[i].lastRequest = now;
                return 1;
            }
            return 0; // Rate limit exceeded
        }
    }
    if (rateLimitCount < MAX_USERS) {
        strcpy(rateLimits[rateLimitCount].userId, userId);
        rateLimits[rateLimitCount].lastRequest = now;
        rateLimitCount++;
        return 1;
    }
    return 0;
}

void loadData() {
    FILE* file = fopen("users.txt", "r");
    if (file) {
        while (fscanf(file, "%s %s %s %d %d", users[userCount].username, users[userCount].password,
                      users[userCount].fullName, &users[userCount].isEmailVerified, &users[userCount].isAdmin) == 5)
            userCount++;
        fclose(file);
    }

    file = fopen("candidates.txt", "r");
    if (file) {
        while (fscanf(file, "%d %s %s", &candidates[candidateCount].id, candidates[candidateCount].name,
                      candidates[candidateCount].description) == 3)
            candidateCount++;
        fclose(file);
    }

    file = fopen("votes.txt", "r");
    if (file) {
        while (fscanf(file, "%s %s", votes[voteCount].userId, votes[voteCount].encryptedCandidateId) == 2)
            voteCount++;
        fclose(file);
    }

    file = fopen("audit.txt", "r");
    if (file) {
        while (fscanf(file, "%s %s %ld %s", auditLogs[auditCount].userId, auditLogs[auditCount].action,
                      &auditLogs[auditCount].timestamp, auditLogs[auditCount].details) == 4)
            auditCount++;
        fclose(file);
    }
}

void saveData() {
    FILE* file = fopen("users.txt", "w");
    if (file) {
        for (int i = 0; i < userCount; i++)
            fprintf(file, "%s %s %s %d %d\n", users[i].username, users[i].password, users[i].fullName,
                    users[i].isEmailVerified, users[i].isAdmin);
        fclose(file);
    } else {
        printf("Error writing to users file!\n");
    }

    file = fopen("candidates.txt", "w");
    if (file) {
        for (int i = 0; i < candidateCount; i++)
            fprintf(file, "%d %s %s\n", candidates[i].id, candidates[i].name, candidates[i].description);
        fclose(file);
    } else {
        printf("Error writing to candidates file!\n");
    }

    file = fopen("votes.txt", "w");
    if (file) {
        for (int i = 0; i < voteCount; i++)
            fprintf(file, "%s %s\n", votes[i].userId, votes[i].encryptedCandidateId);
        fclose(file);
    } else {
        printf("Error writing to votes file!\n");
    }
}

// Voting System Functions
void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("User limit reached!\n");
        return;
    }

    User newUser;
    printf("Enter username (email): "); scanf("%s", newUser.username);
    printf("Enter password (8+ chars, 1 digit, 1 upper, 1 special): "); scanf("%s", newUser.password);
    printf("Enter full name: "); getchar(); fgets(newUser.fullName, MAX_STRING, stdin);
    newUser.fullName[strcspn(newUser.fullName, "\n")] = 0;

    if (!isPasswordComplex(newUser.password)) {
        printf("Password does not meet complexity requirements!\n");
        return;
    }

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, newUser.username) == 0) {
            printf("User already exists!\n");
            return;
        }
    }

    newUser.isEmailVerified = 0;
    newUser.isAdmin = 0;
    users[userCount] = newUser;
    char token[MAX_STRING];
    generateVerificationToken(token);
    printf("Verification token sent to %s: %s\nEnter token: ", newUser.username, token);
    char inputToken[MAX_STRING];
    scanf("%s", inputToken);
    if (strcmp(inputToken, token) == 0) {
        users[userCount].isEmailVerified = 1;
        userCount++;
        saveData();
        logAudit(newUser.username, "Register", "User registered and verified");
        printf("Registration successful!\n");
    } else {
        printf("Verification failed!\n");
    }
}

User* login() {
    char username[MAX_STRING], password[MAX_STRING];
    printf("Enter username: "); scanf("%s", username);
    printf("Enter password: "); scanf("%s", password);

    if (!checkRateLimit(username)) {
        printf("Rate limit exceeded. Try again later.\n");
        return NULL;
    }

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0 && users[i].isEmailVerified) {
            logAudit(username, "Login", "User logged in");
            return &users[i];
        }
    }
    printf("Invalid credentials or email not verified!\n");
    return NULL;
}

void vote(User* user) {
    if (!user) {
        printf("Error: User not logged in!\n");
        return;
    }

    for (int i = 0; i < voteCount; i++) {
        if (strcmp(votes[i].userId, user->username) == 0) {
            printf("You have already voted!\n");
            return;
        }
    }

    printf("Candidates:\n");
    for (int i = 0; i < candidateCount; i++)
        printf("%d: %s - %s\n", candidates[i].id, candidates[i].name, candidates[i].description);
    printf("Enter candidate ID: ");
    int candidateId;
    if (scanf("%d", &candidateId) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }

    int valid = 0;
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i].id == candidateId) {
            valid = 1;
            break;
        }
    }
    if (!valid) {
        printf("Invalid candidate ID!\n");
        return;
    }

    char csrfToken[MAX_STRING];
    sprintf(csrfToken, "CSRF%d", rand() % 1000);
    printf("Enter CSRF token (%s): ", csrfToken);
    char inputCsrf[MAX_STRING];
    scanf("%s", inputCsrf);
    if (strcmp(inputCsrf, csrfToken) != 0) {
        printf("CSRF verification failed!\n");
        return;
    }

    if (voteCount < MAX_VOTES) {
        strcpy(votes[voteCount].userId, user->username);
        encryptVote(candidateId, votes[voteCount].encryptedCandidateId);
        votes[voteCount].voteDate = time(NULL);
        voteCount++;
        for (int i = 0; i < candidateCount; i++) {
            if (candidates[i].id == candidateId) candidates[i].voteCount++;
        }
        saveData();
        char details[MAX_STRING];
        sprintf(details, "Voted for candidate %d", candidateId);
        logAudit(user->username, "Vote", details);
        printf("Vote recorded successfully!\n");
    } else {
        printf("Vote limit reached!\n");
    }
}

void showResults() {
    printf("Voting Results:\n");
    // Bubble sort by voteCount (descending)
    for (int i = 0; i < candidateCount - 1; i++) {
        for (int j = 0; j < candidateCount - i - 1; j++) {
            if (candidates[j].voteCount < candidates[j + 1].voteCount) {
                Candidate temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < candidateCount; i++)
        printf("%s - %s: %d votes\n", candidates[i].name, candidates[i].description, candidates[i].voteCount);
}

void adminInterface(User* user) {
    if (!user || !user->isAdmin) {
        printf("Error: Access denied!\n");
        return;
    }

    while (1) {
        printf("\nAdmin Menu:\n1. Add Candidate\n2. View Audit Logs\n3. Exit\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
        case 1:
            if (candidateCount < MAX_CANDIDATES) {
                candidates[candidateCount].id = candidateCount + 1;
                printf("Enter candidate name: "); getchar(); fgets(candidates[candidateCount].name, MAX_STRING, stdin);
                candidates[candidateCount].name[strcspn(candidates[candidateCount].name, "\n")] = 0;
                printf("Enter description: "); fgets(candidates[candidateCount].description, MAX_STRING, stdin);
                candidates[candidateCount].description[strcspn(candidates[candidateCount].description, "\n")] = 0;
                candidates[candidateCount].voteCount = 0;
                candidateCount++;
                saveData();
                logAudit(user->username, "AddCandidate", "Added candidate");
                printf("Candidate added!\n");
            } else {
                printf("Candidate limit reached!\n");
            }
            break;
        case 2:
            printf("Audit Logs:\n");
            for (int i = 0; i < auditCount; i++) {
                char* timeStr = ctime(&auditLogs[i].timestamp);
                timeStr[strlen(timeStr) - 1] = 0; // Remove newline
                printf("User: %s, Action: %s, Time: %s, Details: %s\n",
                       auditLogs[i].userId, auditLogs[i].action, timeStr, auditLogs[i].details);
            }
            break;
        case 3:
            return;
        default:
            printf("Invalid choice!\n");
        }
    }
}

// Main function
int main() {
    srand((unsigned)time(NULL));
    loadData();

    // Seed initial data if empty
    if (userCount == 0) {
        strcpy(users[0].username, "admin@example.com");
        strcpy(users[0].password, "Admin@123");
        strcpy(users[0].fullName, "Admin User");
        users[0].isEmailVerified = 1;
        users[0].isAdmin = 1;
        userCount++;
        saveData();
    }
    if (candidateCount == 0) {
        candidates[0].id = 1;
        strcpy(candidates[0].name, "John Doe");
        strcpy(candidates[0].description, "Candidate 1");
        candidates[0].voteCount = 0;
        candidates[1].id = 2;
        strcpy(candidates[1].name, "Jane Smith");
        strcpy(candidates[1].description, "Candidate 2");
        candidates[1].voteCount = 0;
        candidateCount = 2;
        saveData();
    }

    User* currentUser = NULL;
    while (1) {
        printf("\n1. Register\n2. Login\n3. Vote\n4. View Results\n5. Admin Interface\n6. Exit\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
        case 1:
            registerUser();
            break;
        case 2:
            currentUser = login();
            break;
        case 3:
            vote(currentUser);
            break;
        case 4:
            showResults();
            break;
        case 5:
            adminInterface(currentUser);
            break;
        case 6:
            printf("Exiting...\n");
            return 0;
        default:
            printf("Invalid choice!\n");
        }
    }
    return 0;
}
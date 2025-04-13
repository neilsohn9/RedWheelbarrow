#include <iostream>
#include <string>
using namespace std;

// Structure to store user data
struct UserData {
    string name;
    string email;
    bool consent;
};

// Function declarations
void collectData(UserData database[], int &count);
void encryptData(UserData database[], int count);
void displayData(const UserData database[], int count);
void deleteData(int &count);

// Function to collect user data with consent
void collectData(UserData database[], int &count) {
    if (count >= 100) {
        cout << "Database is full. Cannot add more users." << endl;
        return;
    }

    UserData user;
    char consentInput;

    cout << "\nEnter user name: ";
    cin.ignore(); // Clear the input buffer
    getline(cin, user.name);

    cout << "Enter user email: ";
    getline(cin, user.email);

    cout << "Do you give consent to store your data? (y/n): ";
    cin >> consentInput;

    if (tolower(consentInput) == 'y') { // Handle both 'y' and 'Y'
        user.consent = true;
        database[count] = user; // Add user to the database
        count++; // Increment the user count
        cout << "Data collected successfully with consent." << endl;
    } else {
        cout << "Consent not given. Data not collected." << endl;
    }
}

// Function to simulate data encryption
void encryptData(UserData database[], int count) {
    if (count == 0) {
        cout << "\nNo data to encrypt!" << endl;
        return;
    }

    cout << "\nEncrypting user data..." << endl;
    for (int i = 0; i < count; i++) {
        if (database[i].consent) { // Only encrypt data if consent is given
            database[i].name = "ENCRYPTED_" + database[i].name; // Simulate encryption
            database[i].email = "ENCRYPTED_" + database[i].email; // Simulate encryption
        }
    }

    cout << "All data encrypted successfully." << endl;
}

// Function to display data ethically (only if consent is true)
void displayData(const UserData database[], int count) {
    if (count == 0) {
        cout << "\nNo data to display!" << endl;
        return;
    }

    cout << "\nDisplaying User Data:" << endl;
    for (int i = 0; i < count; i++) {
        if (database[i].consent) {
            cout << "Name: " << database[i].name << ", Email: " << database[i].email << endl;
        } else {
            cout << "User did not give consent. Data is not displayed." << endl;
        }
    }
}

// Function to delete all user data (data minimization and privacy)
void deleteData(int &count) {
    count = 0; // Reset the user count to 0
    cout << "\nAll user data deleted to ensure privacy." << endl;
}


int main() {
    const int MAX_USERS = 100; // Maximum number of users
    UserData database[MAX_USERS]; // Array to store user data
    int count = 0; // Number of users in the database
    int choice;

    do {
        cout << "\n==== AI Ethical Data Management System ====" << endl;
        cout << "1. Collect User Data" << endl;
        cout << "2. Encrypt Data" << endl;
        cout << "3. Display Data" << endl;
        cout << "4. Delete All Data" << endl;
        cout << "5. Exit" << endl;
        cout << "Choose an option (1-5): ";
        cin >> choice;

        switch (choice) {
            case 1:
                collectData(database, count);
                break;
            case 2:
                encryptData(database, count);
                break;
            case 3:
                displayData(database, count);
                break;
            case 4:
                deleteData(count);
                break;
            case 5:
                cout << "Exiting system. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 5);

    return 0;
}


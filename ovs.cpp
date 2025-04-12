#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

class Voter {
private:
    std::string id;
    std::string name;
    std::string password;
    bool hasVoted;

public:
    Voter(const std::string& voterId, const std::string& voterName, const std::string& pwd)
        : id(voterId), name(voterName), password(pwd), hasVoted(false) {}

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    bool checkPassword(const std::string& pwd) const { return password == pwd; }
    bool hasAlreadyVoted() const { return hasVoted; }
    void markAsVoted() { hasVoted = true; }
};

class Ballot {
private:
    std::map<std::string, int> candidates;
    std::vector<std::string> candidateList;

public:
    void addCandidate(const std::string& candidate) {
        candidates[candidate] = 0;
        candidateList.push_back(candidate);
    }

    void castVote(const std::string& candidate) {
        if(candidates.find(candidate) != candidates.end()) {
            candidates[candidate]++;
        }
    }

    void displayResults() const {
        std::cout << "\nElection Results:\n";
        std::cout << "-----------------\n";
        for(const auto& [candidate, votes] : candidates) {
            std::cout << candidate << ": " << votes << " votes\n";
        }
    }

    const std::vector<std::string>& getCandidates() const { return candidateList; }
};

class VotingSystem {
private:
    std::vector<Voter> voters;
    Ballot ballot;
    const std::string adminPassword = "admin123";

    Voter* findVoter(const std::string& voterId) {
        auto it = std::find_if(voters.begin(), voters.end(),
            [&voterId](const Voter& v) { return v.getId() == voterId; });
        return it != voters.end() ? &(*it) : nullptr;
    }

public:
    void initializeSampleData() {
        voters.emplace_back("V123", "Alice Smith", "alice2023");
        voters.emplace_back("V124", "Bob Johnson", "bob@secure");
        voters.emplace_back("V127", "vincent obare", "vin123");
        voters.emplace_back("V125", "Carol Williams", "carol#safe");

        ballot.addCandidate("RUTO");
        ballot.addCandidate("WAJAKOYA");
        ballot.addCandidate("RAILA ");
    }

    void voterLogin() {
        std::string voterId, password;
        std::cout << "\nVoter Login\n";
        std::cout << "Enter Voter ID: ";
        std::cin >> voterId;
        std::cout << "Enter Password: ";
        std::cin >> password;

        Voter* voter = findVoter(voterId);
        if(voter && voter->checkPassword(password)) {
            if(voter->hasAlreadyVoted()) {
                std::cout << "Error: You have already voted!\n";
            } else {
                displayBallot(*voter);
            }
        } else {
            std::cout << "Invalid credentials!\n";
        }
    }

    void displayBallot(Voter& voter) {
        std::cout << "\nAvailable Candidates:\n";
        const auto& candidates = ballot.getCandidates();
        for(size_t i = 0; i < candidates.size(); ++i) {
            std::cout << i+1 << ". " << candidates[i] << "\n";
        }

        int choice;
        std::cout << "Enter your choice (1-" << candidates.size() << "): ";
        while(!(std::cin >> choice) || choice < 1 || choice > static_cast<int>(candidates.size())) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a valid choice: ";
        }

        ballot.castVote(candidates[choice-1]);
        voter.markAsVoted();
        std::cout << "Vote recorded successfully!\n";
    }

    void adminPanel() {
        std::string password;
        std::cout << "\nAdmin Login\n";
        std::cout << "Enter Admin Password: ";
        std::cin >> password;

        if(password != adminPassword) {
            std::cout << "Unauthorized access!\n";
            return;
        }

        int choice;
        do {
            std::cout << "\nAdmin Menu\n";
            std::cout << "1. Verify Voter\n";
            std::cout << "2. View Results\n";
            std::cout << "3. Return to Main Menu\n";
            std::cout << "Enter choice: ";
            std::cin >> choice;

            switch(choice) {
                case 1: {
                    std::string voterId;
                    std::cout << "Enter Voter ID to verify: ";
                    std::cin >> voterId;
                    Voter* voter = findVoter(voterId);
                    if(voter) {
                        std::cout << "Voter: " << voter->getName() << "\n";
                        std::cout << "Voting Status: "
                                  << (voter->hasAlreadyVoted() ? "Voted" : "Not Voted") << "\n";
                    } else {
                        std::cout << "Voter not found!\n";
                    }
                    break;
                }
                case 2:
                    ballot.displayResults();
                    break;
                case 3:
                    return;
                default:
                    std::cout << "Invalid choice!\n";
            }
        } while(choice != 3);
    }

    void run() {
        initializeSampleData();
        int choice;

        do {
            std::cout << "\nOnline Voting System\n";
            std::cout << "1. Voter Login\n";
            std::cout << "2. Admin Login\n";
            std::cout << "3. Exit\n";
            std::cout << "Enter choice: ";
            std::cin >> choice;

            switch(choice) {
                case 1:
                    voterLogin();
                    break;
                case 2:
                    adminPanel();
                    break;
                case 3:
                    std::cout << "Exiting system...\n";
                    break;
                default:
                    std::cout << "Invalid choice!\n";
            }
        } while(choice != 3);
    }
};

int main() {
    VotingSystem votingSystem;
    votingSystem.run();
    return 0;
}
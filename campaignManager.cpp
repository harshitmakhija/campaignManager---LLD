#include <bits/stdc++.h>
using namespace std;

// Base Campaign Class
class Campaign {
protected:
    string campaignId;
    string name;
    string description;

public:
    Campaign(string id = "", string name = "", string desc = "")
        : campaignId(id), name(name), description(desc) {}

    string getId() { return campaignId; }
    string getName() { return name; }
    string getDescription() { return description; }

    virtual ~Campaign() {} 
};

// Communication Class
class Communication {
protected:
    string communicationId;
    string channel;
    string subject;  
    string message;

public:
    Communication(string id, string ch, string subj, string msg)
        : communicationId(id), channel(ch), subject(subj), message(msg) {}

    string getCommunicationId() { return communicationId; }
    string getChannel() { return channel; }
    string getSubject() { return subject; }
    string getMessage() { return message; }
};

// Communication Campaign Class derived from Campaign
class CommunicationCampaign : public Campaign {
private:
    vector<Communication> communications;

public:
    CommunicationCampaign(string id = "", string name = "", string desc = "")
        : Campaign(id, name, desc) {}

    bool addCommunication(Communication communication) {
        for (int i = 0; i < communications.size(); ++i) {
            if (communications[i].getCommunicationId() == communication.getCommunicationId()) {
                return false; // Duplicate communication ID found
            }
        }
        communications.push_back(communication);
        return true;
    }

    vector<Communication> evaluateCommunicationOrder() {
        return communications; 
    }
};

// Campaign Manager Class
class CampaignManager {
private:
    map<string, Campaign*> campaigns;

public:
    enum class ErrorCode {
        SUCCESS,
        CAMPAIGN_ID_ALREADY_EXISTS,
        CAMPAIGN_NOT_FOUND,
        DUPLICATE_COMMUNICATION_ID
    };

    pair<ErrorCode, string> createCampaign(string id, string name, string description) {
        if (campaigns.find(id) != campaigns.end()) {
            return { ErrorCode::CAMPAIGN_ID_ALREADY_EXISTS, "" }; // Campaign ID already exists
        }

        campaigns[id] = new CommunicationCampaign(id, name, description);
        return { ErrorCode::SUCCESS, id };
    }

    ErrorCode addCommunication(string campaignId, Communication communication) {
        auto it = campaigns.find(campaignId);
        if (it != campaigns.end()) {
            CommunicationCampaign* commCampaign = dynamic_cast<CommunicationCampaign*>(it->second);
            if (commCampaign) {
                if (!commCampaign->addCommunication(communication)) {
                    return ErrorCode::DUPLICATE_COMMUNICATION_ID; // Duplicate communication ID
                }
                return ErrorCode::SUCCESS;
            } else {
                return ErrorCode::CAMPAIGN_NOT_FOUND; // Campaign type mismatch
            }
        }
        return ErrorCode::CAMPAIGN_NOT_FOUND; // Campaign not found
    }

    pair<ErrorCode, Campaign*> getCampaign(string id) {
        auto it = campaigns.find(id);
        if (it != campaigns.end()) {
            return { ErrorCode::SUCCESS, it->second };
        }
        return { ErrorCode::CAMPAIGN_NOT_FOUND, nullptr }; // Campaign not found
    }

    ~CampaignManager() {
        for (auto& pair : campaigns) {
            delete pair.second;
        }
        campaigns.clear();
    }
};

void evaluateCommunicationOrder(Campaign* campaign) {
    CommunicationCampaign* commCampaign = dynamic_cast<CommunicationCampaign*>(campaign);
    if (commCampaign) {
        vector<Communication> communications = commCampaign->evaluateCommunicationOrder();

        cout << "[\n";
        for (int i = 0; i < communications.size(); ++i) {
            Communication comm = communications[i];
            cout << "  {\n";
            cout << "    \"rank\": " << (i + 1) << ",\n";
            cout << "    \"communicationId\": \"" << comm.getCommunicationId() << "\",\n";
            cout << "    \"channel\": \"" << comm.getChannel() << "\",\n";
            if (comm.getChannel() == "EMAIL") {
                cout << "    \"subject\": \"" << comm.getSubject() << "\",\n";
            }
            cout << "    \"message\": \"" << comm.getMessage() << "\"\n";
            cout << "  }";
            if (i != communications.size() - 1) {
                cout << ",";
            }
            cout << "\n";
        }
        cout << "]\n";
    } else {
        cout << "Failed to evaluate campaign: Campaign not found or is not a CommunicationCampaign.\n";
    }
}

void runCode(CampaignManager& manager) {
    while (true) {
        cout << "\nHey there, Im your campaign manager. Please select your choice\n";
        cout << "1. Create a Campaign\n";
        cout << "2. Add Communication to a Campaign\n";
        cout << "3. Evaluate Communication Order\n";
        cout << "4. Get Campaign Details\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                string id, name, description;

                cout << "Enter Campaign ID: ";
                cin >> id;
                if(cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid Campaign ID.\n";
                    break;
                }
                cout << "Enter Campaign Name: ";
                cin.ignore(); 
                getline(cin, name);
                cout << "Enter Campaign Description: ";
                getline(cin, description);

                auto createResult = manager.createCampaign(id, name, description);
                if (createResult.first == CampaignManager::ErrorCode::SUCCESS) {
                    cout << "Campaign created successfully. Campaign ID: " << createResult.second << "\n";
                } else {
                    cout << "Failed to create campaign: Campaign ID already exists.\n";
                }
                break;
            }
            case 2: {
                string campaignId;

                cout << "Enter Campaign ID to add communication: ";
                cin >> campaignId;
                if(cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid Campaign ID.\n";
                    break;
                }

                auto campaignResult = manager.getCampaign(campaignId);
                if (campaignResult.first == CampaignManager::ErrorCode::CAMPAIGN_NOT_FOUND) {
                    cout << "Failed to add communication: Campaign not found.\n";
                    break;
                }

                string communicationId, channel, subject, message;
                cout << "Enter Communication ID: ";
                cin >> communicationId;
                cout << "Enter Communication Channel (SMS or EMAIL): ";
                cin >> channel;

                if (channel == "EMAIL") {
                    cout << "Enter Email Subject: ";
                    cin.ignore(); 
                    getline(cin, subject);
                } else if (channel == "SMS") {
                    subject = "";
                    cin.ignore(); 
                } else {
                    cout << "Invalid channel. Only 'SMS' or 'EMAIL' are allowed.\n";
                    break;
                }

                cout << "Enter Communication Message: ";
                getline(cin, message);

                Communication newCommunication(communicationId, channel, subject, message);
                auto addResult = manager.addCommunication(campaignId, newCommunication);
                if (addResult == CampaignManager::ErrorCode::SUCCESS) {
                    cout << "Communication added to campaign.\n";
                } else if (addResult == CampaignManager::ErrorCode::DUPLICATE_COMMUNICATION_ID) {
                    cout << "Failed to add communication: Duplicate communication ID.\n";
                } else {
                    cout << "Failed to add communication: Campaign not found.\n";
                }
                break;
            }
            case 3: {
                string campaignId;

                cout << "Enter Campaign ID to evaluate communication order: ";
                cin >> campaignId;
                if(cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid Campaign ID.\n";
                    break;
                }

                auto campaignResult = manager.getCampaign(campaignId);
                if (campaignResult.first == CampaignManager::ErrorCode::SUCCESS) {
                    cout << "Execution order for campaign ID " << campaignId << ":\n";
                    evaluateCommunicationOrder(campaignResult.second);
                } else {
                    cout << "Failed to evaluate campaign: Campaign not found.\n";
                }
                break;
            }
            case 4: {
                string campaignId;

                cout << "Enter Campaign ID to get details: ";
                cin >> campaignId;
                if(cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid Campaign ID.\n";
                    break;
                }

                auto campaignResult = manager.getCampaign(campaignId);
                if (campaignResult.first == CampaignManager::ErrorCode::SUCCESS) {
                    Campaign* campaign = campaignResult.second;
                    cout << "Campaign ID: " << campaign->getId() << "\n";
                    cout << "Name: " << campaign->getName() << "\n";
                    cout << "Description: " << campaign->getDescription() << "\n";
                } else {
                    cout << "Failed to get campaign: Campaign not found.\n";
                }
                break;
            }
            case 5:
                return;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    CampaignManager manager;
    runCode(manager);
    return 0;
}

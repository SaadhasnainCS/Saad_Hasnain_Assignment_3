/*
Program Name: EECS 348 Assignment 3

Description:This is C++ program that uses listbased Maxheap to prioritize emails for a CEO based on sender category and date. 
            The priority order is Boss, Subordinate, Peer, ImportantPerson, and OtherPerson.
            If two emails are from same sender, the latest email has the higher priority.

Inputs: The program reads the command from an commands.txt file.
        Commands formats are:
        EMAIL <sender>,<subject>,<date>
        NEXT
        READ
        COUNT

Output: Email information and unread email count will be displayed.
        
Collaborator: Gemini was used for the final version of code from two GenAI(Claude & Gemini) comparison code. 
              I modified the Gemini's code by fixing the empty Email date issue, input NEXT and COUNT output to match assignment requirements, and adding comments line by line throughout the program.

Other sources: 
Claude - https://claude.ai/ 
Gemini - https://gemini.google.com/

Author: Saad Hasnain

Creation Date: 09/26/2026

Revision Date: 09/26/2026

Revisions: None 
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Enumeration for Sender Priority (lower numerical value = higher priority)
enum class SenderCategory {
    Boss = 1, // Highest sender priority.
    Subordinate = 2, // Second-highest sender priority.
    Peer = 3, // Third highest sender priority.
    ImportantPerson = 4, // Fourth highest sender priority.
    OtherPerson = 5, // Fifth(Lowest) sender priority.
    Unknown = 6 // Used as an  unrecognized category.
};

// Represents an individual email
class Email {
private:
    std::string senderStr; // Stores the sender category as a text.
    SenderCategory category; // Stores the sender's priority category.
    std::string subject; // Stores the email subject line.
    std::string dateStr; // Stores the original MM-DD-YYYY date.
    int year; // Stores the year for comparison.
    int month; // Stores the month for comparison.
    int day; // Stores the day for comparison.

    // Helper to map string category to Enum
    SenderCategory parseCategory(const std::string& cat) {
        if (cat == "Boss") return SenderCategory::Boss; // Map boss
        if (cat == "Subordinate") return SenderCategory::Subordinate; //Map Subordinate
        if (cat == "Peer") return SenderCategory::Peer; // Map Peer
        if (cat == "ImportantPerson") return SenderCategory::ImportantPerson; // Map ImportantPerson
        if (cat == "OtherPerson") return SenderCategory::OtherPerson; // Map OtherPerson
        return SenderCategory::Unknown; // Handle any unknown category
    }

    // Helper to parse MM-DD-YYYY for date comparison
    void parseDate(const std::string& d) {
        std::stringstream ss(d); // create a stream from the date
        std::string m, dayStr, y; // stores the month, date and year
        std::getline(ss, m, '-'); // read the month
        std::getline(ss, dayStr, '-'); // Read the day
        std::getline(ss, y, '-'); // Read the year
        month = std::stoi(m); // Convert the month to integer value
        day = std::stoi(dayStr); // Convert the day to integer value
        year = std::stoi(y); // Convert the year to integer value
    }

public:
    Email(std::string sender, std::string subj, std::string date) 
        : senderStr(sender), subject(subj), dateStr(date) {
        category = parseCategory(sender); // Determine the sender priority
        parseDate(date); // Parse the email date
    }

    std::string getSender() const { return senderStr; } // Return Email Sender 
    std::string getSubject() const { return subject; } // Return the Email Subject
    std::string getDate() const { return dateStr; } // Return Email date

    // Priority Comparison logic:
    // Returns true if 'this' email has HIGHER priority than 'other'
    bool isHigherPriorityThan(const Email& other) const {
        if (this->category != other.category) {
            return this->category < other.category; // Higher priority category wins
        }

        // If categories match, compare dates (Newest email wins)
        if (this->year != other.year) return this->year > other.year; // compare year
        if (this->month != other.month) return this->month > other.month; // compare month
        return this->day > other.day; // compare day
    }
};

// Node structure for List-Based Binary Heap implementation
struct HeapNode {
    Email data; // stores the email in this node 
    HeapNode* parent; // points parent node
    HeapNode* left; // points to left child 
    HeapNode* right; // points to right child

    HeapNode(Email email) 
        : data(email), parent(nullptr), left(nullptr), right(nullptr) {} // Initialize heap node
};

// Custom List-Based (Tree-based) MaxHeap Class
class MaxHeap {
private:
    HeapNode* root; // points root of MaxHeap
    int size; // stores current number for heap node

    // Helper function to find a node by its 1-based index (Level-order Traversal via Bit Path)
    HeapNode* getNodeAtIndex(int index) const { 
        if (index <= 0 || index > size) return nullptr; // Return null for an invalid index
        if (index == 1) return root; // return root when index is 1

        // Collect path bits from root to target index
        std::vector<int> path; // stores the path of target node 
        while (index > 1) { // WHile loop continues until the root index is reached
            path.push_back(index % 2); // 0 = left child, 1 = right child
            index /= 2; // Move forward to parent index
        }

        HeapNode* curr = root; // Start traversal at root
        for (int i = path.size() - 1; i >= 0; --i) { // For loop, follow the path to targeted node
            if (path[i] == 0) curr = curr->left; // Move to the left child
            else curr = curr->right; // Move to the right child
        }
        return curr; // Return located node
    }

    // Swaps email payloads between two nodes
    void swapData(HeapNode* a, HeapNode* b) { 
        Email temp = a->data; // stores first Email (Temporarily)
        a->data = b->data; // copy second email into first node
        b->data = temp; // copy saved Email to second node.
    }

    // Restores heap order upward starting from the given node
    void heapifyUp(HeapNode* node) {
        while (node->parent != nullptr && node->data.isHigherPriorityThan(node->parent->data)) { // Check parent priority
            swapData(node, node->parent); // swap with parent that has lower priority
            node = node->parent; // continue upward
        }
    }

    // Restores heap order downward starting from the given node
    void heapifyDown(HeapNode* node) {
        while (node->left != nullptr) { // continue while the code has a child 
            HeapNode* maxChild = node->left; // Assume left child have highest priority 

            if (node->right != nullptr && node->right->data.isHigherPriorityThan(node->left->data)) { // compare childrens
                maxChild = node->right; // Select Right child
            }

            if (maxChild->data.isHigherPriorityThan(node->data)) { // Check if child outranks parent
                swapData(node, maxChild); // Swap with child that have high priority
                node = maxChild; // continue downward
            } else {
                break; // stops, when order of heap is correct.
            }
        }
    }

    // Dynamic clean-up for deep tree deletion
    void clear(HeapNode* node) { 
        if (!node) return; //stops when the node is null
        clear(node->left); // Delete left subtree
        clear(node->right); // Delete right subtree
        delete node; // Delete the current node
    }

public:
    MaxHeap() : root(nullptr), size(0) {} // Initialize the empty maxheap

    ~MaxHeap() {
        clear(root); // Removes all allocated nodes
    }

    int getSize() const {
        return size; // Return number of heap nodes
    }

    bool isEmpty() const {
        return size == 0; // Return True, when Heap is empty
    }

    // Insert a new Email into the heap
    void insert(const Email& email) { 
        HeapNode* newNode = new HeapNode(email); // create new heap 
        size++; // heap size increment

        if (root == nullptr) { // check whether the heap is empty
            root = newNode; // Makes the new node the root
            return; // Finish of insertion
        }

        // Find the parent node where the new child must attach
        HeapNode* parentNode = getNodeAtIndex(size / 2); // locate new node's parent 
        newNode->parent = parentNode; // connect a new node to its parent 

        if (size % 2 == 0) { // Even index refers left child
            parentNode->left = newNode; // Attach node as left child
        } else {
            parentNode->right = newNode; // Attach node as the right child 
        }

        heapifyUp(newNode); // Restore Maxheap ordering
    }

    // Peek at the highest priority Email without removing it
    const Email* peek() const {
        if (isEmpty()) return nullptr; // if heap is empty, return null           
        return &(root->data);  // Return the highest priority email                           
    }

    // Remove the highest priority Email
    bool extractMax() {
        if (isEmpty()) return false; // Return False, if heap is empty
        if (size == 1) { // Handle heap containing only one email
            delete root; // Delete the root node
            root = nullptr; // Delete root pointer
            size = 0; // Reset heap size to zero
            return true; // Return successfull removal 
        }

        // Get last node in complete binary tree
        HeapNode* lastNode = getNodeAtIndex(size); // Locate the final heap node
        HeapNode* lastParent = lastNode->parent; // Store its parent

        // Move last node's content to root
        root->data = lastNode->data; // Replace root email with final email

        // Disconnect last node
        if (lastParent->left == lastNode) { // check if its left child or not
            lastParent->left = nullptr; // Disconnects the left child
        } else {
            lastParent->right = nullptr; // Disconnects right child
        }

        delete lastNode; // Delete the node that is removed
        size--; // Heapsize decrement

        heapifyDown(root); // Restore Maxheap ordering 
        return true; // Return successfull removal 
    }
};

// Managing Class to control CEO Inbox workflow
class EmailInboxManager {
private:
    MaxHeap priorityQueue; // Stores unread emails by priority

public:
    void addEmail(const std::string& line) {
        std::stringstream ss(line); // create a stream for the email data
        std::string sender, subject, date; // stores three email fields(sender, subject, data)

        if (std::getline(ss, sender, ',') && //Read the sender category
            std::getline(ss, subject, ',') && //Read the subject category
            std::getline(ss, date)) { //Read the date category
            
            Email email(sender, subject, date); // creating email object
            priorityQueue.insert(email); // Insert Email into Maxheap
        }
    }

    void displayNext() {
        const Email* email = priorityQueue.peek(); // Get the highest priority email  
        if (email != nullptr) { // check if email exists
            std::cout << "Next email:\n"; // Display the required next handling  
            std::cout << "Sender: " << email->getSender() << "\n"; // Display sender
            std::cout << "Subject: " << email->getSubject() << "\n"; // Display subject
            std::cout << "Date: " << email->getDate() << "\n"; // Display date
        } else {
            std::cout << "No unread emails.\n"; // handle safely an empty inbox
        }
    }


    void readNext() {
    if (!priorityQueue.extractMax()) { // Removes the highest priority email
        std::cout << "No unread emails to read.\n"; // handle safely an empty inbox
        }
    }

    // Modified as per 2g (Author)
    void displayCount() const {
        std::cout << "There are " << priorityQueue.getSize() << " emails to read.\n"; //  Displays unread counts of emails
    }

    // Command File Processor
    void processCommandFile(const std::string& filename) { 
        std::ifstream file(filename); // open the command file 
        if (!file.is_open()) { // check whether the file opened
            std::cerr << "Error: Could not open file " << filename << std::endl; // if file wont open display the error message 
            return; // stops proccessing
        }

        std::string line; // stores each command line
        while (std::getline(file, line)) { // Read each line from the file
            if (line.empty()) continue; // Ignore empty lines

            if (line.rfind("EMAIL ", 0) == 0) { // Check for EMAIL command 
                addEmail(line.substr(6)); // add a new email
            } else if (line == "NEXT") { // Check for NEXT command 
                displayNext(); // Displays highest priority email
            } else if (line == "READ") { // Check for READ command 
                readNext(); // Highest priority email removed
            } else if (line == "COUNT") { // Check for COUNT command 
                displayCount(); // Displays number of unread emails
            }
        }

        file.close(); // command file closed
    }
};

int main(int argc, char* argv[]) {
    std::string filename = "commands.txt"; // use commands.txt by default 
    if (argc > 1) { // check for supplied filename 
        filename = argv[1]; // use the supplied filename 
    }

    EmailInboxManager manager; // create inbox manager
    manager.processCommandFile(filename); // process the command file

    return 0; // Ends program
}
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <limits>  // Needed for numeric_limits<streamsize>::max()
#include <cctype>

using namespace std;

// --------------------- Utility Functions ---------------------
string toLower(const string& s) {
    string ret = s;
    transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    return ret;
}

bool getValidatedInteger(int& number) {
    if (!(cin >> number)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}
bool getValidatedDouble(double& number) {
    if (!(cin >> number)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}
bool getValidatedString(string& input, bool allowSpaces = false) {
    getline(cin, input);
    // Validate each character in the input
    for (char ch : input) {
        if (allowSpaces && ch == ' ')
            continue;
        if (!isalpha(ch))
            return false;
    }
    return true;
}
bool isValidDate(const string& date) {
    // Check if length is exactly 10 characters (YYYY-MM-DD)
    if (date.size() != 10)
        return false;
    // Check that the 5th and 8th characters are hyphens
    if (date[4] != '-' || date[7] != '-')
        return false;

    // Extract year, month, and day substrings
    string year = date.substr(0, 4);
    string month = date.substr(5, 2);
    string day = date.substr(8, 2);

    // Check that all characters in year, month, and day are digits
    if (!all_of(year.begin(), year.end(), ::isdigit) ||
        !all_of(month.begin(), month.end(), ::isdigit) ||
        !all_of(day.begin(), day.end(), ::isdigit))
        return false;

    // Convert month and day to integers for further checking
    int m = stoi(month);
    int d = stoi(day);

    // Basic validation: month must be between 1 and 12, day between 1 and 31.
    if (m < 1 || m > 12)
        return false;
    if (d < 1 || d > 31)
        return false;

    return true;
}

// --------------------- Product Class ---------------------
// Note: Product id is now an integer.
class Product {
public:
    int id;
    string name;
    string category;
    double price;
    int quantity;
    string Date;    // Expiry date or similar
    double discount;
    double tax;

    Product(int id = 0, string name = "", string category = "",
        double price = 0.0, int quantity = 0, string Date = "",
        double discount = 0.0, double tax = 0.0)
        : id(id), name(name), category(category),
        price(price), quantity(quantity), Date(Date),
        discount(discount), tax(tax) {
    }

    bool operator<(const Product& other) const {
        return id < other.id;
    }
};

// --------------------- Binary Search Tree ---------------------
struct TreeNode {
    Product product;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Product p) : product(p), left(nullptr), right(nullptr) {}
};

class ProductTree {
private:
    TreeNode* root;

    TreeNode* insert(TreeNode* node, Product product) {
        if (node == nullptr)
            return new TreeNode(product);
        if (product.id < node->product.id)
            node->left = insert(node->left, product);
        else
            node->right = insert(node->right, product);
        return node;
    }

    // Search by product id (integer)
    TreeNode* search(TreeNode* node, int id) {
        if (node == nullptr || node->product.id == id)
            return node;
        if (id < node->product.id)
            return search(node->left, id);
        return search(node->right, id);
    }

    // delete by product id
    TreeNode* deleteNode(TreeNode* root, int id) {
        if (root == nullptr)
            return root;
        if (id < root->product.id) {
            root->left = deleteNode(root->left, id);
        }
        else if (id > root->product.id) {
            root->right = deleteNode(root->right, id);
        }
        else {
            if (root->left == nullptr) {
                TreeNode* temp = root->right;
                delete root;
                return temp;
            }
            else if (root->right == nullptr) {
                TreeNode* temp = root->left;
                delete root;
                return temp;
            }
            TreeNode* successor = root->right;
            while (successor->left != nullptr) {
                successor = successor->left;
            }
            root->product = successor->product;
            root->right = deleteNode(root->right, successor->product.id);
        }
        return root;
    }

    void inorder(TreeNode* node, vector<Product>& result) {
        if (!node) return;
        inorder(node->left, result);
        result.push_back(node->product);
        inorder(node->right, result);
    }

public:
    ProductTree() : root(nullptr) {}

    void insert(Product product) {
        root = insert(root, product);
    }

    // Search by id (integer)
    Product* search(int id) {
        TreeNode* result = search(root, id);
        return (result ? &result->product : nullptr);
    }

    void deleteProduct(int id) {
        root = deleteNode(root, id);
    }

    vector<Product> getAllProducts() {
        vector<Product> products;
        inorder(root, products);
        return products;
    }
};

// --------------------- Linked List for Wishlist ---------------------
struct ListNode {
    Product product;
    ListNode* next;

    ListNode(Product p) : product(p), next(nullptr) {}
};

class LinkedList {
private:
    ListNode* head;
public:
    LinkedList() : head(nullptr) {}

    void add(Product product) {
        ListNode* newNode = new ListNode(product);
        newNode->next = head;
        head = newNode;
    }

    void remove(int id) {
        ListNode* temp = head;
        ListNode* prev = nullptr;
        while (temp && temp->product.id != id) {
            prev = temp;
            temp = temp->next;
        }
        if (!temp) return;  // not found
        if (!prev)
            head = temp->next;
        else
            prev->next = temp->next;
        delete temp;
    }

    vector<Product> getAll() {
        vector<Product> products;
        ListNode* temp = head;
        while (temp) {
            products.push_back(temp->product);
            temp = temp->next;
        }
        return products;
    }

    bool isEmpty() {
        return (head == nullptr);
    }
};

// --------------------- Order History Stack ---------------------
class OrderStack {
private:
    stack<Product> orders;
public:
    void push(Product product) {
        orders.push(product);
    }
    Product pop() {
        Product product = orders.top();
        orders.pop();
        return product;
    }
    bool isEmpty() {
        return orders.empty();
    }
};

// --------------------- Low Stock Alerts Queue ---------------------
class LowStockQueue {
private:
    queue<Product> lowStockProducts;
public:
    void enqueue(Product product) {
        lowStockProducts.push(product);
    }
    Product dequeue() {
        Product product = lowStockProducts.front();
        lowStockProducts.pop();
        return product;
    }
    bool isEmpty() {
        return lowStockProducts.empty();
    }
};

// --------------------- PointOfSaleSystem Class ---------------------
class PointOfSaleSystem {
private:
    ProductTree productTree;
    LinkedList wishlist;
    OrderStack orderHistory;
    LowStockQueue lowStockAlerts;
    vector<pair<string, string>> admins;

    // Updated file names for CSV files
    const string productFile = "products.csv";
    const string adminFile = "admins.csv";
    const string ordersFile = "orders.csv";
    const string wishlistFile = "wishlist.csv";

    // Helper function to split a CSV line into tokens
    vector<string> splitCSV(const string& line) {
        vector<string> tokens;
        stringstream ss(line);
        string token;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        return tokens;
    }

    // ----------------------- Products -----------------------
    void loadProductsFromFile() {
        ifstream file(productFile);
        if (!file) {
            cout << "\nNo existing product records found. Starting fresh.\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> tokens = splitCSV(line);
            if (tokens.size() != 8) {
                cout << "Skipping malformed record: " << line << "\n";
                continue;
            }
            try {
                int id = stoi(tokens[0]);
                string name = tokens[1];
                string category = tokens[2];
                double price = stod(tokens[3]);
                int quantity = stoi(tokens[4]);
                double discount = stod(tokens[5]);
                double tax = stod(tokens[6]);
                string Date = tokens[7];
                productTree.insert(Product(id, name, category, price,
                    quantity, Date, discount, tax));
            }
            catch (...) {
                cout << "Error parsing record: " << line << "\n";
            }
        }
        file.close();
    }

    void saveProductsToFile() {
        ofstream file(productFile, ios::trunc);
        if (!file) {
            cout << "\nError saving products to file.\n";
            return;
        }
        vector<Product> products = productTree.getAllProducts();
        for (const auto& product : products) {
            // CSV: id,name,category,price,quantity,discount,tax,Date
            file << product.id << ","
                << product.name << ","
                << product.category << ","
                << product.price << ","
                << product.quantity << ","
                << product.discount << ","
                << product.tax << ","
                << product.Date << "\n";
        }
        file.close();
    }

    // ----------------------- Admins -----------------------
    void loadAdminsFromFile() {
        ifstream file(adminFile);
        if (!file) {
            // If no admin file, create a default admin
            admins.emplace_back("admin", "admin");
            return;
        }
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> tokens = splitCSV(line);
            if (tokens.size() != 2) {
                cout << "Skipping malformed admin record: " << line << "\n";
                continue;
            }
            admins.emplace_back(tokens[0], tokens[1]);
        }
        file.close();
    }

    void saveAdminsToFile() {
        ofstream file(adminFile, ios::trunc);
        if (!file) {
            cout << "\nError saving admins to file.\n";
            return;
        }
        for (const auto& admin : admins) {
            file << admin.first << "," << admin.second << "\n";
        }
        file.close();
    }

    // ----------------------- Wishlist -----------------------
    void loadWishlistFromFile() {
        ifstream file(wishlistFile);
        if (!file) return; // No wishlist file yet
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> tokens = splitCSV(line);
            if (tokens.size() != 8) {
                cout << "Skipping malformed wishlist record: " << line << "\n";
                continue;
            }
            try {
                int id = stoi(tokens[0]);
                string name = tokens[1];
                string category = tokens[2];
                double price = stod(tokens[3]);
                int quantity = stoi(tokens[4]);
                double discount = stod(tokens[5]);
                double tax = stod(tokens[6]);
                string Date = tokens[7];
                wishlist.add(Product(id, name, category, price,
                    quantity, Date, discount, tax));
            }
            catch (...) {
                cout << "Error parsing wishlist record: " << line << "\n";
            }
        }
        file.close();
    }

    void saveWishlistToFile() {
        ofstream file(wishlistFile, ios::trunc);
        if (!file) {
            cout << "\nError saving wishlist to file.\n";
            return;
        }
        vector<Product> products = wishlist.getAll();
        for (const auto& product : products) {
            // CSV: id,name,category,price,quantity,discount,tax,Date
            file << product.id << ","
                << product.name << ","
                << product.category << ","
                << product.price << ","
                << product.quantity << ","
                << product.discount << ","
                << product.tax << ","
                << product.Date << "\n";
        }
        file.close();
    }

    // ----------------------- Other Functions -----------------------
    void addToWishlist() {
        int id;
        cout << "\nEnter Product ID to add to wishlist: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input ID: ";
        }
        Product* product = productTree.search(id);
        if (product) {
            wishlist.add(*product);
            cout << "\nProduct added to wishlist successfully.\n";
            saveWishlistToFile();
        }
        else {
            cout << "\nProduct not found.\n";
        }
    }

    void viewWishlist() {
        vector<Product> products = wishlist.getAll();
        if (products.empty()) {
            cout << "\nYour wishlist is empty.\n";
            return;
        }
        cout << "\nYour Wishlist:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(20) << "Category" << setw(10) << "Price"
            << setw(10) << "Qty" << setw(10) << "Discount"
            << setw(10) << "Tax" << setw(15) << "Expiry Date" << "\n";
        cout << string(105, '-') << "\n";
        for (const auto& product : products) {
            cout << left << setw(10) << product.id
                << setw(20) << product.name
                << setw(20) << product.category
                << setw(10) << fixed << setprecision(2) << product.price
                << setw(10) << product.quantity
                << setw(10) << product.discount << "%"
                << setw(10) << product.tax << "%"
                << setw(15) << product.Date << "\n";
        }
    }

    // Customer places order (cart functionality removed)
    void checkout() {
        int id;
        int orderQuantity;
        cout << "\nEnter Product ID to order: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input ID: ";
        }
        Product* product = productTree.search(id);
        if (product) {
            cout << "Enter Quantity to order: ";
            while (!getValidatedInteger(orderQuantity)) {
                cout << "Invalid input Quantity: ";
            }
            if (orderQuantity > product->quantity) {
                cout << "\nNot enough stock available. Order quantity exceeds available stock.\n";
            }
            else {
                product->quantity -= orderQuantity;
                double discountedPrice = product->price * (1 - product->discount / 100);
                double taxedPrice = discountedPrice * (1 + product->tax / 100);
                double totalPrice = taxedPrice * orderQuantity;
                cout << "\nOrder placed successfully.\n";
                cout << "Total Bill: Rs." << fixed << setprecision(2) << totalPrice << "\n";
                // Save order info
                ofstream orderFile(ordersFile, ios::app);
                orderFile << "Product ID: " << product->id << "\n";
                orderFile << "Product Name: " << product->name << "\n";
                orderFile << "Quantity Ordered: " << orderQuantity << "\n";
                orderFile << "Total Price: Rs." << fixed << setprecision(2) << totalPrice << "\n\n";
                orderFile.close();
                saveProductsToFile();
            }
        }
        else {
            cout << "\nProduct not found.\n";
        }
    }

    void viewTotalInventoryValue() {
        double totalValue = 0;
        vector<Product> products = productTree.getAllProducts();
        for (const auto& product : products) {
            double discountedPrice = product.price * (1 - product.discount / 100);
            double taxedPrice = discountedPrice * (1 + product.tax / 100);
            totalValue += taxedPrice * product.quantity;
        }
        cout << "\nTotal Inventory Value: Rs." << fixed << setprecision(2) << totalValue << "\n";
    }

    void sortProductsById() {
        vector<Product> products = productTree.getAllProducts();
        sort(products.begin(), products.end(), [](const Product& a, const Product& b) {
            return a.id < b.id;
            });
        cout << "\nProducts sorted by ID successfully.\n";
    }

    void generateReport() {
        ofstream report("report.csv");
        if (!report) {
            cout << "\nError generating report.\n";
            return;
        }
        // Write header row in CSV format
        report << "ID,Name,Category,Price,Quantity,Discount,Tax,Expiry Date\n";
        vector<Product> products = productTree.getAllProducts();
        for (const auto& product : products) {
            report << product.id << ","
                << product.name << ","
                << product.category << ","
                << fixed << setprecision(2) << product.price << ","
                << product.quantity << ","
                << product.discount << ","
                << product.tax << ","
                << product.Date << "\n";
        }
        cout << "\nReport generated successfully in 'report.csv'.\n";
    }

    void searchProduct() {
        int id;
        cout << "\nEnter product ID to search: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input ID: ";
        }
        Product* product = productTree.search(id);
        if (product) {
            cout << "\nProduct Found:\n";
            cout << "ID: " << product->id << "\n";
            cout << "Name: " << product->name << "\n";
            cout << "Category: " << product->category << "\n";
            cout << "Price: " << product->price << "\n";
            cout << "Quantity: " << product->quantity << "\n";
            cout << "Discount: " << product->discount << "%\n";
            cout << "Tax: " << product->tax << "%\n";
            cout << "Date: " << product->Date << "\n";
        }
        else {
            cout << "\nProduct not found.\n";
        }
    }

    void deleteProduct() {
        int id;
        cout << "\nEnter product ID to delete: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input ID. Please try again: ";
        }

        Product* product = productTree.search(id);
        if (product) {
            productTree.deleteProduct(id); // Correctly call the delete function
            cout << "\nProduct deleted successfully.\n";
            saveProductsToFile(); // Update the CSV file after deletion
        }
        else {
            cout << "\nProduct not found.\n";
        }
    }

    // Modified modifyProduct function with an option to add additional quantity.
    void modifyProduct() {
        int id;
        cout << "\nEnter Product ID to modify: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input for Product ID. Please try again: ";
        }

        Product* product = productTree.search(id);
        if (product) {
            cout << "\nEnter new details for the product:\n";

            // Get and validate the new name
            string newName;
            cout << "Name: ";
            while (!getValidatedString(newName, true)) {
                cout << "Invalid input for Name. Please try again: ";
            }
            product->name = newName;

            // Get and validate the new category
            string newCategory;
            cout << "Category: ";
            while (!getValidatedString(newCategory, true)) {
                cout << "Invalid input for Category. Please try again: ";
            }
            product->category = newCategory;

            // Get and validate the new price
            double newPrice;
            cout << "Price: ";
            while (!getValidatedDouble(newPrice)) {
                cout << "Invalid input for Price. Please try again: ";
            }
            product->price = newPrice;

            // Show current quantity and get additional quantity to add
            cout << "Current Quantity: " << product->quantity << "\n";
            int addQty;
            cout << "Enter quantity to add (enter 0 if no change): ";
            while (!getValidatedInteger(addQty)) {
                cout << "Invalid input for Quantity. Please try again: ";
            }
            product->quantity += addQty;

            // Get and validate the new discount
            double newDiscount;
            cout << "Enter Discount (%): ";
            while (!getValidatedDouble(newDiscount)) {
                cout << "Invalid input for Discount. Please try again: ";
            }
            product->discount = newDiscount;

            // Get and validate the new tax
            double newTax;
            cout << "Enter Tax (%): ";
            while (!getValidatedDouble(newTax)) {
                cout << "Invalid input for Tax. Please try again: ";
            }
            product->tax = newTax;

            // Get and validate the new date
            string newDate;
            cout << "Date (YYYY-MM-DD): ";
            // Ensure any leftover newline is consumed
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            while (true) {
                getline(cin, newDate);
                if (isValidDate(newDate)) {   // Assumes isValidDate() is defined elsewhere.
                    product->Date = newDate;
                    break;
                }
                else {
                    cout << "Invalid date format. Please try again (YYYY-MM-DD): ";
                }
            }

            cout << "\nProduct modified successfully.\n";
        }
        else {
            cout << "\nProduct not found.\n";
        }
    }

    void addProduct() {
        int id;
        string name, category, Date;
        double price, discount, tax;
        int quantity;

        cout << "\nEnter Product ID: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input for product ID: ";
        }

        cout << "Enter Product Name: ";
        // Use getline to get the full name (allows spaces)
        while (!getValidatedString(name, true)) {
            cout << "Invalid input for name: ";
        }

        cout << "Enter Product Category: ";
        while (!getValidatedString(category, true)) {
            cout << "Invalid input for category: ";
        }

        cout << "Enter Product Price: ";
        while (!getValidatedDouble(price)) {
            cout << "Invalid input for Price: ";
        }

        cout << "Enter Product Quantity: ";
        while (!getValidatedInteger(quantity)) {
            cout << "Invalid input for Quantity: ";
        }

        cout << "Enter Discount (%): ";
        while (!getValidatedDouble(discount)) {
            cout << "Invalid input for Discount: ";
        }

        cout << "Enter Tax (%): ";
        while (!getValidatedDouble(tax)) {
            cout << "Invalid input for Tax: ";
        }

        while (true) {
            cout << "Enter Date (YYYY-MM-DD): ";
            getline(cin, Date);

            if (isValidDate(Date)) {
                cout << "Valid date entered: " << Date << endl;
                break;  // Exit the loop if the date is valid
            }
            else {
                cout << "Invalid date format. Please try again." << endl;
            }
        }

        productTree.insert(Product(id, name, category, price,
            quantity, Date, discount, tax));
        cout << "\nProduct added successfully.\n";
        saveProductsToFile();
    }

    void showAvailableProducts() {
        vector<Product> products = productTree.getAllProducts();
        if (products.empty()) {
            cout << "\nNo products available.\n";
            return;
        }
        cout << "\nAvailable Products:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name"
            << setw(20) << "Category" << setw(10) << "Price"
            << setw(10) << "Qty" << setw(10) << "Discount"
            << setw(10) << "Tax" << setw(15) << "Date" << "\n";
        cout << string(105, '-') << "\n";
        for (const auto& product : products) {
            cout << left << setw(10) << product.id
                << setw(20) << product.name
                << setw(20) << product.category
                << setw(10) << fixed << setprecision(2) << product.price
                << setw(10) << product.quantity
                << setw(10) << product.discount << "%"  // Percentage sign for discount
                << setw(10) << product.tax << "%"       // Percentage sign for tax
                << setw(15) << product.Date << "\n";    // No percentage sign for date
        }
    }
    void addAdmin() {
        string username, password;
        cout << "\nEnter New Admin Username: ";
        while (true) {
            if (getValidatedString(username)) {
                break;
            }
            cout << "Invalid input for username. Please try again: ";
        }
        cout << "Enter New Admin Password: ";
        while (true) {
            if (getValidatedString(password)) {
                break;
            }
            cout << "Invalid input for password. Please try again: ";
        }
        admins.emplace_back(username, password);
        cout << "\nAdmin added successfully.\n";
        saveAdminsToFile();
    }

    void placeOrder() {
        int id;
        int orderQuantity;
        cout << "\nEnter Product ID to order: ";
        while (!getValidatedInteger(id)) {
            cout << "Invalid input. Please enter a valid product ID: ";
        }
        Product* product = productTree.search(id);
        if (product) {
            cout << "Enter Quantity to order: ";
            while (!getValidatedInteger(orderQuantity)) {
                cout << "Invalid input. Please enter a valid quantity: ";
            }
            if (orderQuantity > product->quantity) {
                cout << "\nNot enough stock available. Order quantity exceeds available stock.\n";
            }
            else {
                product->quantity -= orderQuantity;
                double discountedPrice = product->price * (1 - product->discount / 100);
                double taxedPrice = discountedPrice * (1 + product->tax / 100);
                double totalPrice = taxedPrice * orderQuantity;
                cout << "\nOrder placed successfully.\n";
                cout << "Total Bill: Rs." << fixed << setprecision(2) << totalPrice << "\n";
                // Save order info
                ofstream orderFile(ordersFile, ios::app);
                orderFile << "Product ID: " << product->id << "\n";
                orderFile << "Product Name: " << product->name << "\n";
                orderFile << "Quantity Ordered: " << orderQuantity << "\n";
                orderFile << "Total Price: Rs." << fixed << setprecision(2) << totalPrice << "\n\n";
                orderFile.close();
                saveProductsToFile();
            }
        }
        else {
            cout << "\nProduct not found.\n";
        }
    }

    // Modified checkLowStockLevels: if there are no products, show "No products available."
    void checkLowStockLevels() {
        vector<Product> products = productTree.getAllProducts();
        if (products.empty()) {
            cout << "\nNo products available.\n";
            return;
        }
        bool lowStockFound = false;
        cout << "\nChecking for low stock levels...\n";
        for (const auto& product : products) {
            if (product.quantity < 20) {
                cout << "Warning: Product ID " << product.id
                    << " (" << product.name << ") has low stock. Current quantity: "
                    << product.quantity << "\n";
                lowStockFound = true;
            }
        }
        if (!lowStockFound)
            cout << "No products have low stock levels.\n";
    }

public:
    PointOfSaleSystem() {
        loadAdminsFromFile();
        loadProductsFromFile();
        loadWishlistFromFile();
    }

    ~PointOfSaleSystem() {
        saveAdminsToFile();
        saveProductsToFile();
        saveWishlistToFile();
    }

    void showHeader() {
        cout << "=============================================" << endl;
        cout << "   RIPHAH INTERNATIONAL UNIVERSITY SAHIWAL   " << endl;
        cout << "=============================================" << endl;
        cout << "\n---------------------------------------------" << endl;
        cout << "            E-Sales Terminal:)              " << endl;
        cout << "---------------------------------------------" << endl;
        cout << "   1. Admin.                                 " << endl;
        cout << "   2. Customer.                              " << endl;
        cout << "   3. Exit.                                  " << endl;
        cout << "---------------------------------------------" << endl;
    }

    bool login() {
        string username, password;
        // This loop will keep asking for valid credentials
        while (true) {
            cout << "Enter admin username: ";
            while (!getValidatedString(username)) {
                cout << "Invalid input for username. Please try again: ";
            }

            cout << "Enter admin password: ";
            while (!getValidatedString(password)) {
                cout << "Invalid input for password. Please try again: ";
            }

            // Check credentials
            for (const auto& admin : admins) {
                if (admin.first == username && admin.second == password) {
                    cout << "\nSuccessfully logged in!\n";
                    return true;
                }
            }
            cout << "\nAccess denied. Invalid username or password. Please try again.\n";
        }
    }

    void adminMenu() {
        int choice;
        do {
            cout << "\nAdmin Menu\n";
            cout << "----------------------------------------" << endl;
            cout << "1. Add Product\n";
            cout << "2. Show Available Products\n";
            cout << "3. Modify Product\n";
            cout << "4. Delete Product\n";
            cout << "5. Add Admin\n";
            cout << "6. Search Product\n";
            cout << "7. Sort Products by ID\n";
            cout << "8. View Total Inventory Value\n";
            cout << "9. Generate Report\n";
            cout << "10. Check Low Stock Levels\n";
            cout << "11. Exit\n";

            while (true) {
                cout << "Enter your choice: ";
                if (getValidatedInteger(choice)) {
                    break;
                }
                cout << "Invalid input. Please enter a valid number.\n";
            }

            switch (choice) {
            case 1: addProduct(); break;
            case 2: showAvailableProducts(); break;
            case 3: modifyProduct(); break;
            case 4: deleteProduct(); break;
            case 5: addAdmin(); break;
            case 6: searchProduct(); break;
            case 7: sortProductsById(); break;
            case 8: viewTotalInventoryValue(); break;
            case 9: generateReport(); break;
            case 10: checkLowStockLevels(); break;
            case 11: cout << "\nExiting Admin Menu...\n"; break;
            default: cout << "\nInvalid choice. Please try again.\n";
            }
        } while (choice != 11);
    }

    void customerMenu() {
        int choice;
        do {
            cout << "\nCustomer Menu\n";
            cout << "1. Place Order\n";
            cout << "2. Show Available Products\n";
            cout << "3. Add to Wishlist\n";
            cout << "4. View Wishlist\n";
            cout << "5. Exit\n";

            while (true) {
                cout << "Enter your choice: ";
                if (getValidatedInteger(choice)) {
                    break;
                }
                cout << "Invalid input. Please enter a valid number.\n";
            }

            switch (choice) {
            case 1: placeOrder(); break;
            case 2: showAvailableProducts(); break;
            case 3: addToWishlist(); break;
            case 4: viewWishlist(); break;
            case 5: cout << "\nExiting Customer Menu...\n"; break;
            default: cout << "\nInvalid choice. Please try again.\n";
            }
        } while (choice != 5);
    }

    void startProgram() {
        int choice;
        do {
            showHeader();

            while (true) {
                cout << "Enter your choice: ";
                if (getValidatedInteger(choice)) {
                    break;
                }
                cout << "Invalid choice input. Please enter a valid number.\n";
            }

            switch (choice) {
            case 1:
                if (login()) {
                    adminMenu();
                }
                break;
            case 2:
                customerMenu();
                break;
            case 3:
                cout << "\nExiting Program... Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
            }
        } while (choice != 3);
    }
};

int main() {
    PointOfSaleSystem system;
    system.startProgram();
    return 0;
}
#include <iostream>
#include <memory>
#include <string>

using namespace std;

// Sample resource class to track object lifecycle
class Resource {
private:
    string name;
public:
    Resource(const string& n) : name(n) {
        cout << "[Resource Acquired] " << name << "\n";
    }
    ~Resource() {
        cout << "[Resource Released] " << name << "\n";
    }
    void use() const {
        cout << "Using resource: " << name << "\n";
    }
};

// Custom RAII Smart Pointer implementation
template <typename T>
class CustomUniquePtr {
private:
    T* ptr;
public:
    explicit CustomUniquePtr(T* p = nullptr) : ptr(p) {}

    ~CustomUniquePtr() {
        delete ptr;
    }

    // Delete copy constructor and copy assignment to enforce unique ownership
    CustomUniquePtr(const CustomUniquePtr&) = delete;
    CustomUniquePtr& operator=(const CustomUniquePtr&) = delete;

    // Move constructor
    CustomUniquePtr(CustomUniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // Move assignment
    CustomUniquePtr& operator=(CustomUniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }
};

int main() {
    int choice;
    cout << "Select Smart Pointer Demo:\n";
    cout << "1. Custom RAII UniquePtr (Ownership Transfer)\n";
    cout << "2. std::unique_ptr & std::shared_ptr Reference Counting\n";
    cout << "3. std::weak_ptr Breaking Circular Reference\n";
    cout << "Enter choice (1-3): ";
    cin >> choice;

    if (choice == 1) {
        cout << "\n--- Custom RAII UniquePtr Demo ---\n";
        CustomUniquePtr<Resource> res1(new Resource("Custom_DB_Connection"));
        res1->use();

        cout << "Transferring ownership via std::move...\n";
        CustomUniquePtr<Resource> res2 = std::move(res1);
        if (res1.get() == nullptr) {
            cout << "res1 is now nullptr.\n";
        }
        res2->use();
        // res2 goes out of scope here and automatically frees memory
    } 
    else if (choice == 2) {
        cout << "\n--- Standard Smart Pointers Demo ---\n";
        unique_ptr<Resource> uPtr = make_unique<Resource>("Unique_File_Handle");
        uPtr->use();

        shared_ptr<Resource> sPtr1 = make_shared<Resource>("Shared_Network_Socket");
        cout << "Initial use_count: " << sPtr1.use_count() << "\n";
        {
            shared_ptr<Resource> sPtr2 = sPtr1;
            cout << "Inside inner block use_count: " << sPtr1.use_count() << "\n";
            sPtr2->use();
        }
        cout << "After inner block use_count: " << sPtr1.use_count() << "\n";
    } 
    else if (choice == 3) {
        cout << "\n--- std::weak_ptr Demo ---\n";
        weak_ptr<Resource> wPtr;
        {
            shared_ptr<Resource> sPtr = make_shared<Resource>("Temporary_Cache");
            wPtr = sPtr;
            cout << "Inside scope - Is expired? " << (wPtr.expired() ? "Yes" : "No") << "\n";
            if (auto locked = wPtr.lock()) {
                locked->use();
            }
        }
        cout << "Outside scope - Is expired? " << (wPtr.expired() ? "Yes" : "No") << "\n";
    } 
    else {
        cout << "Invalid choice.\n";
    }

    return 0;
}
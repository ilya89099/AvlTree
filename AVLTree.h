#ifndef DA2_AVLTREE_H
#define DA2_AVLTREE_H

#include <tuple>
#include <cmath>
#include <fstream>


template <typename Key, typename Value>
struct TAvlNode {
    TAvlNode() = default;

    TAvlNode(Key newKey, Value newValue, TAvlNode* parentNode, TAvlNode* nil) {
        key = newKey;
        value = newValue;
        left = nil;
        right = nil;
        parent = parentNode;
    }

    Key key;
    Value value;

    int balance = 0;
    TAvlNode* parent = nullptr;
    TAvlNode* left = nullptr;
    TAvlNode* right = nullptr;
};

template <typename Key, typename Value>
class TAvlTree {

    using TNodeType = TAvlNode<Key, Value>;

public:
    TAvlTree() {
        nil = new TNodeType;
        Root() = nil;
    };

    ~TAvlTree() {
        DeleteSubtree(Root());
        delete nil;
    }

    TAvlTree(const TAvlTree&) = delete;

    std::pair<Value, bool> operator[] (const Key& key) {

        TNodeType* temp = FindNode(key);
        if (temp == nil) {
            return {Value(), false};
        } else {
            return {temp->value, true};
        }
    }

    bool Insert(Key key, Value value) {
        
        TNodeType* temp = FindNode(key);
        if (temp != nil) {
            return false;
        }
        temp = temp->parent;
        TNodeType* newNode = new TNodeType(key, value, temp, nil);
        if (temp == nil) {
            Root() = newNode;
            return true;
        }
        if (newNode->key > temp->key) {
            temp->right = newNode;
            temp->balance -= 1;
        } else {
            temp->left = newNode;
            temp->balance += 1;
        }
        RebalanceTree(temp, RebalanceType::Insert);
        return true;
    }

    bool Erase(Key key) {
        
        TNodeType* curNode = FindNode(key);
        if (curNode == nil) {
            return false;
        }
        if (curNode->left == nil && curNode->right == nil) {
            //RebalanceTree(DeleteLeave(curNode), RebalanceType::Delete); // перебалансировать начиная с родителя удаленного листа

            TNodeType* parent = curNode->parent;
            if (curNode == parent->right) {
                parent->balance += 1;
                parent->right = nil;
            } else {
                parent->balance -= 1;
                parent->left = nil;
            }
            delete curNode;
            RebalanceTree(parent, RebalanceType::Delete);
        } else if (curNode->right == nil) { // частный случай
            curNode->value = curNode->left->value;
            curNode->key = curNode->left->key;
            curNode->balance -= 1;
            delete curNode->left;
            curNode->left = nil;
            RebalanceTree(curNode, RebalanceType::Delete);
        } else {
            TNodeType* replacer = TreeMin(curNode->right);
            TNodeType* replacerParent = replacer->parent;
            curNode->value = replacer->value;
            curNode->key = replacer->key;
            if (replacerParent == curNode) {
                replacerParent->balance += 1;
                replacerParent->right = replacer->right;
                replacerParent->right->parent = replacerParent;
            } else {
                replacerParent->balance -= 1;
                replacerParent->left = replacer->right;
                replacerParent->left->parent = replacerParent;
            }
            delete replacer;
            RebalanceTree(replacerParent, RebalanceType::Delete);
        }
        return true;
    }

    void SaveToFile(const char* filename) {
        
        std::ofstream file;
        file.open(filename, std::ios::binary | std::ios::out);
        if (!file.is_open()) {
            return;
        }
        SaveRecursive(file, Root());
        file.close();
    }

    void LoadFromFile(const char* filename) {
        
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return;
        }
        TNodeType *new_root = LoadRecursive(file, nil);
        DeleteSubtree(Root());
        Root() = new_root;
    }

    void Print() {
        Print(Root(), 0);
    }

private:
    //variables
    TNodeType* nil;

    //delete functions
    void DeleteSubtree(TNodeType* node) {
        if (node == nil) {
            return;
        }
        TNodeType* right = node->right;
        TNodeType* left = node->left;
        delete node;
        DeleteSubtree(right);
        DeleteSubtree(left);
    }

    //helpers
    TNodeType* TreeMin(TNodeType* node) {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

    TNodeType* TreeMax(TNodeType* node) {
        while (node->right != nil) {
            node = node->right;
        }
        return node;
    }


    //save and load
    void SaveRecursive(std::ofstream& os, TNodeType* node) {
        
        char has_info = int(node != nil) + '0';
        os.write((char*) &has_info, sizeof(char));
        if (has_info == '0') {
            return;
        }
        size_t size = node->key.Size();
        os.write((char*) &size, sizeof(size_t));
        os.write((char*) node->key.Data(), sizeof(char) * node->key.Size());
        os.write((char*) &(node->value), sizeof(Value));
        os.write((char*) &(node->balance), sizeof(int));
        SaveRecursive(os, node->left);
        SaveRecursive(os, node->right);
    }

    TNodeType* LoadRecursive(std::ifstream& is, TNodeType* parentNode) {
        
        char has_info;
        is.read(&has_info, sizeof(char));
        if (has_info == '0') {
            return nil;
        }

        Value value;
        int balance;
        size_t size;

        is.read((char*) &size, sizeof(size_t));
        char* buffer = new char[size];

        Key key(size);

        is.read((char*) buffer, sizeof(char) * size);
        is.read((char*) &value, sizeof(Value));
        is.read((char*) &balance, sizeof(int));

        for (size_t i = 0; i < size; ++i) {
            key[i] = buffer[i];
        }
        delete[] buffer;

        TNodeType* newNode = new TNodeType(key,value,parentNode, nil);
        newNode->balance = balance;
        newNode->left = LoadRecursive(is, newNode);
        newNode->right = LoadRecursive(is, newNode);
        return newNode;
    }


    //rebalance
    enum class RebalanceType {
        Delete, Insert
    };

    void RebalanceTree(TNodeType* node, RebalanceType type) {
        
        if (node == nil) {
            return;
        }

        int exit_value = (type == RebalanceType::Delete ? 1 : 0);
        int right_change = (type == RebalanceType::Delete ? 1 : -1);
        int left_change = -right_change;
        if (std::abs(node->balance) == exit_value) {
            return;
        }

        if (abs(node->balance) != 2) {
            if (node->parent->right == node) {
                node->parent->balance += right_change;
            } else {
                node->parent->balance += left_change;
            }
            RebalanceTree(node->parent, type);
            return;
        }

        if (node->balance == -2) {
            TNodeType* a = node;
            TNodeType* b = node->right;
            if (b->balance == -1 || b->balance == 0) {
                if (b->balance == -1) {
                    a->balance = 0;
                    b->balance = 0;
                } else {
                    a->balance = -1;
                    b->balance = 1;
                }
                RebalanceTree(LesserLeftRotate(a), type);
            } else {
                TNodeType* c = b->left;
                switch (c->balance) {
                    case 1 :
                        a->balance = 0;
                        b->balance = -1;
                        c->balance = 0;
                        break;
                    case -1 :
                        a->balance = 1;
                        b->balance = 0;
                        c->balance = 0;
                        break;
                    case 0 :
                        a->balance = 0;
                        b->balance = 0;
                        c->balance = 0;
                        break;
                }
                RebalanceTree(GreaterLeftRotate(a), type);
            }
        } else if (node->balance == 2) {
            TNodeType* a = node;
            TNodeType* b = node->left;
            if (b->balance == 1 || b->balance == 0) {
                if (b->balance == 1) {
                    a->balance = 0;
                    b->balance = 0;
                } else {
                    a->balance = 1;
                    b->balance = -1;
                }
                RebalanceTree(LesserRightRotate(a), type);
            } else {
                TNodeType* c = b->right;
                switch (c->balance) {
                    case 1 :
                        a->balance = -1;
                        b->balance = 0;
                        c->balance = 0;
                        break;
                    case -1 :
                        a->balance = 0;
                        b->balance = 1;
                        c->balance = 0;
                        break;
                    case 0 :
                        a->balance = 0;
                        b->balance = 0;
                        c->balance = 0;
                        break;
                }
                RebalanceTree(GreaterRightRotate(a), type);
            }
        }
    }

    TNodeType* LesserLeftRotate(TNodeType* node) {
        TNodeType* parent = node->parent;
        TNodeType* a = node;
        TNodeType* b = node->right;
        TNodeType* beta = node->right->left;

        a->right = beta;
        beta->parent = a;
        b->left = a;
        a->parent = b;

        if (parent->left == node) {
            parent->left = b;
            b->parent = parent;
        } else {
            parent->right = b;
            b->parent = parent;
        }

        return b;
    };

    TNodeType* LesserRightRotate(TNodeType* node) {
        
        TNodeType* parent = node->parent;
        TNodeType* b = node;
        TNodeType* a = node->left;
        TNodeType* beta = node->left->right;

        a->right = b;
        b->parent = a;
        b->left = beta;
        beta->parent = b;

        if (parent->left == node) {
            parent->left = a;
            a->parent = parent;
        } else {
            parent->right = a;
            a->parent = parent;
        }

        return a;
    };

    TNodeType* GreaterLeftRotate(TNodeType* node) {
        
        TNodeType* a = node;
        TNodeType* b = a->right;
        TNodeType* c = b->left;

        LesserRightRotate(b);
        LesserLeftRotate(a);

        return c;
    }

    TNodeType* GreaterRightRotate(TNodeType* node) {
        
        TNodeType* a = node;
        TNodeType* b = a->left;
        TNodeType* c = b->right;

        LesserLeftRotate(b);
        LesserRightRotate(a);

        return c;
    }

    TNodeType* FindNode(const Key& key) {
        TNodeType* prev = nil;
        TNodeType* curNode = Root();
        while(curNode != nil && curNode->key != key) {
            prev = curNode;
            if (key < curNode->key) {
                curNode = curNode->left;
            } else {
                curNode = curNode->right;
            }
        }
        if (curNode == nil) {
            curNode->parent = prev;
        }
        return curNode;
    }



    void Print(TNodeType* node, int depth) {
        
        for (int i = 0; i < depth * 2; ++i) {
            std::cout << " ";
        }
        if (node == nil) {
            std::cout << "0" << "\n";
        } else {
            std::cout << node->key << " " << node->value << " " << node->balance << "\n";
            Print(node->left, depth + 1);
            Print(node->right, depth + 1);
        }


    }

    struct ProxyRoot {
        ProxyRoot(TNodeType* newNil) : nil(newNil) {}

        operator TNodeType*() const {
            return nil->left;
        }

        ProxyRoot& operator = (TNodeType* node) {
            nil->left = node;
            node->parent = nil;
            return *this;
        }

    private:
        TNodeType* nil;
    };

    ProxyRoot Root() {
        return ProxyRoot(nil);
    }




};

#endif //DA2_AVLTREE_H

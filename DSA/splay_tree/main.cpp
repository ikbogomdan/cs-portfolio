#include <iostream>
#include <algorithm>
#include <vector>
#include <climits>
struct Node {
    int value = -1;
    Node* left_son = nullptr;
    Node* right_son = nullptr; 
    Node* parent = nullptr;
    Node(int v, Node* p): value(v), parent(p){}
    Node() = default;
    ~Node() {
        delete left_son;
        delete right_son;
    }
};
class Splay {
    Node* root = nullptr;
    int size = 0;

    bool Isroot(Node* x) {
        return x->parent == root;
    }
    bool Oneside(Node* x) {
        Node* f = x->parent; 
        Node* g = f->parent; 
        return (((f->left_son == x) && (g->left_son == f)) || 
        ((f->right_son == x) && (g->right_son == f)));
    }
    bool Onesideleft(Node* x) {
        Node* f = x->parent; 
        Node* g = f->parent; 
        return ((f->left_son == x) && (g->left_son == f));
    }
    bool Othersideleft(Node* x) {
        Node* f = x->parent; 
        Node* g = f->parent; 
        return ((f->right_son == x) && (g->left_son == f)); 
    }
    void Leftround(Node* t) {
        Node* a = t;
        Node* b = t->left_son;
        Node* betta = b->right_son;
        a->left_son = betta;
        b->right_son = a;
        if (betta != nullptr) {
            betta->parent = a;
        }
        b->parent = a->parent;
        a->parent = b;
        if (b->parent->left_son == a) {
            b->parent->left_son = b; 
        } else {
            b->parent->right_son = b;
        } 
    } 
    void Rightround(Node* t) {
        Node* a = t;
        Node* b = t->right_son;
        Node* betta = b->left_son;
        a->right_son = betta;
        b->left_son = a;
        if (betta != nullptr) {
            betta->parent = a;
        }
        b->parent = a->parent;
        a->parent = b;
        if (b->parent->left_son == a) {
            b->parent->left_son = b; 
        } else {
            b->parent->right_son = b;
        } 
    }
    void Spl(Node* x) {
        while (!Isroot(x)) {
            if (Isroot(x->parent)) {
                if (x->parent->left_son == x) {
                    Leftround(x->parent);
                } else {
                    Rightround(x->parent); 
                }
            } else if (Oneside(x)) {
                if (Onesideleft(x)) {
                    Node* g = x->parent->parent;
                    Node* f = x->parent;
                    Leftround(g);
                    Leftround(f);
                } else {
                    Node* g = x->parent->parent;
                    Node* f = x->parent;
                    Rightround(g);
                    Rightround(f);
                }
            } else {
                if (Othersideleft(x)) {
                    Node* g = x->parent->parent;
                    Node* f = x->parent; 
                    Rightround(f);
                    Leftround(g);
                } else {
                    Node* g = x->parent->parent;
                    Node* f = x->parent; 
                    Leftround(f);
                    Rightround(g);
                }
            }
        }
    }
public:
    Splay(): root(new Node()){}
    void Insert(int x) {
        if (size == 0) {
            root->left_son = new Node(x, root);
            ++size; 
            return;
        }
        if (Exist(x)) {
            return;
        }
        Node* cur_node = root->left_son;
        Node* place_node = cur_node;
        while (cur_node != nullptr) {
            place_node = cur_node;
            if ((cur_node->value) < x) {
                cur_node = cur_node->right_son;
            } else {
                cur_node = cur_node->left_son;
            }
        }
        cur_node = new Node(x, place_node);
        if (place_node->value > x) {
            place_node->left_son = cur_node;
        } else {
            place_node->right_son = cur_node;
        } 
        ++size;
        Spl(cur_node);
    }
    bool Exist(int x) {
        Node* cur = root->left_son;
        if (cur == nullptr) {
            return false;
        }
        int ret = INT_MAX;
        while (cur != nullptr) {
            if (cur->value >= x) {
                ret = std::min(ret, cur->value);
            }
            if (cur->value > x) {
                cur = cur->left_son;
            } else {
                cur = cur->right_son;
            }
        }

        return ret == INT_MAX ? false : true;
    }
    void Next(int x) {
        Node* cur = root->left_son;
        if (cur == nullptr) {
            std::cout << "none\n";
        }
        int ret = INT_MAX;
        Node* last_cur = nullptr;
        while (cur != nullptr) {
            last_cur = cur;
            if (cur->value > x) {
                ret = std::min(ret, cur->value);
            }
            if (cur->value > x) {
                cur = cur->left_son;
            } else {
                cur = cur->right_son;
            }
        }
        if (ret == INT_MAX) {
            std::cout << "none\n";
        } else {
            std::cout << ret << '\n';
            Spl(last_cur);
        }
    }
    void Prev(int x) {
        Node* cur = root->left_son;
        if (cur == nullptr) {
            std::cout << "none\n";
        }
        int ret = INT_MIN;
        Node* last_cur = nullptr;
        while (cur != nullptr) {
            last_cur = cur;
            if (cur->value < x) {
                ret = std::max(ret, cur->value);
            }
            if (cur->value < x) {
                cur = cur->right_son;
            } else {
                cur = cur->left_son;
            }
        }
        if (ret == INT_MIN) {
            std::cout << "none\n";
        } else {
            std::cout << ret << '\n';
            Spl(last_cur);
        }

    }
    void Kth_helper(Node* x, int& t, int k) {
        if (x->left_son == nullptr) {
            if (t == k) {
                std::cout << x->value << '\n';
                return;
            }
            ++t;
            if (x->right_son != nullptr) {
                Kth_helper(x->right_son, t, k);
            }
            return;
        } else {
            Kth_helper(x->left_son, t, k);
        }
    }
    void Kth(int k) {
        int tmp = 0;
        Kth_helper(root->left_son, tmp, k);
    }
    Node* Getson(Node* x) {
        if (x->left_son != nullptr) {
            return x->left_son;
        }
        return x->right_son;
    }
    void Delete(int x) {
        if (!Exist(x)) {
            return;
        }
        Node* cur = root->left_son;
        while(cur->value != x) {
            if (cur->value > x) {
                cur = cur->left_son;
            } else {
                cur = cur->right_son;
            }
        }
        if (cur->right_son == nullptr && cur->left_son == nullptr) {
            delete cur;
        } else if (cur->left_son == nullptr || cur->right_son == nullptr) {
            Node* s = Getson(cur);
            s->parent = cur->parent;
            if (s->parent->left_son == cur) {
               s->parent->left_son = s; 
            } else {
               s->parent->right_son = s;  
            }
            delete cur;
        } else {
            Node* s = cur->right_son;
            while (s->left_son != nullptr) {
                s = s->left_son;
            }
            s->parent->left_son = nullptr;
            s->parent = cur->parent;
            if (s->parent->left_son == cur) {
               s->parent->left_son = s; 
            } else {
               s->parent->right_son = s;  
            }
            delete cur;
        }
        --size;
    }   
    ~Splay() {
        delete root;
    }
};
int main() {
    std::string s;
    std::string command;
    int x;
    Splay tree;
    while (std::cin >> command >> x) {
        if (command == "insert") {
            tree.Insert(x);
        } else if (command == "exists") {
            tree.Exist(x);
        } else if (command == "next") {
            tree.Next(x);
        } else if (command == "prev") {
            tree.Prev(x);
        } else if (command == "delete") {
            tree.Delete(x);
        } else if (command == "kth") {
            tree.Kth(x);
        }
    }
}
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <climits>
const int cBig = 1e9;
struct Node {
    int value_ = -1;
    int height_;
    Node* right_son;
    Node* left_son;
    Node* papa;
    Node(): value_(-1), height_(-1), right_son(nullptr), left_son(nullptr), papa(nullptr){}
    Node(int x): value_(x), height_(0), right_son(nullptr), left_son(nullptr), papa(nullptr){}
    Node(int x, Node* p): value_(x), height_(0), right_son(nullptr), left_son(nullptr), papa(p){}
    ~Node() {
        delete left_son;
        delete right_son;
    }
};

class Tree {
    Node* root;
    int size_ = 0;
    int Height_helper(Node* t) {
       return t == nullptr ? 0 : t->height_ + 1;
    }
    void Height(Node* t) {
        t->height_ = std::max(Height_helper(t->left_son), Height_helper(t->right_son));
    }
    int Delta_helper(Node* t) {
        return t == nullptr ? 0 : t->height_ + 1; 
    }
    int Delta(Node* t) {
        return Delta_helper(t->left_son) - Delta_helper(t->right_son);
    }
    void Leftround(Node* t) {
        Node* a = t;
        Node* b = t->left_son;
        Node* betta = b->right_son;
        a->left_son = betta;
        Height(a);
        b->right_son = a;
        Height(b);
        if (betta != nullptr) {
            betta->papa = a;
        }
        b->papa = a->papa;
        a->papa = b;
        if (b->papa->left_son == a) {
            b->papa->left_son = b; 
        } else {
            b->papa->right_son = b;
        } 
    } 
    void Rightround(Node* t) {
        Node* a = t;
        Node* b = t->right_son;
        Node* betta = b->left_son;
        a->right_son = betta;
        Height(a);
        b->left_son = a;
        Height(b);
        if (betta != nullptr) {
            betta->papa = a;
        }
        b->papa = a->papa;
        a->papa = b;
        if (b->papa->left_son == a) {
            b->papa->left_son = b; 
        } else {
            b->papa->right_son = b;
        } 
    }
    void Rightroundh(Node* t) {
        Node* right = t->right_son;
        Leftround(right);
        Height(t);
        Rightround(t);
    }
    void Leftroundh(Node* t) {
        Node* left = t->left_son;
        Rightround(left);
        Height(t);
        Leftround(t);
    }
    void Balance(Node* t) {
        if (t == root) {
            return;
        }
        Height(t);
        int delt = Delta(t);
        if (delt == -2) {
            if (Delta(t->right_son) == 0 || Delta(t->right_son) == -1) {
                Rightround(t);
            } else {
                Rightroundh(t);
            }
        } else if (delt == 2) {
            if (Delta(t->left_son) == 0 || Delta(t->left_son) == 1) {
                Leftround(t);
            } else {
                Leftroundh(t);
            } 
        } 
        Balance(t->papa);
    }
public:
    Tree():root(new Node()){}

    int Lower(int x) {
        Node* cur = root->left_son;
        if (cur == nullptr) {
            return -1;
        }
        int ret = INT_MAX;
        while (cur != nullptr) {
            if (cur->value_ >= x) {
                ret = std::min(ret, cur->value_);
            }
            if (cur->value_ > x) {
                cur = cur->left_son;
            } else {
                cur = cur->right_son;
            }
        }
        return ret == INT_MAX ? -1 : ret;
    }
    void Insert(int x) {
        if (size_ == 0) {
            root->left_son = new Node(x, root);
            ++size_; 
            return;
        }
        if (Lower(x) == x) {
            return;
        }
        Node* cur_node = root->left_son;
        Node* place_node = cur_node;
        while (cur_node != nullptr) {
            place_node = cur_node;
            if ((cur_node->value_) < x) {
                cur_node = cur_node->right_son;
            } else {
                cur_node = cur_node->left_son;
            }
        }
        cur_node = new Node(x, place_node);
        if (place_node->value_ > x) {
            place_node->left_son = cur_node;
        } else {
            place_node->right_son = cur_node;
        } 
        ++size_; 
        Balance(place_node);
    }
    ~Tree() {
        delete root;
    }
};

int main() {
    Tree avl;
    int q;
    std::cin >> q;
    int last_value = -1;
    for (int i =0; i < q; ++i) {
        char c;
        int t;
        std::cin >> c >> t;
        if (c == '+') {
            if (last_value != -1) {
                t = (t + last_value) % cBig;
            }
            avl.Insert(t);
            last_value = -1;
        } else {
            last_value = avl.Lower(t);
            std::cout << last_value << '\n';
        }
    }
}
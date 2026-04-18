#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm> 
#include <unordered_set>

class Earley {
    
    struct Rule {
        int id; // left side id
        std::vector<int> rrs; // id sequence of right side
    }; 

    struct Item {
        int id;
        int dot_pos;
        int start_pos; 
        bool operator==(const Item& other) const {
            return id == other.id && 
                   dot_pos == other.dot_pos && start_pos == other.start_pos;
        }
    };

    struct ItemHash {
        size_t operator()(const Item& item) const { 
            const size_t p = 1000003;  
            size_t h = 0; 
            h = h * p + item.id;
            h = h * p + item.dot_pos;
            h = h * p + item.start_pos; 
            return h;
        }
    };

    std::unordered_map<char, int> symbol_int;

    int nt_size; // first nt - non terminals
    int t_size; // next - terminals
    int r_size; // rules size
    int start_id;

    std::vector<std::vector<int>> nt_rules; // id -> all_rules
    std::vector<Rule> rules;   

    void error_() {
        throw std::runtime_error("Grammar error");
    }   

    int get_id(char c) {
        if (symbol_int.find(c) == symbol_int.end()) {
            error_();
        }
        return symbol_int[c];
    }

public:

    void fit() {
        std::string in_; 
        std::cin >> nt_size >> t_size >> r_size >> in_;

        nt_rules.resize(nt_size);
        rules.resize(r_size); 
        int id_cnt = 0;
        for (char c: in_) {
            if (symbol_int.find(c) == symbol_int.end()) {
                symbol_int[c] = id_cnt++;
            }
        } 
        std::cin >> in_;
        for (char c: in_) {
            if (symbol_int.find(c) == symbol_int.end()) {
                symbol_int[c] = id_cnt++;
            }
        } 
        std::getline(std::cin, in_); // get \n from prev 
        for (int i = 0; i < r_size; ++i) {
            std::getline(std::cin, in_);
            in_.erase(std::remove(in_.begin(), in_.end(), ' '), in_.end());
            
            size_t arrow_pos = in_.find("->");

            std::string lrs = in_.substr(0, arrow_pos); 
            std::string rrs = in_.substr(arrow_pos + 2); 
            
            if ((lrs.size() != 1) || (symbol_int.find(lrs.front()) == symbol_int.end()) || (!isupper(lrs[0]))) {
                error_();
            }   
            rules[i].id = get_id((char)lrs[0]);
            for (char c: rrs) {
                if (symbol_int.find(c) == symbol_int.end()) {
                    error_();
                } 
                rules[i].rrs.push_back(get_id(c));
            } 
            nt_rules[rules[i].id].push_back(i);
        } 
        char st;
        std::cin >> st;
        start_id = get_id(st);
    } 
    void add_state(std::vector<Item>& d_vec, std::unordered_set<Item, ItemHash>& d_hash, const Item& item) {
        if (d_hash.find(item) == d_hash.end()) {
            d_hash.insert(item);
            d_vec.push_back(item);
        }
    }
    void scan_step(const Item& item, int i, const std::string& w, std::vector<std::vector<Item>>& D, 
                   std::vector<std::unordered_set<Item, ItemHash>>& D_hash) {
        if (i >= w.size()) return; 
        int next_sym_id = rules[item.id].rrs[item.dot_pos]; 
        if (symbol_int.find(w[i]) == symbol_int.end()) return; 
        int char_id = symbol_int.at(w[i]);
        if (next_sym_id == char_id) {
            Item new_item = {item.id, item.dot_pos + 1, item.start_pos};
            add_state(D[i + 1], D_hash[i + 1], new_item);
        }
    }

    void predict_step(const Item& item, int i, std::vector<std::vector<Item>>& D, 
                      std::vector<std::unordered_set<Item, ItemHash>>& D_hash) {
        int next_sym_id = rules[item.id].rrs[item.dot_pos]; 
        if (next_sym_id < nt_rules.size()) {
            for (int rule_i : nt_rules[next_sym_id]) {
                Item new_item = {rule_i, 0, i};
                add_state(D[i], D_hash[i], new_item);
            }
        }
    }

    void complete_step(const Item& item, int i, std::vector<std::vector<Item>>& D, 
                       std::vector<std::unordered_set<Item, ItemHash>>& D_hash) {
        int k = item.start_pos;
        int lrs_id = rules[item.id].id; 
        for (size_t p = 0; p < D[k].size(); ++p) {
            Item parent_item = D[k][p];  
            if (parent_item.dot_pos < rules[parent_item.id].rrs.size()) {
                int next_symb = rules[parent_item.id].rrs[parent_item.dot_pos]; 
                if (next_symb == lrs_id) {
                    Item new_item = {parent_item.id, parent_item.dot_pos + 1, parent_item.start_pos};
                    add_state(D[i], D_hash[i], new_item);
                }
            }
        }
    }
 
    bool predict(std::string w) { 
        int w_size = (int)w.size();
        std::vector<std::vector<Item>> D(w_size + 1);
        std::vector<std::unordered_set<Item, ItemHash>> D_hash(w_size + 1);
        for (int rule_idx : nt_rules[start_id]) {
            Item new_item = {rule_idx, 0, 0};
            add_state(D[0], D_hash[0], new_item);
        }
        for (int i = 0; i <= w_size; ++i) {
            std::vector<bool> completed_eps(nt_size, false);
            for (int j = 0; j < D[i].size(); ++j) {
                Item current = D[i][j];
                if (current.dot_pos == rules[current.id].rrs.size()) {
                    if (current.start_pos == i) {
                        completed_eps[rules[current.id].id] = true;
                    }
                    complete_step(current, i, D, D_hash);  
                }
                else {
                    int next_sym_id = rules[current.id].rrs[current.dot_pos];
                    if (next_sym_id < nt_size) {
                        predict_step(current, i, D, D_hash); 
                        if (completed_eps[next_sym_id]) {
                            Item new_item = {current.id, current.dot_pos + 1, current.start_pos};
                            add_state(D[i], D_hash[i], new_item);
                        }
                    }
                    else {
                        scan_step(current, i, w, D, D_hash);  
                    }
                }
            }
        }  
        for (const auto& item : D[w_size]) {
            if (item.start_pos == 0 && rules[item.id].id == start_id && 
                item.dot_pos == rules[item.id].rrs.size()) {
                return true;
            }
        }
        return false;
    }
};

int main() { 
    Earley kek;
    kek.fit();
    int m;
    std::cin >> m; 
    std::string w;
    std::getline(std::cin, w);
    for (int i = 0; i < m; ++i) {
        std::getline(std::cin, w);
        if (kek.predict(w)) {
            std::cout << "Yes\n";
        } else {
            std::cout << "No\n";
        }
    }  
}
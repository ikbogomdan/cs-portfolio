#ifndef CYK_HPP
#define CYK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <queue>

class CYK_Algorithm {
private:
    struct Symbol {
        bool is_t;
        int id; // nonterminal <-> int
        char symbol; // terminal - asci
        Symbol(): is_t(true), id(-1), symbol('\0'){}
        Symbol(bool b, int i, char s): is_t(b), id(i), symbol(s) {}
        static Symbol NT(int num) { return Symbol(false, num, 0); }
        static Symbol T(char c) { return Symbol(true, -1, c); }
        bool Term() const { return is_t; }


        bool operator==(const Symbol& other) const { 
            if (is_t != other.is_t) {
                return false;
            } 
            if (is_t) {
                return symbol == other.symbol;
            } 
            return id == other.id;
        }
        bool operator!=(const Symbol& other) const {
            return !(*this == other);
        }
         
    }; 
    std::map<std::string, int> nt_int;
    std::vector<std::string> nt_str;
    int gramar_start_nt; 
    std::vector<std::vector<std::vector<Symbol>>> rules;  
    Symbol create_symbol(char c) { 
        if (isupper(c)) { 
            return Symbol::NT(nt_int[std::string(1, c)]); 
        } 
        return Symbol::T(c);
    } 
    void error_() {
        throw std::runtime_error("Grammar error");
    }   
    int last_id() {
        return static_cast<int>(rules.size());
    }

    std::string get_new_name() {
        static int counter = 0;
        return "@" + std::to_string(counter++);
    } 
    int create_new_nt() {
        std::string name = get_new_name();
        int id = rules.size();
        nt_str.push_back(name);
        nt_int[name] = id;
        rules.resize(id + 1);
        return id;
    }
public: 

    void read_grammar() {
        int nt_size;
        int t_size;
        int p_size;
        std::cin >> nt_size >> t_size >> p_size;

        rules.resize(nt_size); 
        std::set<char> validation;
        std::string input_;

        std::cin >> input_;
        for (char c : input_) {
            std::string s(1, c);
            if (nt_int.find(s) == nt_int.end()) {
                int new_id = nt_str.size();
                nt_int[s] = new_id;
                nt_str.push_back(s);
            }
            validation.insert(c);
        }
        std::cin >> input_;
        for (auto x: input_) validation.insert(x);
        std::getline(std::cin, input_); // next \n before getline

        for (int i = 0; i < p_size; ++i) { 
            std::getline(std::cin, input_);
            input_.erase(std::remove(input_.begin(), input_.end(), ' '), input_.end());
            int arrow_pos = input_.find("->");
            std::string lrs = input_.substr(0, arrow_pos); // left rule side
            std::string rrs = input_.substr(arrow_pos + 2); // right rule side

            if ((lrs.size() != 1) || (validation.find(lrs.front()) == validation.end()) || (!isupper(lrs[0]))) {
                error_();
            }
            Symbol nt = create_symbol(lrs[0]); 
            int nt_pos = nt.id;
            
            std::vector<Symbol> new_rule; 
            for (char c: rrs) {
                if (validation.find(c) == validation.end()) {
                    error_();
                } 
                new_rule.push_back(create_symbol(c));
            } 
            rules[nt_pos].push_back(new_rule);  
        } 
        char start_nt;
        std::cin >> start_nt; 
        gramar_start_nt = nt_int[std::string(1, start_nt)];
    } 


    void CNF_useless_helper(std::set<int>& useless) { 
        for (int i = 0; i < rules.size(); ++i) {
            if (useless.find(i) != useless.end()) {
                rules[i].clear();
                continue;
            }
            for (int j = 0; j < rules[i].size(); ++j) {
                bool is_rem = false;
                for (Symbol c: rules[i][j]) {
                    if (!c.is_t && (useless.find(c.id) != useless.end())) {
                        is_rem = true;
                        break;
                    }
                }
                if (is_rem) {
                    rules[i].erase(rules[i].begin() + j);
                    --j;
                }
            }
        } 
    }

    void CNF_useless() { 
        std::set<int> useless; 
        for (int i = 0; i < nt_str.size(); ++i) {
            useless.insert(i);
        }  
        while (true) {
            int before_size = useless.size(); 
            std::queue<int> valid;
            for (int i: useless) {  
                bool gj_rule = false;   
                for (int j = 0; j < rules[i].size(); ++j) {
                    bool rule_in_rule = false;   
                    for (Symbol c: rules[i][j]) {  
                        if (!c.is_t && useless.find(c.id) != useless.end()) {
                            rule_in_rule = true;
                            break;
                        }
                    } 
                    if (!rule_in_rule) { 
                        gj_rule = true;
                        break;  
                    }
                } 
                
                if (gj_rule) {
                    valid.push(i);
                }
            } 
            while (!valid.empty()) {
                int id = valid.front();  
                valid.pop();            
                useless.erase(id);
            }
            if (useless.size() == before_size) {
                break;
            }
        }  
        CNF_useless_helper(useless); 
        std::queue<int> reachable; 
        std::set<int> valid; 
        reachable.push(gramar_start_nt); 
        valid.insert(gramar_start_nt);
        while (!reachable.empty()) { 
            int x = reachable.front(); 
            reachable.pop();
            for (int i = 0; i < rules[x].size(); ++i) {
                for (Symbol c: rules[x][i]) {
                    if (c.is_t) {
                        continue;
                    }
                    if (valid.find(c.id) != valid.end()) {
                        continue;
                    }
                    valid.insert(c.id);  
                    reachable.push(c.id);
                }
            } 
        } 
        for (int i = 0; i < rules.size(); ++i) {
            if (valid.find(i) == valid.end()) {
                rules[i].clear();
            }
        } 
    }

    void CNF_start() {
        int new_elem = last_id(); 
        std::string new_name = "S'";  
        nt_str.push_back(new_name);
        nt_int[new_name] = new_elem;
        std::vector<Symbol> new_rule = {Symbol::NT(gramar_start_nt)};
        rules.resize(new_elem + 1);
        rules[new_elem].push_back(new_rule); 
        gramar_start_nt = new_elem;
    }

    void CNF_mixed() {  
        std::map<char, int> term_to_nt_id;  
        int original_size = rules.size();  
        for (int i = 0; i < original_size; ++i) {
            for (int j = 0; j < rules[i].size(); ++j) { 
                if (rules[i][j].size() == 1) continue; 
                for (int t = 0; t < rules[i][j].size(); ++t) {
                    Symbol c = rules[i][j][t]; 
                    if (c.is_t) {
                        int id; 
                        if (term_to_nt_id.find(c.symbol) != term_to_nt_id.end()) {
                            id = term_to_nt_id[c.symbol];
                        } else { 
                            std::string new_name = std::string(1, c.symbol) + "'";
                            int new_elem = last_id();   
                            nt_str.push_back(new_name);
                            nt_int[new_name] = new_elem; 
                            term_to_nt_id[c.symbol] = new_elem; 
                            std::vector<Symbol> new_rule = {Symbol::T(c.symbol)};
                            rules.resize(new_elem + 1);
                            rules[new_elem].push_back(new_rule);
                            
                            id = new_elem;
                        }  
                        rules[i][j][t] = Symbol::NT(id); 
                    }
                }
            }
        } 
    }

    void CNF_long() {
        bool changed = true;
        while (changed) {
            changed = false;
            int current_size = rules.size();  
            for (int i = 0; i < current_size; ++i) {
                std::vector<std::vector<Symbol>> new_rules_for_i; 
                for (int j = 0; j < rules[i].size(); ++j) { 
                    if (rules[i][j].size() > 2) {
                        changed = true; 
                        int new_nt_id = create_new_nt(); 
                        std::vector<Symbol> tail;
                        for (int k = 1; k < rules[i][j].size(); ++k) {
                            tail.push_back(rules[i][j][k]);
                        }
                        rules[new_nt_id].push_back(tail); 
                        std::vector<Symbol> short_rule;
                        short_rule.push_back(rules[i][j][0]);      
                        short_rule.push_back(Symbol::NT(new_nt_id));  
                        new_rules_for_i.push_back(short_rule);
                    } else { 
                        new_rules_for_i.push_back(rules[i][j]);
                    }
                }
                rules[i] = new_rules_for_i;
            }
        }
    }

    void CNF_eps() {
        int n = rules.size();
        std::vector<bool> is_nullable(n, false); 
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < n; ++i) {
                if (is_nullable[i]) continue;

                for (int j = 0; j < rules[i].size(); ++j) { 
                    bool all_nullable = true; 
                    for (int k = 0; k < rules[i][j].size(); ++k) {
                        Symbol sym = rules[i][j][k];
                        if (sym.is_t) {
                            all_nullable = false;
                            break;
                        }
                        if (!is_nullable[sym.id]) {
                            all_nullable = false;
                            break;
                        }
                    }

                    if (all_nullable) {
                        is_nullable[i] = true;
                        changed = true;
                        break;
                    }
                }
            }
        } 
        changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < n; ++i) {
                std::vector<std::vector<Symbol>> to_add; 
                for (int j = 0; j < rules[i].size(); ++j) { 
                    for (int k = 0; k < rules[i][j].size(); ++k) {
                        Symbol sym = rules[i][j][k]; 
                        if (!sym.is_t && is_nullable[sym.id]) { 
                            std::vector<Symbol> new_rule;
                            for (int m = 0; m < rules[i][j].size(); ++m) {
                                if (m == k) continue; 
                                new_rule.push_back(rules[i][j][m]);
                            } 
                            bool exists = false;
                            for (int r = 0; r < rules[i].size(); ++r) {
                                if (rules[i][r] == new_rule) {
                                    exists = true;
                                    break;
                                }
                            } 
                            for (int r = 0; r < to_add.size(); ++r) {
                                if (to_add[r] == new_rule) {
                                    exists = true; 
                                    break;
                                }
                            } 
                            if (!exists) {
                                to_add.push_back(new_rule);
                                changed = true;
                            }
                        }
                    }
                } 
                for (int k = 0; k < to_add.size(); ++k) {
                    rules[i].push_back(to_add[k]);
                }
            }
        }
        
        for (int i = 0; i < n; ++i) { 
            if (i == gramar_start_nt) continue; 
            std::vector<std::vector<Symbol>> clean_rules;
            for (int j = 0; j < rules[i].size(); ++j) {
                if (rules[i][j].size() > 0) {
                    clean_rules.push_back(rules[i][j]);
                }
            }
            rules[i] = clean_rules;
        }
    }

    void CNF_chain() {
        int n = rules.size(); 
        std::vector<std::set<int>> reachable(n); 
        for (int i = 0; i < n; ++i) { 
            std::queue<int> q;
            q.push(i);  
            while (!q.empty()) {
                int u = q.front();
                q.pop(); 
                for (int j = 0; j < rules[u].size(); ++j) { 
                    if (rules[u][j].size() == 1 && !rules[u][j][0].is_t) {
                        int v = rules[u][j][0].id;
                        if (reachable[i].find(v) == reachable[i].end()) {
                            reachable[i].insert(v);
                            q.push(v);
                        }
                    }
                }
            }
        } 
        std::vector<std::vector<std::vector<Symbol>>> new_rules = rules; 
        for (int i = 0; i < n; ++i) { 
            for (int b : reachable[i]) { 
                for (int k = 0; k < rules[b].size(); ++k) { 
                    bool is_chain = (rules[b][k].size() == 1 && !rules[b][k][0].is_t);
                    
                    if (!is_chain) { 
                        bool exists = false;
                        for (int r = 0; r < new_rules[i].size(); ++r) {
                            if (new_rules[i][r] == rules[b][k]) {
                                exists = true; break;
                            }
                        }
                        if (!exists) {
                            new_rules[i].push_back(rules[b][k]);
                        }
                    }
                }
            }
        }
        
        rules = new_rules;
 
        for (int i = 0; i < n; ++i) {
            std::vector<std::vector<Symbol>> clean;
            for (int j = 0; j < rules[i].size(); ++j) { 
                bool is_chain = (rules[i][j].size() == 1 && !rules[i][j][0].is_t);
                if (!is_chain) {
                    clean.push_back(rules[i][j]);
                }
            }
            rules[i] = clean;
        }
    }

    void CNF() {
        CNF_useless(); 
        CNF_start();
        CNF_mixed();
        CNF_long();
        CNF_eps();
        CNF_chain(); 
    } 
    
    void fit() {
        CNF();
    }
    
    bool predict(const std::string& word) {
        int n = word.length(); 
        if (n == 0) { 
            for (const auto& rule : rules[gramar_start_nt]) {
                if (rule.empty()) return true;
            }
            return false;
        } 
        int num_nt = nt_str.size(); 
        std::vector<std::vector<std::vector<bool>>> d(
            n, std::vector<std::vector<bool>>(n, std::vector<bool>(num_nt, false))
        ); 
        for (int i = 0; i < n; ++i) { 
            for (int A = 0; A < num_nt; ++A) {
                for (const auto& rule : rules[A]) { 
                    if (rule.size() == 1 && rule[0].is_t) { 
                        if (rule[0].symbol == word[i]) { 
                            d[i][i][A] = true;
                        }
                    }
                }
            }
        } 
        for (int l = 1; l < n; ++l) { 
            for (int i = 0; i < n - l; ++i) {
                int end = i + l; 
                for (int j = i; j < end; ++j) { 
                    for (int A = 0; A < num_nt; ++A) {
                        for (const auto& rule : rules[A]) { 
                            if (rule.size() == 2 && !rule[0].is_t && !rule[1].is_t) {
                                int B = rule[0].id;
                                int C = rule[1].id; 
                                if (d[i][j][B] && d[j + 1][end][C]) {
                                    d[i][end][A] = true;
                                }
                            }
                        }
                    }
                }
            }
        }  
        return d[0][n - 1][gramar_start_nt];
    }
};

#endif

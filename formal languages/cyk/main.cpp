#include "cyk.hpp"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL); 
    CYK_Algorithm solve;
    solve.read_grammar();
    solve.fit(); 
    int m;
    std::cin >> m; 

    std::string garbage;
    std::getline(std::cin, garbage);
    
    for (int i = 0; i < m; ++i) {
        std::string w; 
        std::getline(std::cin, w);
        if (solve.predict(w)) {
            std::cout << "Yes\n";
        } else {
            std::cout << "No\n";
        }
    }  
}

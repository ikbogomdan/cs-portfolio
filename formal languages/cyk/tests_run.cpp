#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "cyk.hpp" 

void run_test_file(int test_num) {
    std::string filename = "tests/test" + std::to_string(test_num) + ".txt";
    std::ifstream file(filename); 

    std::cout << "=== Test " << test_num  << std::endl; 
    std::streambuf* cin_backup = std::cin.rdbuf(); 
    std::cin.rdbuf(file.rdbuf());

    CYK_Algorithm algo; 
    algo.read_grammar();
    algo.fit(); 
    int m;
    if (std::cin >> m) {
        std::string dummy;
        std::getline(std::cin, dummy);  
        for (int i = 0; i < m; ++i) {
            std::string w;
            std::getline(std::cin, w);  
            std::cout << "Word '" << w << "': ";
            if (algo.predict(w)) {
                std::cout << "Yes" << std::endl;
            } else {
                std::cout << "No" << std::endl;
            }
        }
    }
    
    std::cin.rdbuf(cin_backup);
    
    file.close();
    std::cout << "--------------------------------\n" << std::endl;
}

int main() { 
    for (int i = 1; i <= 8; ++i) {
        run_test_file(i);
    }
    return 0;
}
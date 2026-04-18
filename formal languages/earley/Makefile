all:
	g++ -O2 -std=c++17 main.cpp -o earley

test: all
	python3 run_tests.py

clean:
	rm -f earley tests/*.out
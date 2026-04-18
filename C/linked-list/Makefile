all: linked_list
	timeout 5s python3 -u test_linked_list.py ./linked_list

linked_list: linked_list.h linked_list.c main.c
	gcc linked_list.c main.c -o linked_list
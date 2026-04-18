#ifndef LINKED_LIST_H
#define LINKED_LIST_H


struct ListNode {
    struct ListNode *next;
    char country_code[2];
    int part_number;
};


struct ListNode* read_linked_list(const char*);


#endif  // LINKED_LIST_H

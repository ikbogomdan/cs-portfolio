#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"


void print_then_free_ll(struct ListNode *head) {
    struct ListNode *next;

    while (head != NULL) {
        next = head->next;

        printf(
            "%c%c %d\n",
            head->country_code[0], head->country_code[1],
            head->part_number
        );

        free(head);
        head = next;
    }
}


int main(int argc, const char **argv) {
    if (argc != 2) {
        printf("Usage: %s <path_to_file>\n", argv[0]);
        return -1;
    }

    struct ListNode *ll = read_linked_list(argv[1]);

    print_then_free_ll(ll);

    return 0;
}

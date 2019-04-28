#include <stdlib.h>

#include "linked_list.h"

linked_list_t *create_linked_list(void *value, linked_list_t *prev, linked_list_t *next) {
    //we want to put the list element on the heap
    linked_list_t *list = malloc(sizeof(linked_list_t));

    //set all the fields
    list->value = value;
    list->prev = prev;
    list->next = next;

    return list;
}

void free_linked_list(linked_list_t *list, free_f free_value) {
    linked_list_t *cur = list;
    
    //free element in the list
    while (NULL != cur) {
        free_value(cur->value);

		//hold onto this element so we can free it
		linked_list_t *temp = cur;
		//move on
		cur = cur->next;
		//and free the one we just left behind
		free(temp);
    }
}

linked_list_t * find_in_linked_list(linked_list_t *head, void *value, compare_f compare) {
    for (linked_list_t *cur = head; NULL != cur; cur = cur->next) {
        //if cur's value equals the given value, we found what we're looking for
        if (0 == compare(value, cur->value)) {
            return cur;
        }
    }

    //if we didn't find anything which match value, return NULL
    return NULL;
}

linked_list_t * add_to_linked_list(linked_list_t *head, void *value) {
    //make a new list el to hold value, and make it the new head
    linked_list_t *new_head = create_linked_list(value, NULL, head);
    
    if (NULL != head) {
        head->prev = new_head;
    }

    return new_head;
}

linked_list_t * append_to_linked_list(linked_list_t *head, void *value) {
    //if we don't have a list yet, make this the head
    if (NULL == head) {
        return create_linked_list(value, NULL, NULL);
    }

    //otherwise, find the curent tail
    linked_list_t *cur;
    for (cur = head; NULL != cur->next; cur = cur->next) {}


    //...and put our new node after it, as the new tail
    linked_list_t *new_el = create_linked_list(value, cur, NULL); 
    cur->next = new_el;

    return head;
}

linked_list_t * remove_from_linked_list(linked_list_t *head, 
                                        void *value, 
                                        compare_f compare,
                                        free_f free_value) {
    linked_list_t *el = find_in_linked_list(head, value, compare);

    //if we found the value in the list, take it out
    if (NULL != el) {
        //if the value occurs at the head of the list, move it down to the next element
        if (head == el) {
            head = el->next;
        }

        //make the prior element skip over this one (with its forward pointer)
        if (NULL != el->prev) {
            el->prev->next = el->next;
        }

        //make the next element skip over this one (with its backwards pointer)
        if (NULL != el->next) {
            el->next->prev = el->prev;
        }

        //free all allocated memory
        free_value(el->value);
        free(el);
    }

    //return the new head
    return head;
}

//NOTE: does not preserve order
linked_list_t * copy_linked_list(linked_list_t *list, copy_f copy_value) {
    linked_list_t *copy = NULL;
    for (list = list; NULL != list; list = list->next) {
        copy = add_to_linked_list(copy, copy_value(list->value));
    }

    return copy;
}

linked_list_t * diff_linked_lists(linked_list_t *list_0, linked_list_t *list_1, 
                                  compare_f compare, copy_f copy_value, free_f free_value) {
    linked_list_t *diff = copy_linked_list(list_0, copy_value);

    //remove any element in list_1 from diff
    for (list_1 = list_1; NULL != list_1; list_1 = list_1->next) {
        diff = remove_from_linked_list(diff, list_1->value, compare, free_value);
    }

    return diff;
}

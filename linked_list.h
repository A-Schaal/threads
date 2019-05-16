#ifndef LINKED_LIST
    #define LINKED_LIST

    typedef struct linked_list {
        void *value;
        struct linked_list *prev;
        struct linked_list *next;
    } linked_list_t;

    //function that compares two values; should return 0 if they are equal,
    //and optionally impose a partial order on the values
    typedef int (*compare_f) (void *value_0, void *value_1);

    //should free the given value
    typedef void (*free_f) (void *value);

    //should return an allocated copy of the given value
    typedef void * (*copy_f) (void *value);

    linked_list_t *create_linked_list(void *value, linked_list_t *prev, linked_list_t *next);
    void free_linked_list(linked_list_t *list, free_f free_value);
    linked_list_t * find_in_linked_list(linked_list_t *head, void *value, compare_f compare);
    linked_list_t * add_to_linked_list(linked_list_t *head, void *value);
    linked_list_t * append_to_linked_list(linked_list_t *tail, void *value);
    linked_list_t * remove_from_linked_list(linked_list_t *head, 
                                            void *value, 
                                            compare_f compare,
                                            free_f free_value);
    linked_list_t * copy_linked_list(linked_list_t *list, copy_f copy_value);
    linked_list_t * diff_linked_lists(linked_list_t *list_0, linked_list_t *list_1, 
                                      compare_f compare, copy_f copy_value, free_f free_value);

    //helper functions which implement common compare_f and free_f behaviors

    //basic free_f:
    void free_nothing(void *value);
    //the regular free function from stdlib.h also can be used

    //basic compare_f:
    int compare_pointer(void *pointer_0, void *pointer_1);
#endif

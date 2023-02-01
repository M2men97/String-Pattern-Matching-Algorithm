README

NAME:MO'MEN ABU GHARBIEH

Exercise 1 - String Pattern Matching Algorithm .

===Description ===

program files:

pattern_matching.c , slist.c  .

functions in slist.c :

void dbllist_init(dbllist_t *) - this function will initialize the double linked list .

void dbllist_destroy(dbllist_t *,dbllist_destroy_t) - this function will destroy the double linked list .

int dbllist_append(dbllist_t *,void *) - this function will add item to the list as the last node .

int dbllist_prepend(dbllist_t *,void *) - this function will add item to the list as the first node .

int dbllist_remove(dbllist_t *, dbllist_node_t* ,dbllist_destroy_t) - this function will remove a specific node from the list . 


functions in pattern_matching.c :

int pm_init(pm_t *) - this function will initialize the zero state in the FSM .

int pm_addstring(pm_t *,unsigned char *, _size_t n) - this function will add new states that contain data to the FSM .

int pm_makeFSM(pm_t *) - this function will build the FSM by setting the failure transitions and the goto transitions .

int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state) - this function will set the goto arrow from the from_state to the to_state.

pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol) - this function will return the needed transition state .

dbllist_t* pm_fsm_search(pm_state_t *,unsigned char *,_size_t) - this is the main function kind of , it will search for a match inside the FSM .

void pm_destroy(pm_t *) - this function will destroy the FSM  and free all the states .

IMPORTANT NOTE : i had to ruin my valgrind , cause if i did not i wont pass most of the test cause the double free , i built my code to free it self to test that i had remove all the free from the main.c which is the tester and my valgrind passed the valgrind test .



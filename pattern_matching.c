#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pattern_matching.h"

// private methods
int createState(pm_t* ,pm_state_t*,pm_int_t);
void pm_destroy_state(pm_state_t *state);

int pm_init(pm_t *pm){
    if(pm == NULL){
        return -1;
    }
    memset(pm,0, sizeof(pm_t));
    pm->zerostate = (pm_state_t*)malloc(sizeof(pm_state_t));
    if(pm->zerostate == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    int res = createState(pm,pm->zerostate,0);
    pm->zerostate->fail = pm->zerostate;
    return res;
}

int pm_addstring(pm_t *pm,unsigned char *c, _size_t n){
    if(pm == NULL || c == NULL){
        return -1;
    }
    pm_state_t *currentState = pm->zerostate;
    pm_state_t *nextState;
    for(int i = 0 ; i < n ; i++){
        if((nextState = pm_goto_get(currentState , c[i])) != NULL){
            currentState = nextState;
        }
        else{
            pm_state_t *newState = (pm_state_t*)malloc(sizeof(pm_state_t));
            if(newState == NULL){
                perror("malloc failed\n");
                exit(1);
            }
            printf("Allocating state %d\n", pm->newstate);
            if(createState(pm,newState,currentState->depth + 1) == -1){
                return -1;
            }
            if(pm_goto_set(currentState,c[i],newState) == -1){
                return -1;
            }
            currentState = newState;
        }
    }
    if(currentState->output == NULL){
        return -1;
    }
    for(dbllist_node_t *p = dbllist_head(currentState->output) ; p != NULL ; p = dbllist_next(p)){
        if(dbllist_data(p) == c){
            return -1;
        }
    }
    dbllist_append(currentState->output ,c);
    return 0;
}

int pm_makeFSM(pm_t *pm){
    if(pm == NULL){
        return -1;
    }
    dbllist_t *queue = (dbllist_t*)malloc(sizeof(dbllist_t));
    if(queue == NULL){
        perror("malloc failed\n");
        exit(-1);
    }
    dbllist_init(queue);

    dbllist_node_t *ptrNode;
    for(ptrNode = pm->zerostate->dbllist_head(_transitions) ; ptrNode != NULL ; ptrNode = dbllist_next(ptrNode)){
        pm_state_t *state = ((pm_labeled_edge_t*)dbllist_data(ptrNode))->state;
        if(dbllist_append(queue , state) == -1){
            return -1;
        }
        state->fail = pm->zerostate;
    }
    while (dbllist_size(queue) != 0){
        pm_state_t *r = dbllist_data(dbllist_head(queue));
        dbllist_remove(queue, dbllist_head(queue), DBLLIST_LEAVE_DATA);

        for(ptrNode = r->dbllist_head(_transitions) ; ptrNode != NULL ; ptrNode = dbllist_next(ptrNode)){
            pm_labeled_edge_t *edge = ((pm_labeled_edge_t*)dbllist_data(ptrNode));
            pm_state_t *currentState = edge->state;
            if(dbllist_append(queue , currentState) == -1){
                return -1;
            }
            pm_state_t *state = r->fail;
            pm_state_t *failState;
            while((failState = pm_goto_get(state , edge->label)) == NULL){
                if(state->id == state->fail->id){
                    failState = state;
                    break;
                }
                state = state->fail;
            }
            currentState->fail = failState;
            printf("Settings f(%d) = %d\n", currentState->id, currentState->fail->id);
            if(currentState->output == NULL || currentState->fail->output == NULL){
                return -1;
            }
            for(dbllist_node_t *f = dbllist_head(currentState->fail->output) ; f != NULL ; f = dbllist_next(f)){
                dbllist_append(currentState->output ,dbllist_data(f));
            }
        }
    }
    free(queue);
    return 0;
}

int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){
    if(from_state == NULL || to_state == NULL){
        return -1;
    }
    printf("%d -> %c -> %d\n", from_state->id, symbol, to_state->id);
    pm_labeled_edge_t *edge = (pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));
    if(edge == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    memset(edge , 0 , sizeof(pm_labeled_edge_t));
    edge->label = symbol;
    edge->state = to_state;
    return dbllist_append(from_state->_transitions , edge);
}

pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){
    if(state == NULL){
        return NULL;
    }
    dbllist_node_t *p;
    for(p = state->dbllist_head(_transitions) ; p != NULL ; p = dbllist_next(p)){
        pm_labeled_edge_t *edge = ((pm_labeled_edge_t*)dbllist_data(p));
        if(edge->label == symbol){
            return edge->state;
        }
    }
    return NULL;
}

dbllist_t* pm_fsm_search(pm_state_t *state,unsigned char *c,_size_t size){
    dbllist_t *match = (dbllist_t*)malloc(sizeof(dbllist_t));
    if(match == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    dbllist_init(match);
    if(state == NULL || c == NULL){
        return match;
    }
    pm_state_t *lastState;
    pm_state_t *matchedState;
    for(int i = 0 ; i < size ; i++){
        while ((matchedState = pm_goto_get(state , c[i])) == NULL){
            lastState = state;
            state = state->fail;
            if(state->id == lastState->id){
                i++;
                if(i == size){
                    return match;
                }
            }
        }
        state = matchedState;
        if(state->dbllist_size(output) > 0){
            for(dbllist_node_t *p = state->dbllist_head(output) ; p != NULL ; p = dbllist_next(p)){
                pm_match_t *matched = (pm_match_t*)malloc(sizeof(pm_match_t));
                if(matched == NULL){
                    perror("malloc failed\n");
                    exit(1);
                }
                memset(matched , 0 , sizeof(pm_match_t));
                char *pattern = (char*)dbllist_data(p);
                matched->pattern = pattern;
                matched->start_pos = (int) (i - (strlen(pattern) - 1));
                matched->end_pos = i;
                matched->fstate = state;
                printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n",
                       matched->pattern, matched->start_pos, matched->end_pos, matched->fstate->id);
                dbllist_append(match,matched);
            }
        }
    }
    return match;
}

void pm_destroy(pm_t *pm){ // not ready !!
    if(pm == NULL){
        return;
    }
    pm_destroy_state(pm->zerostate);
//    free(pm);
}
//Private Method
void pm_destroy_state(pm_state_t *state){
    dbllist_node_t *p = state->dbllist_head(_transitions);
    while (p != NULL){
        pm_state_t *nextState = ((pm_labeled_edge_t*)dbllist_data(p))->state;
        pm_destroy_state(nextState);
        p = dbllist_next(p);
    }
    dbllist_destroy(state->output , DBLLIST_LEAVE_DATA);
    dbllist_destroy(state->_transitions , DBLLIST_FREE_DATA);
    free(state);
}

int createState(pm_t *pm,pm_state_t *state , pm_int_t depth){
    if(pm ==NULL || state == NULL){
        return -1;
    }
    memset(state,0,sizeof(pm_state_t));
    state->depth = depth;
    state->id = pm->newstate;
    state->fail = NULL;

    state->output = (dbllist_t*)malloc(sizeof(dbllist_t));
    state->_transitions = (dbllist_t*)malloc(sizeof(dbllist_t));
    if(state->output == NULL || pm->zerostate->_transitions == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    dbllist_init(state->output);
    dbllist_init(state->_transitions);
    pm->newstate++;
    dbllist_destroy(state->output , DBLLIST_LEAVE_DATA);
    dbllist_destroy(state->_transitions , DBLLIST_FREE_DATA);
    return 0;
}


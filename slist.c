#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "slist.h"


// Init Func

void dbllist_init(dbllist_t *list){
    if(list == NULL){
        return;
    }
    memset(list,0,sizeof(dbllist_t));
    dbllist_head(list) = NULL;
    dbllist_tail(list) = NULL;
    dbllist_size(list) = 0;
}

// Des Func

void dbllist_destroy(dbllist_t *list,dbllist_destroy_t dealloc){
    if(list == NULL)
        return;
    dbllist_node_t *prev , *node;
    node = dbllist_tail(list);
    while(node != NULL){
        prev = dbllist_prev(node);
        if(dealloc)
            free(dbllist_data(node));
        free(node);
        node = prev;
    }
}

// Add Func

int dbllist_append(dbllist_t *list,void *data){
    if(list == NULL || data == NULL)
        return -1;
    dbllist_node_t *node = (dbllist_node_t*)malloc(sizeof(dbllist_node_t));
    if(node == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    memset(node,0, sizeof(dbllist_node_t));
    dbllist_data(node) = data;
    dbllist_next(node) = NULL;
    dbllist_size(list)++;
    if(dbllist_head(list) == NULL){ // the list is empty
        dbllist_head(list) = node;
        dbllist_tail(list) = node;
        return 0;
    }
    dbllist_prev(node) = dbllist_tail(list);
    dbllist_next(dbllist_tail(list)) = node;
    dbllist_tail(list) = node;
    return 0;
}

int dbllist_prepend(dbllist_t *list,void *data){
    if(list == NULL || data == NULL)
        return -1;
    dbllist_node_t *node = (dbllist_node_t*)malloc(sizeof(dbllist_node_t));
    if(node == NULL){
        perror("malloc failed\n");
        exit(1);
    }

    memset(node,0,sizeof(dbllist_node_t));
    dbllist_data(node) = data;
    dbllist_prev(node) = NULL;
    dbllist_size(list)++;
    if(dbllist_head(list) == NULL){
        dbllist_head(list) = node;
        dbllist_tail(list) = node;
        return 0;
    }
    dbllist_next(node) = dbllist_head(list);
    dbllist_prev(dbllist_head(list)) = node;
    dbllist_head(list) = node;
    return 0;
}

int dbllist_remove(dbllist_t *list, dbllist_node_t* node,dbllist_destroy_t dealloc){
    if(list == NULL || node == NULL)
        return -1;
    dbllist_node_t *temp ;
    // we have 4 types
    //if the list has 1 item and we want to remove it
    if(dbllist_size(list) == 1 && node == dbllist_head(list)){
        temp = dbllist_head(list);
        dbllist_head(list) = NULL;
        dbllist_tail(list) = NULL;
        if(dealloc)
            free(dbllist_data(temp));
        free(temp);
        dbllist_size(list)--;
        return 0;
    }
    // to remove the head
    else if(node == dbllist_head(list)){
        temp = dbllist_head(list);
        dbllist_head(list) = dbllist_next(temp);
        dbllist_prev(dbllist_head(list)) = NULL; // cause the head do not have prev
        if(dealloc)
            free(dbllist_data(temp));
        free(temp);
        dbllist_size(list)--;
        return 0;
    }
    // to remove the tail
    else if(node == dbllist_tail(list)){
        temp = dbllist_tail(list);
        dbllist_tail(list) = dbllist_prev(temp);
        dbllist_next(dbllist_tail(list)) = NULL;
        if(dealloc)
            free(dbllist_data(temp));
        free(temp);
        dbllist_size(list)--;
        return 0;
    }
    // to remove others
    for(temp = dbllist_head(list) ; temp != NULL ; temp = dbllist_next(temp)){
        if(temp == node){
            dbllist_next(dbllist_prev(temp)) = dbllist_next(temp);
            dbllist_prev(dbllist_next(temp)) = dbllist_prev(temp);
            if(dealloc)
                free(dbllist_data(temp));
            free(temp);
            dbllist_size(list)--;
            return 0;
        }
    }
//    free(temp);
    return -1;
}

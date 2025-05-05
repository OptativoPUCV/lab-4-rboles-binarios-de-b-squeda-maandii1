#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {
    TreeMap * new = (TreeMap *)malloc(sizeof(TreeMap));
    if (new == NULL) return NULL;
    new->root = NULL;
    new->current = NULL;
    new->lower_than = lower_than;
    return new;
}


void insertTreeMap(TreeMap * tree, void* key, void * value) {
    if (tree == NULL) {
        tree->root = createTreeNode(key, value);
        tree->current = tree->root;
        return;
    }

    TreeNode * aux = tree->root;
    TreeNode * parent = NULL;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) return; // ya existe la clave
        parent = aux;
        if (tree->lower_than(aux->pair->key, key)) {
            aux = aux->right; 
        } else {
            aux = aux->left; 
        }
    }
    
    TreeNode * new = createTreeNode(key, value);
    new ->parent = parent;
    if (tree->lower_than(parent->pair->key, key)) {
        parent->right = new; 
    } else {
        parent->left = new; 
    }
    tree->current = new; //el nodo actual va a apuntar al nuevo nodo insertado
}

TreeNode * minimum(TreeNode * x){ 
    if (x == NULL) return NULL;
    while (x->left != NULL) {
        x = x->left;
    }// la clave minima, es el nodo mas a la izquierda
    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    //caso 1: sin hijos
    if (node->left == NULL && node->right == NULL) {
        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = NULL;
            } else {
                node->parent->right = NULL;
            }
        } else {
            tree->root = NULL; //el nodo es la raiz
        }
        free(node->pair);
        free(node);
        return;
    }
    //caso 2: un hijo
    if (node->left == NULL ) {
        if (node->parent != NULL) {
            if (node->parent->left == node) {
                node->parent->left = node->right;
            } else {
                node->parent->right = node->right;
            }
        } else {
            tree->root = node->right; //el nodo es la raiz
        }
        node->right->parent = node->parent;
        free(node->pair);
        free(node);
        return;
    }

    if (node->right == NULL ) {
        if (node->parent != NULL) {
            if (node->parent->right == node) {
                node->parent->right = node->left;
            } else {
                node->parent->left = node->right;
            }
        } else {
            tree->root = node->right; //el nodo es la raiz
        }
        node->left->parent = node->parent;
        free(node->pair);
        free(node);
        return;
    }
    //caso 3: dos hijos
    TreeNode * sucesor = minimum(node->right); //busco el sucesor
    node->pair->key = sucesor->pair->key; //cambio la clave por la del sucesor
    node->pair->value = sucesor->pair->value; //cambio el valor por el del sucesor
    removeNode(tree, sucesor); //elimino el sucesor
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);
}


Pair * searchTreeMap(TreeMap * tree, void* key) {
    TreeNode * aux = tree->root;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) {
            tree->current = aux;
            return aux->pair;
        } else if (tree->lower_than(aux->pair->key, key)) {
            aux = aux->right; 
        } else {
            aux = aux->left; 
        }
    }
    return NULL;
}


Pair * upperBound(TreeMap * tree, void* key) {
    TreeNode * aux = tree->root;
    TreeNode * ub_node = NULL;
    while (aux != NULL) {
        if (is_equal(tree, key, aux->pair->key)) {
            tree->current = aux;
            return aux->pair;
        } 
        if (tree->lower_than(aux->pair->key, key)) {
            ub_node = aux;
            aux = aux->right; 
        } else {
            aux = aux->left;
        }
    }
    if (ub_node != NULL) {
        tree->current = ub_node;
        return ub_node->pair;
    }
    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL) return NULL;
    TreeNode * minNode = minimum(tree->root);
    tree->current = minNode;
    return minNode->pair;
}

Pair * nextTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->current == NULL) return NULL;
    TreeNode * aux = tree->current;
    if (aux->right != NULL) {
        aux = minimum(aux->right);
        tree->current = aux;
        return aux->pair;
    }
    while (aux->parent != NULL) {
        if (aux->parent->left == aux) {
            tree->current = aux->parent;
            return aux->parent->pair;
        }
        aux = aux->parent;
    }
    tree->current = NULL; // no hay siguiente
    return NULL;
}

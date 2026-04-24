#include "bst.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static BSTNode* bstNodeCreate(int value)
{
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (node == NULL) {
        return NULL;
    }

    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

BST* bstCreate(void)
{
    BST* tree = (BST*)malloc(sizeof(BST));
    if (tree == NULL) {
        return NULL;
    }

    tree->root = NULL;
    tree->size = 0;
    return tree;
}

static void bstNodeFreeRecursive(BSTNode* node)
{
    if (node == NULL) {
        return;
    }

    bstNodeFreeRecursive(node->left);
    bstNodeFreeRecursive(node->right);
    free(node);
}

void bstFree(BST* tree)
{
    if (tree == NULL) {
        return;
    }

    bstNodeFreeRecursive(tree->root);
    free(tree);
}

size_t bstSize(BST* tree)
{
    return tree == NULL ? 0 : tree->size;
}

bool bstContains(BST* tree, int value)
{
    if (tree == NULL) {
        return false;
    }

    BSTNode* cur = tree->root;
    while (cur != NULL) {
        if (value < cur->value) {
            cur = cur->left;
        } else if (value > cur->value) {
            cur = cur->right;
        } else {
            return true;
        }
    }

    return false;
}

bool bstInsert(BST* tree, int value)
{
    if (tree == NULL) {
        return false;
    }

    BSTNode** cur = &tree->root;
    while (*cur != NULL) {
        if (value < (*cur)->value) {
            cur = &(*cur)->left;
        } else if (value > (*cur)->value) {
            cur = &(*cur)->right;
        } else {
            return false;
        }
    }

    *cur = bstNodeCreate(value);
    if (*cur == NULL) {
        errno = ENOMEM;
        return false;
    }

    tree->size++;
    return true;
}

/* D Удаление */

static BSTNode* findMinNode(BSTNode* node)
{
    if (node == NULL) {
        return NULL;
    }

    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

static BSTNode* deleteNodeRecursive(BSTNode* node, int value, bool* deleted)
{
    if (node == NULL) {
        return NULL;
    }

    if (value < node->value) {
        node->left = deleteNodeRecursive(node->left, value, deleted);
        return node;
    }

    if (value > node->value) {
        node->right = deleteNodeRecursive(node->right, value, deleted);
        return node;
    }

    *deleted = true;

    if (node->left == NULL && node->right == NULL) {
        free(node);
        return NULL;
    }

    if (node->left == NULL) {
        BSTNode* right = node->right;
        free(node);
        return right;
    }

    if (node->right == NULL) {
        BSTNode* left = node->left;
        free(node);
        return left;
    }

    BSTNode* successor = findMinNode(node->right);
    node->value = successor->value;

    bool dummyDeleted = false;
    node->right = deleteNodeRecursive(node->right, successor->value, &dummyDeleted);
    return node;
}

void bstDelete(BST* tree, int value)
{
    if (tree == NULL) {
        return;
    }

    bool deleted = false;
    tree->root = deleteNodeRecursive(tree->root, value, &deleted);
    if (deleted) {
        tree->size--;
    }
}

/* H Итератор */

static bool iteratorEnsureCapacity(Iterator* it)
{
    if (it->top < it->capacity) {
        return true;
    }

    size_t newCapacity = (it->capacity == 0) ? 8 : it->capacity * 2;
    BSTNode** newStack = (BSTNode**)realloc(it->stack, newCapacity * sizeof(BSTNode*));
    if (newStack == NULL) {
        return false;
    }

    it->stack = newStack;
    it->capacity = newCapacity;
    return true;
}

static bool iteratorPush(Iterator* it, BSTNode* node)
{
    if (!iteratorEnsureCapacity(it)) {
        return false;
    }

    it->stack[it->top++] = node;
    return true;
}

static BSTNode* iteratorPopNode(Iterator* it)
{
    if (it == NULL || it->top == 0) {
        return NULL;
    }

    return it->stack[--it->top];
}

static bool iteratorPushLeftBranch(Iterator* it, BSTNode* node)
{
    while (node != NULL) {
        if (!iteratorPush(it, node)) {
            return false;
        }
        node = node->left;
    }
    return true;
}

Iterator* iteratorInit(BST* tree)
{
    Iterator* it = (Iterator*)malloc(sizeof(Iterator));
    if (it == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    it->stack = NULL;
    it->top = 0;
    it->capacity = 0;
    it->failed = false;

    if (tree != NULL) {
        if (!iteratorPushLeftBranch(it, tree->root)) {
            free(it->stack);
            free(it);
            errno = ENOMEM;
            return NULL;
        }
    }

    return it;
}

bool iteratorHasNext(Iterator* it)
{
    return it != NULL && !it->failed && it->top > 0;
}

int iteratorNext(Iterator* it)
{
    if (it == NULL || it->failed || !iteratorHasNext(it)) {
        errno = ERANGE;
        return 0;
    }

    BSTNode* node = iteratorPopNode(it);
    int result = node->value;

    errno = 0;
    if (!iteratorPushLeftBranch(it, node->right)) {
        it->failed = true;
        errno = ENOMEM;
    }

    return result;
}

void iteratorFree(Iterator* it)
{
    if (it == NULL) {
        return;
    }

    free(it->stack);
    free(it);
}

/* E Слияние двух деревьев */

BST* bstMerge(BST* tree1, BST* tree2)
{
    BST* result = bstCreate();
    if (result == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    Iterator* it1 = iteratorInit(tree1);
    Iterator* it2 = iteratorInit(tree2);

    if (it1 == NULL || it2 == NULL) {
        iteratorFree(it1);
        iteratorFree(it2);
        bstFree(result);
        errno = ENOMEM;
        return NULL;
    }

    errno = 0;
    while (iteratorHasNext(it1)) {
        int value = iteratorNext(it1);
        if (errno == ENOMEM) {
            iteratorFree(it1);
            iteratorFree(it2);
            bstFree(result);
            return NULL;
        }

        errno = 0;
        if (!bstInsert(result, value)) {
            if (errno == ENOMEM) {
                iteratorFree(it1);
                iteratorFree(it2);
                bstFree(result);
                return NULL;
            }
        }
    }

    errno = 0;
    while (iteratorHasNext(it2)) {
        int value = iteratorNext(it2);
        if (errno == ENOMEM) {
            iteratorFree(it1);
            iteratorFree(it2);
            bstFree(result);
            return NULL;
        }

        errno = 0;
        if (!bstInsert(result, value)) {
            if (errno == ENOMEM) {
                iteratorFree(it1);
                iteratorFree(it2);
                bstFree(result);
                return NULL;
            }
        }
    }

    iteratorFree(it1);
    iteratorFree(it2);
    return result;
}

/* F Проверка корректности BST */

static bool isValidNode(BSTNode* node, long long minAllowed, long long maxAllowed)
{
    if (node == NULL) {
        return true;
    }

    if ((long long)node->value <= minAllowed || (long long)node->value >= maxAllowed) {
        return false;
    }

    return isValidNode(node->left, minAllowed, node->value)
        && isValidNode(node->right, node->value, maxAllowed);
}

bool bstIsValid(BST* tree)
{
    if (tree == NULL) {
        return true;
    }

    return isValidNode(tree->root, LLONG_MIN, LLONG_MAX);
}

/* G k й минимальный элемент */

int bstKthMin(BST* tree, int k)
{
    if (tree == NULL) {
        errno = ERANGE;
        return 0;
    }

    if (k <= 0) {
        errno = EINVAL;
        return 0;
    }

    if ((size_t)k > bstSize(tree)) {
        errno = ERANGE;
        return 0;
    }

    Iterator* it = iteratorInit(tree);
    if (it == NULL) {
        errno = ENOMEM;
        return 0;
    }

    int value = 0;
    errno = 0;
    for (int i = 0; i < k; i++) {
        value = iteratorNext(it);
        if (errno == ENOMEM || errno == ERANGE) {
            iteratorFree(it);
            return 0;
        }
    }

    iteratorFree(it);
    return value;
}

#ifndef BST_H
#define BST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct BSTNode {
    int value;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct BST {
    BSTNode* root;
    size_t size;
} BST;

typedef struct Iterator {
    BSTNode** stack;
    size_t top;
    size_t capacity;
    bool failed;
} Iterator;

BST* bstCreate(void);
void bstFree(BST* tree);

/* true если вставили false если уже был или не хватило памяти */
bool bstInsert(BST* tree, int value);

bool bstContains(BST* tree, int value);
size_t bstSize(BST* tree);

/* D Удаление */
void bstDelete(BST* tree, int value);

/* E Слияние */
/* Возвращает новое дерево со всеми элементами из tree1 и tree2 без дубликатов */
/* Если не хватило памяти возвращает NULL и выставляет errno ENOMEM */
BST* bstMerge(BST* tree1, BST* tree2);

/* F Проверка корректности */
bool bstIsValid(BST* tree);

/*
 G k й минимальный элемент
 Если k <= 0 возвращает 0 и выставляет errno EINVAL
 Если tree == NULL или k > bstSize(tree) возвращает 0 и выставляет errno ERANGE
 Если не хватило памяти на внутренний итератор возвращает 0 и выставляет errno ENOMEM
*/
int bstKthMin(BST* tree, int k);

/* H Итератор inorder без рекурсии */
Iterator* iteratorInit(BST* tree);
bool iteratorHasNext(Iterator* it);

/*
 Если итератор исчерпан или it == NULL возвращает 0 и выставляет errno ERANGE
 Если во время продвижения не хватило памяти возвращает текущий элемент
 Выставляет errno ENOMEM и переводит итератор в состояние failed
 После этого дальнейшее использование итератора не допускается
 iteratorHasNext(it) вернет false
*/
int iteratorNext(Iterator* it);

void iteratorFree(Iterator* it);

#endif

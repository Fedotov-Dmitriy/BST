#include "bst.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void testCreateAndFree(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);
    assert(bstSize(tree) == 0);
    assert(!bstContains(tree, 10));

    bstFree(tree);
    bstFree(NULL);
}

static void testInsertAndContains(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    assert(bstInsert(tree, 10));
    assert(bstInsert(tree, 5));
    assert(bstInsert(tree, 15));
    assert(bstInsert(tree, 3));
    assert(bstInsert(tree, 7));

    assert(bstSize(tree) == 5);

    assert(bstContains(tree, 10));
    assert(bstContains(tree, 5));
    assert(bstContains(tree, 15));
    assert(bstContains(tree, 3));
    assert(bstContains(tree, 7));

    assert(!bstContains(tree, 100));
    assert(!bstContains(tree, -1));

    // Дубликаты не вставляются
    assert(!bstInsert(tree, 10));
    assert(!bstInsert(tree, 5));
    assert(bstSize(tree) == 5);

    bstFree(tree);
}

static void testNullArguments(void)
{
    assert(bstSize(NULL) == 0);
    assert(!bstContains(NULL, 1));
    assert(!bstInsert(NULL, 1));

    bstDelete(NULL, 1);

    assert(bstIsValid(NULL));

    errno = 0;
    assert(bstKthMin(NULL, 1) == 0);
    assert(errno == ERANGE);

    Iterator* it = iteratorInit(NULL);
    assert(it != NULL);
    assert(!iteratorHasNext(it));

    errno = 0;
    assert(iteratorNext(it) == 0);
    assert(errno == ERANGE);

    iteratorFree(it);
    iteratorFree(NULL);
}

static void testDeleteLeaf(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    bstInsert(tree, 10);
    bstInsert(tree, 5);
    bstInsert(tree, 15);

    assert(bstSize(tree) == 3);

    bstDelete(tree, 5);

    assert(bstSize(tree) == 2);
    assert(!bstContains(tree, 5));
    assert(bstContains(tree, 10));
    assert(bstContains(tree, 15));
    assert(bstIsValid(tree));

    bstFree(tree);
}

static void testDeleteNodeWithOneChild(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    bstInsert(tree, 10);
    bstInsert(tree, 5);
    bstInsert(tree, 3);

    bstDelete(tree, 5);

    assert(bstSize(tree) == 2);
    assert(!bstContains(tree, 5));
    assert(bstContains(tree, 10));
    assert(bstContains(tree, 3));
    assert(bstIsValid(tree));

    bstFree(tree);
}

static void testDeleteNodeWithTwoChildren(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    bstInsert(tree, 10);
    bstInsert(tree, 5);
    bstInsert(tree, 15);
    bstInsert(tree, 12);
    bstInsert(tree, 20);

    bstDelete(tree, 10);

    assert(bstSize(tree) == 4);
    assert(!bstContains(tree, 10));
    assert(bstContains(tree, 5));
    assert(bstContains(tree, 15));
    assert(bstContains(tree, 12));
    assert(bstContains(tree, 20));
    assert(bstIsValid(tree));

    bstFree(tree);
}

static void testDeleteMissingValue(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    bstInsert(tree, 10);
    bstInsert(tree, 5);
    bstInsert(tree, 15);

    bstDelete(tree, 100);

    assert(bstSize(tree) == 3);
    assert(bstContains(tree, 10));
    assert(bstContains(tree, 5));
    assert(bstContains(tree, 15));
    assert(bstIsValid(tree));

    bstFree(tree);
}

static void testDeleteRootUntilEmpty(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    bstInsert(tree, 2);
    bstInsert(tree, 1);
    bstInsert(tree, 3);

    bstDelete(tree, 2);
    assert(bstSize(tree) == 2);
    assert(!bstContains(tree, 2));
    assert(bstIsValid(tree));

    bstDelete(tree, 1);
    assert(bstSize(tree) == 1);
    assert(!bstContains(tree, 1));
    assert(bstIsValid(tree));

    bstDelete(tree, 3);
    assert(bstSize(tree) == 0);
    assert(!bstContains(tree, 3));
    assert(bstIsValid(tree));

    bstFree(tree);
}

static void testIteratorSortedOrder(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    int values[] = { 10, 5, 15, 3, 7, 12, 20 };
    int expected[] = { 3, 5, 7, 10, 12, 15, 20 };

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        assert(bstInsert(tree, values[i]));
    }

    Iterator* it = iteratorInit(tree);
    assert(it != NULL);

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        assert(iteratorHasNext(it));

        errno = 0;
        int value = iteratorNext(it);

        assert(errno == 0);
        assert(value == expected[i]);
    }

    assert(!iteratorHasNext(it));

    errno = 0;
    assert(iteratorNext(it) == 0);
    assert(errno == ERANGE);

    iteratorFree(it);
    bstFree(tree);
}

static void testKthMin(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    int values[] = { 10, 5, 15, 3, 7, 12, 20 };

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        assert(bstInsert(tree, values[i]));
    }

    errno = 0;
    assert(bstKthMin(tree, 1) == 3);
    assert(errno == 0);

    errno = 0;
    assert(bstKthMin(tree, 2) == 5);
    assert(errno == 0);

    errno = 0;
    assert(bstKthMin(tree, 4) == 10);
    assert(errno == 0);

    errno = 0;
    assert(bstKthMin(tree, 7) == 20);
    assert(errno == 0);

    errno = 0;
    assert(bstKthMin(tree, 0) == 0);
    assert(errno == EINVAL);

    errno = 0;
    assert(bstKthMin(tree, -1) == 0);
    assert(errno == EINVAL);

    errno = 0;
    assert(bstKthMin(tree, 8) == 0);
    assert(errno == ERANGE);

    bstFree(tree);
}

static void testMerge(void)
{
    BST* tree1 = bstCreate();
    BST* tree2 = bstCreate();

    assert(tree1 != NULL);
    assert(tree2 != NULL);

    assert(bstInsert(tree1, 10));
    assert(bstInsert(tree1, 5));
    assert(bstInsert(tree1, 15));

    assert(bstInsert(tree2, 7));
    assert(bstInsert(tree2, 15)); // дубликат относительно tree1
    assert(bstInsert(tree2, 20));

    BST* merged = bstMerge(tree1, tree2);
    assert(merged != NULL);

    assert(bstSize(merged) == 5);

    assert(bstContains(merged, 5));
    assert(bstContains(merged, 7));
    assert(bstContains(merged, 10));
    assert(bstContains(merged, 15));
    assert(bstContains(merged, 20));

    assert(!bstContains(merged, 100));
    assert(bstIsValid(merged));

    assert(bstKthMin(merged, 1) == 5);
    assert(bstKthMin(merged, 2) == 7);
    assert(bstKthMin(merged, 3) == 10);
    assert(bstKthMin(merged, 4) == 15);
    assert(bstKthMin(merged, 5) == 20);

    bstFree(tree1);
    bstFree(tree2);
    bstFree(merged);
}

static void testMergeWithEmptyTrees(void)
{
    BST* tree1 = bstCreate();
    BST* tree2 = bstCreate();

    assert(tree1 != NULL);
    assert(tree2 != NULL);

    BST* merged = bstMerge(tree1, tree2);
    assert(merged != NULL);
    assert(bstSize(merged) == 0);
    assert(bstIsValid(merged));

    bstFree(tree1);
    bstFree(tree2);
    bstFree(merged);
}

static void testMergeWithNullTrees(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    bstInsert(tree, 2);
    bstInsert(tree, 1);
    bstInsert(tree, 3);

    BST* merged1 = bstMerge(tree, NULL);
    assert(merged1 != NULL);
    assert(bstSize(merged1) == 3);
    assert(bstContains(merged1, 1));
    assert(bstContains(merged1, 2));
    assert(bstContains(merged1, 3));

    BST* merged2 = bstMerge(NULL, tree);
    assert(merged2 != NULL);
    assert(bstSize(merged2) == 3);
    assert(bstContains(merged2, 1));
    assert(bstContains(merged2, 2));
    assert(bstContains(merged2, 3));

    BST* merged3 = bstMerge(NULL, NULL);
    assert(merged3 != NULL);
    assert(bstSize(merged3) == 0);

    bstFree(tree);
    bstFree(merged1);
    bstFree(merged2);
    bstFree(merged3);
}

static void testNegativeValues(void)
{
    BST* tree = bstCreate();
    assert(tree != NULL);

    assert(bstInsert(tree, 0));
    assert(bstInsert(tree, -10));
    assert(bstInsert(tree, 10));
    assert(bstInsert(tree, -20));
    assert(bstInsert(tree, -5));

    assert(bstSize(tree) == 5);

    assert(bstContains(tree, -20));
    assert(bstContains(tree, -10));
    assert(bstContains(tree, -5));
    assert(bstContains(tree, 0));
    assert(bstContains(tree, 10));

    assert(bstKthMin(tree, 1) == -20);
    assert(bstKthMin(tree, 2) == -10);
    assert(bstKthMin(tree, 3) == -5);
    assert(bstKthMin(tree, 4) == 0);
    assert(bstKthMin(tree, 5) == 10);

    bstDelete(tree, -10);

    assert(!bstContains(tree, -10));
    assert(bstSize(tree) == 4);
    assert(bstIsValid(tree));

    bstFree(tree);
}

int main(void)
{
    testCreateAndFree();
    testInsertAndContains();
    testNullArguments();

    testDeleteLeaf();
    testDeleteNodeWithOneChild();
    testDeleteNodeWithTwoChildren();
    testDeleteMissingValue();
    testDeleteRootUntilEmpty();

    testIteratorSortedOrder();

    testKthMin();

    testMerge();
    testMergeWithEmptyTrees();
    testMergeWithNullTrees();

    testNegativeValues();
    printf("All BST tests passed successfully.\n");
    return 0;
}

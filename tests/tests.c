#include "bst.h"

int main(void)
{
    BST* tree = bstCreate();
    if (tree != NULL) {
        bstFree(tree);
    }
    return 0;
}

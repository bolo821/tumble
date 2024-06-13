#include "tumble/tree.h"

#include <stdlib.h>
#include <string.h>

void tmb_tree_init(tmb_tree *t)
{
    t->nodes = NULL;
    t->count = 0;
    t->cap = 0;
    t->root = TMB_TREE_NULL;
    t->free_list = TMB_TREE_NULL;
    t->fatten = 0.1f;
}

void tmb_tree_free(tmb_tree *t)
{
    free(t->nodes);
    t->nodes = NULL;
    t->count = t->cap = 0;
    t->root = t->free_list = TMB_TREE_NULL;
}

static uint32_t alloc_node(tmb_tree *t)
{
    if (t->free_list == TMB_TREE_NULL) {
        size_t ncap = t->cap ? t->cap * 2 : 16;
        tmb_tree_node *nn =
            (tmb_tree_node *)realloc(t->nodes, ncap * sizeof(*nn));
        if (!nn)
            return TMB_TREE_NULL;
        t->nodes = nn;
        /* thread the new slots onto the free list */
        for (size_t i = t->cap; i < ncap; i++) {
            t->nodes[i].parent = (i + 1 < ncap) ? (uint32_t)(i + 1)
                                                : TMB_TREE_NULL;
            t->nodes[i].height = -1;
        }
        t->free_list = (uint32_t)t->cap;
        t->cap = ncap;
    }
    uint32_t idx = t->free_list;
    t->free_list = t->nodes[idx].parent;
    tmb_tree_node *n = &t->nodes[idx];
    n->parent = TMB_TREE_NULL;
    n->child1 = n->child2 = TMB_TREE_NULL;
    n->height = 0;
    n->leaf = 1;
    t->count++;
    return idx;
}

static void free_node(tmb_tree *t, uint32_t idx)
{
    t->nodes[idx].parent = t->free_list;
    t->nodes[idx].height = -1;
    t->free_list = idx;
    t->count--;
}

static int32_t maxi(int32_t a, int32_t b) { return a > b ? a : b; }

/* Rebalance the subtree rooted at a using a single rotation if it has become
 * lopsided; returns the new subtree root. */
static uint32_t balance(tmb_tree *t, uint32_t ia)
{
    tmb_tree_node *A = &t->nodes[ia];
    if (A->leaf || A->height < 2)
        return ia;
    uint32_t ib = A->child1, ic = A->child2;
    tmb_tree_node *B = &t->nodes[ib];
    tmb_tree_node *C = &t->nodes[ic];
    int32_t bal = C->height - B->height;

    if (bal > 1) {
        /* rotate C up */
        uint32_t if_ = C->child1, ig = C->child2;
        tmb_tree_node *F = &t->nodes[if_];
        tmb_tree_node *G = &t->nodes[ig];
        C->child1 = ia;
        C->parent = A->parent;
        A->parent = ic;
        if (C->parent != TMB_TREE_NULL) {
            if (t->nodes[C->parent].child1 == ia)
                t->nodes[C->parent].child1 = ic;
            else
                t->nodes[C->parent].child2 = ic;
        } else {
            t->root = ic;
        }
        if (F->height > G->height) {
            C->child2 = if_;
            A->child2 = ig;
            G->parent = ia;
            A->box = tmb_aabb_combine(B->box, G->box);
            C->box = tmb_aabb_combine(A->box, F->box);
            A->height = 1 + maxi(B->height, G->height);
            C->height = 1 + maxi(A->height, F->height);
        } else {
            C->child2 = ig;
            A->child2 = if_;
            F->parent = ia;
            A->box = tmb_aabb_combine(B->box, F->box);
            C->box = tmb_aabb_combine(A->box, G->box);
            A->height = 1 + maxi(B->height, F->height);
            C->height = 1 + maxi(A->height, G->height);
        }
        return ic;
    }
    if (bal < -1) {
        uint32_t id = B->child1, ie = B->child2;
        tmb_tree_node *D = &t->nodes[id];
        tmb_tree_node *E = &t->nodes[ie];
        B->child1 = ia;
        B->parent = A->parent;
        A->parent = ib;
        if (B->parent != TMB_TREE_NULL) {
            if (t->nodes[B->parent].child1 == ia)
                t->nodes[B->parent].child1 = ib;
            else
                t->nodes[B->parent].child2 = ib;
        } else {
            t->root = ib;
        }
        if (D->height > E->height) {
            B->child2 = id;
            A->child1 = ie;
            E->parent = ia;
            A->box = tmb_aabb_combine(C->box, E->box);
            B->box = tmb_aabb_combine(A->box, D->box);
            A->height = 1 + maxi(C->height, E->height);
            B->height = 1 + maxi(A->height, D->height);
        } else {
            B->child2 = ie;
            A->child1 = id;
            D->parent = ia;
            A->box = tmb_aabb_combine(C->box, D->box);
            B->box = tmb_aabb_combine(A->box, E->box);
            A->height = 1 + maxi(C->height, D->height);
            B->height = 1 + maxi(A->height, E->height);
        }
        return ib;
    }
    return ia;
}

static void fix_upward(tmb_tree *t, uint32_t idx)
{
    while (idx != TMB_TREE_NULL) {
        idx = balance(t, idx);
        tmb_tree_node *n = &t->nodes[idx];
        tmb_tree_node *c1 = &t->nodes[n->child1];
        tmb_tree_node *c2 = &t->nodes[n->child2];
        n->height = 1 + maxi(c1->height, c2->height);
        n->box = tmb_aabb_combine(c1->box, c2->box);
        idx = n->parent;
    }
}

static void insert_leaf(tmb_tree *t, uint32_t leaf)
{
    if (t->root == TMB_TREE_NULL) {
        t->root = leaf;
        t->nodes[leaf].parent = TMB_TREE_NULL;
        return;
    }
    tmb_aabb lb = t->nodes[leaf].box;
    uint32_t idx = t->root;
    while (!t->nodes[idx].leaf) {
        uint32_t c1 = t->nodes[idx].child1;
        uint32_t c2 = t->nodes[idx].child2;
        float area = tmb_aabb_perimeter(t->nodes[idx].box);
        tmb_aabb combined = tmb_aabb_combine(t->nodes[idx].box, lb);
        float comb_area = tmb_aabb_perimeter(combined);
        float cost = 2.0f * comb_area;
        float inherit = 2.0f * (comb_area - area);
        float cost1, cost2;
        {
            tmb_aabb u = tmb_aabb_combine(lb, t->nodes[c1].box);
            float p = tmb_aabb_perimeter(u);
            cost1 = (t->nodes[c1].leaf ? p : p - tmb_aabb_perimeter(t->nodes[c1].box)) + inherit;
        }
        {
            tmb_aabb u = tmb_aabb_combine(lb, t->nodes[c2].box);
            float p = tmb_aabb_perimeter(u);
            cost2 = (t->nodes[c2].leaf ? p : p - tmb_aabb_perimeter(t->nodes[c2].box)) + inherit;
        }
        if (cost < cost1 && cost < cost2)
            break;
        idx = (cost1 < cost2) ? c1 : c2;
    }

    uint32_t sibling = idx;
    uint32_t old_parent = t->nodes[sibling].parent;
    uint32_t new_parent = alloc_node(t);
    t->nodes[new_parent].parent = old_parent;
    t->nodes[new_parent].leaf = 0;
    t->nodes[new_parent].box = tmb_aabb_combine(lb, t->nodes[sibling].box);
    t->nodes[new_parent].height = t->nodes[sibling].height + 1;
    t->nodes[new_parent].child1 = sibling;
    t->nodes[new_parent].child2 = leaf;
    t->nodes[sibling].parent = new_parent;
    t->nodes[leaf].parent = new_parent;
    if (old_parent != TMB_TREE_NULL) {
        if (t->nodes[old_parent].child1 == sibling)
            t->nodes[old_parent].child1 = new_parent;
        else
            t->nodes[old_parent].child2 = new_parent;
    } else {
        t->root = new_parent;
    }
    fix_upward(t, t->nodes[leaf].parent);
}

static void remove_leaf(tmb_tree *t, uint32_t leaf)
{
    if (leaf == t->root) {
        t->root = TMB_TREE_NULL;
        return;
    }
    uint32_t parent = t->nodes[leaf].parent;
    uint32_t grand = t->nodes[parent].parent;
    uint32_t sibling = (t->nodes[parent].child1 == leaf)
                           ? t->nodes[parent].child2
                           : t->nodes[parent].child1;
    if (grand != TMB_TREE_NULL) {
        if (t->nodes[grand].child1 == parent)
            t->nodes[grand].child1 = sibling;
        else
            t->nodes[grand].child2 = sibling;
        t->nodes[sibling].parent = grand;
        free_node(t, parent);
        fix_upward(t, grand);
    } else {
        t->root = sibling;
        t->nodes[sibling].parent = TMB_TREE_NULL;
        free_node(t, parent);
    }
}

static tmb_aabb fatten(tmb_aabb b, float m)
{
    b.min.x -= m;
    b.min.y -= m;
    b.max.x += m;
    b.max.y += m;
    return b;
}

uint32_t tmb_tree_insert(tmb_tree *t, tmb_aabb box, uint32_t id)
{
    uint32_t leaf = alloc_node(t);
    if (leaf == TMB_TREE_NULL)
        return TMB_TREE_NULL;
    t->nodes[leaf].box = fatten(box, t->fatten);
    t->nodes[leaf].id = id;
    t->nodes[leaf].leaf = 1;
    t->nodes[leaf].height = 0;
    insert_leaf(t, leaf);
    return leaf;
}

void tmb_tree_remove(tmb_tree *t, uint32_t proxy)
{
    if (proxy >= t->cap || t->nodes[proxy].height < 0)
        return;
    remove_leaf(t, proxy);
    free_node(t, proxy);
}

int tmb_tree_move(tmb_tree *t, uint32_t proxy, tmb_aabb box)
{
    if (tmb_aabb_contains_box(t->nodes[proxy].box, box))
        return 0;
    remove_leaf(t, proxy);
    t->nodes[proxy].box = fatten(box, t->fatten);
    insert_leaf(t, proxy);
    return 1;
}

size_t tmb_tree_query(const tmb_tree *t, tmb_aabb box, uint32_t *out,
                      size_t cap)
{
    if (t->root == TMB_TREE_NULL)
        return 0;
    uint32_t stack[256];
    int sp = 0;
    stack[sp++] = t->root;
    size_t n = 0;
    while (sp > 0) {
        uint32_t idx = stack[--sp];
        if (idx == TMB_TREE_NULL)
            continue;
        const tmb_tree_node *node = &t->nodes[idx];
        if (!tmb_aabb_overlap(node->box, box))
            continue;
        if (node->leaf) {
            if (n < cap)
                out[n] = node->id;
            n++;
        } else if (sp + 2 <= (int)(sizeof(stack) / sizeof(stack[0]))) {
            stack[sp++] = node->child1;
            stack[sp++] = node->child2;
        }
    }
    return n;
}

int tmb_tree_height(const tmb_tree *t)
{
    if (t->root == TMB_TREE_NULL)
        return 0;
    return t->nodes[t->root].height;
}

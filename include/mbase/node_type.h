#ifndef MBASE_NODE_TYPE_H
#define MBASE_NODE_TYPE_H

#include <mbase/common.h>
#include <algorithm>

MBASE_STD_BEGIN

#define MBASE_AVL_ALLOWED_IMBALANCE 1

template<typename T>
struct list_node {
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using move_reference = T&&;

    list_node* prev;
    list_node* next;
    value_type data;

    list_node(const_reference in_object) noexcept : prev(nullptr), next(nullptr), data(in_object) {}
    list_node(move_reference in_object) noexcept : prev(nullptr), next(nullptr), data(std::move(in_object)) {}
};

/*
    IMPLEMENTATION BELOW IS TAKEN FROM the book called: 
    'Data Structures and Algorithm Analysis in C++' by Mark Allen Weiss
*/

template<typename T, typename Compare>
struct avl_node {
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using move_reference = T&&;
    using key_compare = Compare;

    avl_node* parent;
    avl_node* left;
    avl_node* right;
    I32 height;
    value_type data;

    avl_node(const_reference in_object) noexcept : parent(nullptr), left(nullptr), right(nullptr), height(0), data(in_object){}
    avl_node(move_reference in_object) noexcept : parent(nullptr), left(nullptr), right(nullptr), height(0), data(std::move(in_object)) {}

    static I32 get_height(avl_node* in_node) { return in_node == nullptr ? -1 : in_node->height; }

    template<typename ExternalIterator>
    static std::pair<ExternalIterator, bool> insert_node(avl_node*& in_node, const T& in_value, avl_node* in_parent)
    {
        key_compare comparator;
        std::pair<ExternalIterator, bool> resultPair = std::make_pair(nullptr, false);
        if (in_node == nullptr)
        {
            in_node = new avl_node(in_value);
            in_node->parent = in_parent;
            resultPair = std::make_pair(in_node, true);
            balance(in_node);
        }
        else if (comparator(in_value, in_node->data))
        {
            insert_node<ExternalIterator>(in_node->left, in_value, in_node);
            balance(in_node);
        }
        else if (comparator(in_node->data, in_value))
        {
            insert_node<ExternalIterator>(in_node->right, in_value, in_node);
            balance(in_node);
        }
        else
        {
            // SELF-NOTE : we will insert the node if it is multiset
            // MEANS, VALUES ARE EQUAL
            // do nothing
        }
        return resultPair;
    }
    
    template<typename ExternalIterator>
    static std::pair<ExternalIterator, bool> insert_node(avl_node*& in_node, T&& in_value, avl_node* in_parent)
    {
        key_compare comparator;
        std::pair<ExternalIterator, bool> resultPair = std::make_pair(nullptr, false);
        if (in_node == nullptr)
        {
            in_node = new avl_node(std::move(in_value));
            in_node->parent = in_parent;
            resultPair = std::make_pair(in_node, true);
            balance(in_node);
        }
        else if (comparator(in_value, in_node->data))
        {
            return insert_node<ExternalIterator>(in_node->left, std::move(in_value), in_node);
            balance(in_node);
        }
        else if (comparator(in_node->data, in_value))
        {
            return insert_node<ExternalIterator>(in_node->right, std::move(in_value), in_node);
            balance(in_node);
        }
        else
        {
            // MEANS, VALUES ARE EQUAL
            // do nothing
        }
        return resultPair;
    }

    static GENERIC rotate_with_left_child(avl_node*& in_k2)
    {
        avl_node* k1 = in_k2->left;
        in_k2->left = k1->right;
        if(k1->right)
        {
            k1->right->parent = in_k2;
        }
        k1->right = in_k2;
        k1->parent = in_k2->parent;
        in_k2->parent = k1;
        in_k2->height = std::max(get_height(in_k2->left), get_height(in_k2->right)) + 1;
        k1->height = std::max(get_height(k1->left), in_k2->height) + 1;
        in_k2 = k1;
    }

    static GENERIC rotate_with_right_child(avl_node*& in_k2)
    {
        avl_node* k1 = in_k2->right;
        in_k2->right = k1->left;
        if(k1->left)
        {
            k1->left->parent = in_k2;
        }
        k1->left = in_k2;
        k1->parent = in_k2->parent;
        in_k2->parent = k1;
        in_k2->height = std::max(get_height(in_k2->left), get_height(in_k2->right)) + 1;
        k1->height = std::max(get_height(k1->right), in_k2->height) + 1;
        in_k2 = k1;
    }

    static GENERIC double_with_left_child(avl_node*& in_node)
    {
        rotate_with_right_child(in_node->left);
        rotate_with_left_child(in_node);
    }

    static GENERIC double_with_right_child(avl_node*& in_node)
    {
        rotate_with_left_child(in_node->right);
        rotate_with_right_child(in_node);
    }

    static GENERIC balance(avl_node*& in_node)
    {
        if(!in_node)
        {
            return;
        }

        if(get_height(in_node->left) - get_height(in_node->right) > MBASE_AVL_ALLOWED_IMBALANCE)
        {
            if(get_height(in_node->left->left) >= get_height(in_node->left->right))
            {
                rotate_with_left_child(in_node);
            }
            else
            {
                double_with_left_child(in_node);
            }
        }
        else if (get_height(in_node->right) - get_height(in_node->left) > MBASE_AVL_ALLOWED_IMBALANCE)
        {
            if (get_height(in_node->right->right) >= get_height(in_node->right->left))
            {
                rotate_with_right_child(in_node);
            }
            else
            {
                double_with_right_child(in_node);
            }
        }

        in_node->height = std::max(get_height(in_node->left), get_height(in_node->right)) + 1;
    }

    static avl_node* find_min(avl_node* in_node) {
        if(!in_node)
        {
            return nullptr;
        }
        if(!in_node->left)
        {
            return in_node;
        }
        return find_min(in_node->left);
    }

    static avl_node* find_max(avl_node* in_node) {
        if (!in_node)
        {
            return nullptr;
        }
        if (!in_node->right)
        {
            return in_node;
        }
        return find_max(in_node->right);
    }


    static GENERIC remove_node(avl_node*& in_node, const T& in_value)
    {
        if (!in_node)
        {
            return;
        }
        else
        {
            key_compare comparator;
            if (comparator(in_value, in_node->data))
            {
                remove_node(in_node->left, in_value);
            }
            else if (comparator(in_node->data, in_value))
            {
                remove_node(in_node->right, in_value);
            }
            else
            {
                avl_node* oldNode = in_node;
                if(in_node->left && in_node->right)
                {
                    avl_node* traverseRemove = in_node->left;
                    while(traverseRemove)
                    {
                        in_node = traverseRemove;
                        traverseRemove = traverseRemove->right;
                    }
                    printf("%x %x, %d\n", in_node->left, in_node->right, in_node->data);
                    oldNode->data = in_node->data;
                    remove_node(in_node, in_node->data);
                    return;
                }

                if(in_node->left)
                {
                    if (in_node->parent)
                    {
                        if (in_node->parent->left == in_node)
                        {
                            in_node->parent->left = in_node->left;
                        }
                        else if (in_node->parent->right == in_node)
                        {
                            in_node->parent->right = in_node->left;
                        }
                        in_node->left->parent = in_node->parent;
                    }

                    /*if(in_node->left->right)
                    {
                        avl_node* rightTraversal = in_node->left->right;
                        while (rightTraversal)
                        {
                            in_node = rightTraversal;
                            rightTraversal = rightTraversal->right;
                        }

                        in_node->parent->right = nullptr;
                        in_node->left = oldNode->left;
                        in_node->right = oldNode->right;
                        in_node->parent = oldNode->parent;
                        
                        if(in_node->parent->right == oldNode)
                        {
                            in_node->parent->right = in_node;
                        }
                        else if(in_node->parent->left == oldNode)
                        {
                            in_node->parent->left = in_node;
                        }

                        in_node->left->parent = in_node;
                        in_node->right->parent = in_node;
                    }
                    else
                    {
                        if(in_node->parent)
                        {
                            if(in_node->parent->right == in_node)
                            {
                                in_node->parent->right = in_node->left;
                                in_node->left->parent = in_node->parent;
                                in_node->right->parent = in_node->left;
                                in_node->left->right = in_node->right;
                            }
                            else if(in_node->parent->left == in_node)
                            {
                                in_node->parent->left = in_node->left;
                                in_node->left->parent = in_node->parent;
                                in_node->right->parent = in_node->left;
                                in_node->left->right = in_node->right;
                            }
                        }
                    }*/
                }
                else if(in_node->right)
                {
                    // NOTE:
                    // in_node will be deleted in this case
                    if(in_node->parent)
                    {
                        if(in_node->parent->left == in_node) 
                        {
                            in_node->parent->left = in_node->right;
                        }
                        else if(in_node->parent->right == in_node)
                        {
                            in_node->parent->right = in_node->right;
                        }
                        in_node->right->parent = in_node->parent;
                    }
                }
                else
                {
                    // MEANS WE ARE THE LEAF
                    if(in_node->parent)
                    {
                        if(in_node->parent->right == in_node)
                        {
                            in_node->parent->right = nullptr;
                        }
                        else if(in_node->parent->left == in_node)
                        {
                            in_node->parent->left = nullptr;
                        }
                        in_node = in_node->parent;
                        
                    }
                }
                
                delete oldNode;
            }
        }
    }
};

MBASE_STD_END

#endif // !MBASE_NODE_TYPE_H

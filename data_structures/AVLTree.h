#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <string>
#include <memory>
#include <map>

// Enum for rotation types
enum class RotationType {
    LL,
    RR,
    LR,
    RL
};

// Convert enum to string for comparison and output
std::string rotationTypeToString(RotationType type) {
    switch (type) {
    case RotationType::LL: return "LL";
    case RotationType::RR: return "RR";
    case RotationType::LR: return "LR";
    case RotationType::RL: return "RL";
    default: return "UNKNOWN";
    }
}

class AVLNode {
public:
    int value;
    AVLNode* parent;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(int value, AVLNode* parent = nullptr)
        : value(value), parent(parent), left(nullptr), right(nullptr), height(1) {
    }

    void add_edges(std::vector<std::pair<int, int>>& edges) {
        if (left != nullptr) {
            edges.push_back(std::make_pair(value, left->value));
            left->add_edges(edges);
        }
        if (right != nullptr) {
            edges.push_back(std::make_pair(value, right->value));
            right->add_edges(edges);
        }
    }

    int left_height() {
        if (left == nullptr) {
            return 0;
        }
        else {
            return left->height;
        }
    }

    int right_height() {
        if (right == nullptr) {
            return 0;
        }
        else {
            return right->height;
        }
    }

    void update_height() {
        height = std::max(left_height(), right_height()) + 1;

        if (parent) {
            parent->update_height();
        }
    }

    int balance_factor() {
        return left_height() - right_height();
    }

    void set_left_child(AVLNode* node) {
        left = node;
        if (node != nullptr) {
            node->parent = this;
        }
    }

    void set_right_child(AVLNode* node) {
        right = node;
        if (node != nullptr) {
            node->parent = this;
        }
    }

    bool is_left() {
        return (parent != nullptr) && (parent->left == this);
    }

    AVLNode* leftmost() {
        if (left == nullptr) {
            return this;
        }
        else {
            return left->leftmost();
        }
    }

    AVLNode* rightmost() {
        if (right == nullptr) {
            return this;
        }
        else {
            return right->rightmost();
        }
    }
};

class AVLTree {
private:
    std::vector<AVLNode*> array_representation;
    size_t current;

public:
    AVLNode* root;

    AVLTree() : root(nullptr), current(0) {}

    std::vector<AVLNode*> traverse_inorder(AVLNode* node) {
        if (node == nullptr) {
            return std::vector<AVLNode*>();
        }

        std::vector<AVLNode*> array_repr = traverse_inorder(node->left);
        array_repr.push_back(node);
        std::vector<AVLNode*> right_traversal = traverse_inorder(node->right);
        array_repr.insert(array_repr.end(), right_traversal.begin(), right_traversal.end());

        return array_repr;
    }

    // Iterator functionality
    class iterator {
    private:
        std::vector<AVLNode*> nodes;
        size_t index;

    public:
        iterator(const std::vector<AVLNode*>& nodes, size_t index) : nodes(nodes), index(index) {}

        AVLNode* operator*() { return nodes[index]; }
        iterator& operator++() { ++index; return *this; }
        bool operator!=(const iterator& other) const { return index != other.index; }
        bool operator==(const iterator& other) const { return index == other.index; }
    };

    iterator begin() {
        array_representation = traverse_inorder(root);
        return iterator(array_representation, 0);
    }

    iterator end() {
        return iterator(array_representation, array_representation.size());
    }

    AVLNode* right_rotation(AVLNode* node) {
        AVLNode* x = node;
        AVLNode* y = node->left;
        AVLNode* t1 = y->left;
        AVLNode* t2 = y->right;
        AVLNode* t3 = x->right;

        y->set_right_child(x);
        y->set_left_child(t1);
        x->set_left_child(t2);
        x->set_right_child(t3);

        return y;
    }

    AVLNode* left_rotation(AVLNode* node) {
        AVLNode* x = node;
        AVLNode* y = node->right;
        AVLNode* t1 = x->left;
        AVLNode* t2 = y->left;
        AVLNode* t3 = y->right;

        y->set_left_child(x);
        x->set_left_child(t1);
        x->set_right_child(t2);
        y->set_right_child(t3);

        return y;
    }

    AVLNode* left_right_rotation(AVLNode* node) {
        AVLNode* x = node;
        AVLNode* y = left_rotation(node->left);
        x->set_left_child(y);

        return right_rotation(x);
    }

    AVLNode* right_left_rotation(AVLNode* node) {
        AVLNode* x = node;
        AVLNode* y = right_rotation(node->right);
        x->set_right_child(y);

        return left_rotation(x);
    }

    void rotate(AVLNode* node, RotationType type) {
        AVLNode* parent = node->parent;
        bool is_left = node->is_left();

        AVLNode* new_node = nullptr;

        if (type == RotationType::LL) {
            new_node = left_rotation(node);
        }
        else if (type == RotationType::RR) {
            new_node = right_rotation(node);
        }
        else if (type == RotationType::LR) {
            new_node = left_right_rotation(node);
        }
        else if (type == RotationType::RL) {
            new_node = right_left_rotation(node);
        }
        else {
            throw std::runtime_error("Unknown rotation type " + rotationTypeToString(type));
        }

        if (parent != nullptr) {
            if (is_left) {
                parent->set_left_child(new_node);
            }
            else {
                parent->set_right_child(new_node);
            }
        }
        else {
            root = new_node;
            new_node->parent = nullptr;
        }

        new_node->left->update_height();
        new_node->right->update_height();
        new_node->update_height();
    }

    void restructure(AVLNode* node, bool verbose = true) {
        AVLNode* parent = node->parent;
        if (node->balance_factor() >= -1 && node->balance_factor() <= 1) {
            if (parent != nullptr) {
                restructure(parent);
            }
            return;
        }

        if (node->balance_factor() == -2) {
            if (node->right->right_height() > node->right->left_height()) {
                if (verbose) {
                    std::cout << "left" << std::endl;
                }
                rotate(node, RotationType::LL);
            }
            else {
                if (verbose) {
                    std::cout << "right-left" << std::endl;
                }
                rotate(node, RotationType::RL);
            }
        }
        else {
            if (node->left->left_height() > node->left->right_height()) {
                if (verbose) {
                    std::cout << "right" << std::endl;
                }
                rotate(node, RotationType::RR);
            }
            else {
                if (verbose) {
                    std::cout << "left-right" << std::endl;
                }
                rotate(node, RotationType::LR);
            }
        }

        if (parent != nullptr) {
            restructure(parent);
        }
    }

    AVLNode* search(int value) {
        AVLNode* node = root;

        if (node == nullptr) {
            throw std::runtime_error("Cannot search an element in the empty tree");
        }

        while (node != nullptr) {
            if (node->value == value) {
                return node;
            }

            if (node->value < value) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }

        return nullptr;
    }

    void insert(int value) {
        AVLNode* node = new AVLNode(value);
        AVLNode* after = root;

        if (root == nullptr) {
            root = node;
            return;
        }

        while (true) {
            if (after->value < value) {
                if (after->right == nullptr) {
                    after->set_right_child(node);
                    after->update_height();
                    restructure(after);
                    break;
                }
                else {
                    after = after->right;
                }
            }
            else {
                if (after->left == nullptr) {
                    after->set_left_child(node);
                    after->update_height();
                    restructure(after);
                    break;
                }
                else {
                    after = after->left;
                }
            }
        }
    }

    void single_delete(AVLNode* target_node, bool has_left_child, bool is_left) {
        if (has_left_child) {
            if (target_node->parent == nullptr) {
                root = target_node->left;
            }
            else {
                if (is_left) {
                    target_node->parent->set_left_child(target_node->left);
                }
                else {
                    target_node->parent->set_right_child(target_node->left);
                }
                target_node->parent->update_height();
            }
        }
        else {
            if (target_node->parent == nullptr) {
                root = target_node->right;
            }
            else {
                if (is_left) {
                    target_node->parent->set_left_child(target_node->right);
                }
                else {
                    target_node->parent->set_right_child(target_node->right);
                }
                target_node->parent->update_height();
            }
        }
    }

    void delete_node(AVLNode* target_node) {
        bool is_left = target_node->is_left();

        if (target_node->left == nullptr) {
            single_delete(target_node, false, is_left);
            if (target_node->parent != nullptr) {
                restructure(target_node->parent);
            }
        }
        else if (target_node->right == nullptr) {
            single_delete(target_node, true, is_left);
            if (target_node->parent != nullptr) {
                restructure(target_node->parent);
            }
        }
        else {
            AVLNode* parent = target_node->parent;
            AVLNode* left = target_node->left;
            AVLNode* right = target_node->right;
            bool is_left_child = target_node->is_left();
            AVLNode* substitute = target_node->left->rightmost();

            single_delete(substitute, false, substitute->parent->left == substitute);

            // treat the case where left = substitute separately
            if (left == substitute) {
                substitute->set_left_child(nullptr);
            }
            else {
                substitute->set_left_child(left);
            }
            substitute->set_right_child(right);

            if (parent == nullptr) {
                root = substitute;
                substitute->parent = nullptr;
            }
            else {
                if (is_left_child) {
                    parent->set_left_child(substitute);
                }
                else {
                    parent->set_right_child(substitute);
                }
            }

            substitute->update_height();
            restructure(substitute);
        }
    }

    void draw() {
        // Simple graph representation using adjacency list
        std::vector<std::pair<int, int>> edges;
        if (root != nullptr) {
            root->add_edges(edges);
        }

        std::cout << "Tree edges (parent -> child):" << std::endl;
        for (const auto& edge : edges) {
            std::cout << edge.first << " -> " << edge.second << std::endl;
        }

        // Note: For actual visualization, you would need a graph visualization library
        // such as Graphviz C++ bindings, OGDF, or similar
        std::cout << "Note: For graphical visualization, integrate with a graph library like Graphviz" << std::endl;
    }
};
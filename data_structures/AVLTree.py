import networkx as nx
import random
import numpy as np

from matplotlib import pyplot as plt
from enum import Enum


class RotationType(Enum):
    LL = "LL"
    RR = "RR"
    LR = "LR"
    RL = "RL"


class AVLNode:
    def __init__(self, value, parent=None):
        self.value = value
        self.parent = parent
        self.left = None
        self.right = None
        self.height = 1

    def add_edges(self, G):
        if not self.left is None:
            G.add_edge(self.value, self.left.value)
            self.left.add_edges(G)
        if not self.right is None:
            G.add_edge(self.value, self.right.value)
            self.right.add_edges(G)

    def left_height(self):
        if self.left is None:
            return 0
        else:
            return self.left.height

    def right_height(self):
        if self.right is None:
            return 0
        else:
            return self.right.height

    def update_height(self):
        self.height = max(self.left_height(), self.right_height()) + 1

        if self.parent:
            self.parent.update_height()

    def balance_factor(self):
        return self.left_height() - self.right_height()

    def set_left_child(self, node):
        self.left = node
        if not node is None:
            node.parent = self

    def set_right_child(self, node):
        self.right = node
        if not node is None:
            node.parent = self

    def is_left(self):
        return (not self.parent is None) and (self.parent.left == self)

    def leftmost(self):
        if self.left is None:
            return self
        else:
            return self.left.leftmost()

    def rightmost(self):
        if self.right is None:
            return self
        else:
            return self.right.rightmost()


class AVLTree:
    def __init__(self):
        self.root = None

    def traverse_inorder(self, node: AVLNode):
        if node is None:
            return []

        array_repr = self.traverse_inorder(node.left)
        array_repr.append(node)
        array_repr.extend(self.traverse_inorder(node.right))

        return array_repr

    def __iter__(self):
        self.array_representation = self.traverse_inorder(self.root)
        self.current = 0
        return self

    def __next__(self):
        if self.current < len(self.array_representation):
            element = self.array_representation[self.current]
            self.current += 1
            return element
        else:
            raise StopIteration

    def right_rotation(self, node: AVLNode):
        x = node
        y: AVLNode = node.left
        t1 = y.left
        t2 = y.right
        t3 = x.right

        y.set_right_child(x)
        y.set_left_child(t1)
        x.set_left_child(t2)
        x.set_right_child(t3)

        return y

    def left_rotation(self, node: AVLNode):
        x = node
        y: AVLNode = node.right
        t1 = x.left
        t2 = y.left
        t3 = y.right

        y.set_left_child(x)
        x.set_left_child(t1)
        x.set_right_child(t2)
        y.set_right_child(t3)

        return y

    def left_right_rotation(self, node: AVLNode):
        x = node
        y = self.left_rotation(node.left)
        x.set_left_child(y)

        return self.right_rotation(x)

    def right_left_rotation(self, node: AVLNode):
        x = node
        y = self.right_rotation(node.right)
        x.set_right_child(y)

        return self.left_rotation(x)

    def rotate(self, node: AVLNode, type: RotationType):
        parent: AVLNode = node.parent
        is_left = node.is_left()        

        if type == RotationType.LL:
            new_node = self.left_rotation(node)
        elif type == RotationType.RR:
            new_node = self.right_rotation(node)
        elif type == RotationType.LR:
            new_node = self.left_right_rotation(node)
        elif type == RotationType.RL:
            new_node = self.right_left_rotation(node)
        else:
            raise TypeError(f"Unknown rotation type {type}")

        if not parent is None:
            if is_left:
                parent.set_left_child(new_node)
            else:
                parent.set_right_child(new_node)
        else:
            self.root = new_node
            new_node.parent = None

        new_node.left.update_height()
        new_node.right.update_height()
        new_node.update_height()

    def restructure(self, node: AVLNode, verbose = True):
        parent = node.parent
        if -1 <= node.balance_factor() <= 1:
            if not parent is None:
                self.restructure(parent)
            return

        if node.balance_factor() == -2:
            if node.right.right_height() > node.right.left_height():
                if verbose:
                    print("left")
                self.rotate(node, type=RotationType.LL)                
            else:
                if verbose:
                    print("right-left")
                self.rotate(node, type=RotationType.RL)
        else:
            if node.left.left_height() > node.left.right_height():
                if verbose:
                    print("right")
                self.rotate(node, type=RotationType.RR)
            else:
                if verbose:
                    print("left-right")
                self.rotate(node, type=RotationType.LR)            

        if not parent is None:
            self.restructure(parent)

    def search(self, value: int):
        node = self.root

        if node is None:
            raise ValueError("Cannot search an element in the empty tree")

        while not node is None:
            if node.value == value:
                return node

            if node.value < value:
                node = node.right
            else:
                node = node.left

        return None

    def insert(self, value: int):
        node = AVLNode(value)
        after: AVLNode = self.root

        if self.root is None:
            self.root = node
            return

        while True:
            if after.value < value:
                if after.right == None:
                    after.set_right_child(node)
                    after.update_height()
                    self.restructure(after)
                    break
                else:
                    after = after.right
            else:
                if after.left == None:
                    after.set_left_child(node)
                    after.update_height()
                    self.restructure(after)
                    break
                else:
                    after = after.left

    def single_delete(self, target_node: AVLNode, has_left_child, is_left):
        if has_left_child:
            if target_node.parent is None:
                self.root = target_node.left
            else:
                if is_left:
                    target_node.parent.set_left_child(target_node.left)
                else:
                    target_node.parent.set_right_child(target_node.left)

                target_node.parent.update_height()            
        else:
            if target_node.parent is None:
                self.root = target_node.right
            else:
                if is_left:
                    target_node.parent.set_left_child(target_node.right)
                else:
                    target_node.parent.set_right_child(target_node.right)

                target_node.parent.update_height()

    def delete(self, target_node: AVLNode):
        is_left = target_node.is_left()

        if target_node.left is None:
            self.single_delete(target_node, False, is_left)
            if not target_node.parent is None:
                self.restructure(target_node.parent)
        elif target_node.right is None:
            self.single_delete(target_node, True, is_left)
            if not target_node.parent is None:
                self.restructure(target_node.parent)
        else:
            parent = target_node.parent
            left = target_node.left
            right = target_node.right
            is_left = target_node.is_left()
            substitute = target_node.left.rightmost()

            self.single_delete(substitute, False, substitute.parent.left == substitute)
            
            #treat the case where left = substitute separately
            if left == substitute:                
                substitute.set_left_child(None)
            else:
                substitute.set_left_child(left)
            substitute.set_right_child(right)         

            if parent is None:
                self.root = substitute
                substitute.parent = None
            else:
                if is_left:
                    parent.set_left_child(substitute)
                else:
                    parent.set_right_child(substitute)

            substitute.update_height()
            self.restructure(substitute)

    def draw(self):
        graph = nx.DiGraph()
        self.root.add_edges(graph)
        positioning = hierarchy_pos(graph, width = 4)
        nx.draw(graph, positioning, with_labels=True, arrows=True)
        plt.show()


def hierarchy_pos(G, root=None, width=1.0, vert_gap=0.2, vert_loc=0, xcenter=0.5):
    """
    From Joel's answer at https://stackoverflow.com/a/29597209/2966723.
    Licensed under Creative Commons Attribution-Share Alike

    If the graph is a tree this will return the positions to plot this in a
    hierarchical layout.

    G: the graph (must be a tree)

    root: the root node of current branch
    - if the tree is directed and this is not given,
      the root will be found and used
    - if the tree is directed and this is given, then
      the positions will be just for the descendants of this node.
    - if the tree is undirected and not given,
      then a random choice will be used.

    width: horizontal space allocated for this branch - avoids overlap with other branches

    vert_gap: gap between levels of hierarchy

    vert_loc: vertical location of root

    xcenter: horizontal location of root
    """
    if not nx.is_tree(G):
        raise TypeError("cannot use hierarchy_pos on a graph that is not a tree")

    if root is None:
        if isinstance(G, nx.DiGraph):
            root = next(
                iter(nx.topological_sort(G))
            )  # allows back compatibility with nx version 1.11
        else:
            root = random.choice(list(G.nodes))

    def _hierarchy_pos(
        G, root, width=1.0, vert_gap=0.2, vert_loc=0, xcenter=0.5, pos=None, parent=None
    ):
        """
        see hierarchy_pos docstring for most arguments

        pos: a dict saying where all nodes go if they have been assigned
        parent: parent of this branch. - only affects it if non-directed

        """

        if pos is None:
            pos = {root: (xcenter, vert_loc)}
        else:
            pos[root] = (xcenter, vert_loc)
        children = list(G.neighbors(root))
        if not isinstance(G, nx.DiGraph) and parent is not None:
            children.remove(parent)
        if len(children) != 0:
            dx = width / len(children)
            nextx = xcenter - width / 2 - dx / 2
            for child in children:
                nextx += dx
                pos = _hierarchy_pos(
                    G,
                    child,
                    width=dx,
                    vert_gap=vert_gap,
                    vert_loc=vert_loc - vert_gap,
                    xcenter=nextx,
                    pos=pos,
                    parent=root,
                )
        return pos

    return _hierarchy_pos(G, root, width, vert_gap, vert_loc, xcenter)


values = np.random.choice(np.arange(1, 5000), 20, replace = False)
delete_values = np.random.choice(values, 5, replace = False)
tree = AVLTree()

for val in values:
    tree.insert(val)

tree.draw()
print(f"to delete {delete_values}")
for value in delete_values:     
    tree.delete(tree.search(value))
    print(f"deleted {value}")
    tree.draw()
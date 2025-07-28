import numpy as np
import time
import matplotlib.pyplot as plt
from enum import Enum


class NodeType(Enum):
    Node = (1,)
    LeftSentinel = (2,)
    RightSentinel = (3,)


class SkipListNode:
    def __init__(self):
        self.type = None
        self.value = None
        self.prev = None
        self.next = None
        self.below = None
        self.span = 1

    @staticmethod
    def create_node(value, prev, next, below=None):
        node = SkipListNode()
        node.type = NodeType.Node
        node.value = value
        node.prev = prev
        node.next = next
        node.below = below

        return node

    @staticmethod
    def create_sentinel(type):
        node = SkipListNode()
        node.type = type

        return node

    def link_next(self, node):
        self.next = node
        node.prev = self

    def link_prev(self, node):
        self.prev = node
        node.next = self

    def __repr__(self):
        if self.type == NodeType.LeftSentinel:
            return "-∞"
        elif self.type == NodeType.RightSentinel:
            return "∞"
        else:
            return str(self.value)

    def __lt__(self, operand):
        if isinstance(operand, (int, np.integer)):
            if self.type == NodeType.Node:
                return self.value < operand
            else:
                return self.type == NodeType.LeftSentinel
        elif isinstance(operand, SkipListNode):
            if self.type == NodeType.LeftSentinel:
                return operand.type != NodeType.LeftSentinel
            elif self.type == NodeType.RightSentinel:
                return False
            else:
                return self.value < operand.value
        else:
            raise TypeError(f"unsupported type {type(operand)}")

    def __gt__(self, operand):
        if isinstance(operand, (int, np.integer)):
            if self.type == NodeType.Node:
                return self.value > operand
            else:
                return self.type == NodeType.RightSentinel
        elif isinstance(operand, SkipListNode):
            if self.type == NodeType.RightSentinel:
                return operand.type != NodeType.RightSentinel
            elif self.type == NodeType.LeftSentinel:
                return False
            else:
                return self.value > operand.value
        else:
            raise TypeError(f"unsupported type {type(operand)}")

    def __eq__(self, operand):
        if operand is None:
            return False
        elif isinstance(operand, (int, np.integer)):
            if self.type == NodeType.Node:
                return self.value == operand
            else:
                return False
        elif isinstance(operand, SkipListNode):
            if self.type == NodeType.Node and operand.type == NodeType.Node:
                return self.value == operand.value
            else:
                return self.type == operand.type
        else:
            raise TypeError(f"unsupported type {type(operand)}")

    def __le__(self, operand):
        return self < operand or self == operand

    def __ge__(self, operand):
        return self > operand or self == operand


class SkipList:
    def init_level(self, prev):
        left_sentinel = SkipListNode.create_sentinel(NodeType.LeftSentinel)
        right_sentinel = SkipListNode.create_sentinel(NodeType.RightSentinel)
        left_sentinel.link_next(right_sentinel)
        left_sentinel.span = self.size + 1
        right_sentinel.span = 0
        left_sentinel.below = prev[0]
        right_sentinel.below = prev[1]
        return (left_sentinel, right_sentinel)

    def add_levels(self, times):
        """
        returns the head of each added level
        """
        added = []
        prev = self.levels[-1]
        for i in range(times):
            level = self.init_level(prev)
            prev = level
            self.levels.append(level)
            added.append(level[0])

        return added

    def __init__(self):
        self.size = 0
        self.levels = [self.init_level((None, None))]

    def __len__(self):
        return self.size

    def __repr__(self):
        result = ""
        space = 3
        for level in self.levels:
            level_repr = ""
            current = level[0]
            while current != None:
                level_repr = level_repr + repr(current).ljust(space) + "-"

                for i in range(current.span - 1):
                    level_repr = level_repr + " " * space + "-"
                current = current.next
            result = result + level_repr + "\n"

        return result

    def __getitem__(self, index):
        """
        return the node at the given position at the topmost level
        """
        if index < 0 or index >= self.size:
            raise IndexError("index out of bounds")

        begin = self.levels[-1][0]
        position = 0
        index += 1

        while True:
            while position + begin.span <= index:
                position += begin.span
                begin = begin.next
            if position < index:
                begin = begin.below
            else:
                break

        return begin

    def promotions(self):
        return min(np.random.geometric(1 / 2), 32)

    def search(self, value):
        """
        return the node with given value at the topmost level or None if the node is not in the list
        """
        begin = self.levels[-1][0]

        while begin != None:  # itterate while we do not reach the bottom layer
            current = begin

            while current.next <= value:
                current = current.next

            if current == value:
                return current

            begin = current.below

        return None

    def contains(self, value):
        return self.search(value) != None

    def find_predecessors(self, value):
        predecessors = []
        position = 0
        begin = self.levels[-1][0]

        while begin != None:  # itterate while we do not reach the bottom layer
            current = begin

            while current.next < value:
                position += current.span
                current = current.next

            predecessors.append((current, position))

            begin = current.below

        return predecessors

    def insert(self, value, verbose=False):
        predecessors = self.find_predecessors(value)
        new_node = SkipListNode.create_node(value, None, None)

        predecessors = predecessors[::-1]
        insert_position = predecessors[0][1]
        _promotions = self.promotions()

        # account for the difference in the height cause by promotions
        if _promotions > len(self.levels):
            diff = _promotions - len(self.levels)
            added = self.add_levels(diff)
            predecessors.extend(zip(added, [0] * diff))

        tower = []
        for ptr in range(_promotions):
            after = predecessors[ptr][0]

            next = after.next
            after.link_next(new_node)
            new_node.link_next(next)
            tower.append(new_node)

            new_node = SkipListNode.create_node(value, None, None, below=new_node)

        for ptr in range(len(self.levels)):
            current = predecessors[ptr][0]
            if ptr < _promotions:
                next_position = predecessors[ptr][1] + current.span
                current.span = insert_position - predecessors[ptr][1] + 1
                tower[ptr].span = next_position - insert_position
            else:
                current.span += 1

        if verbose:
            print(self)

        self.size += 1

    def delete(self, value):
        if not self.contains(value):
            return False

        predecessors = self.find_predecessors(value)

        for predecessor in predecessors:
            node = predecessor[0]
            if node.next == value:
                node.span += node.next.span - 1
                node.link_next(node.next.next)
            else:
                node.span -= 1

        return True


list = SkipList()
size = 20
rand_input = np.random.randint(0, 10 * size, size)
seq_input = np.arange(size)
elapsed_time = []
for i in rand_input:
    start = time.time()
    list.insert(i, False)
    elapsed_time.append(time.time() - start)

print(list)

list.delete(15)
print(list)
print(list[2])

plt.plot(np.arange(size), elapsed_time)
plt.show()

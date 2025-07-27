import random
import numpy.random as rand
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
        if type(operand) is int:
            if self.type == NodeType.Node:
                return self.value < operand
            else:
                return self.type == NodeType.LeftSentinel
        elif type(operand) is SkipListNode:
            if self.type == NodeType.LeftSentinel:
                return operand.type != NodeType.LeftSentinel
            elif self.type == NodeType.RightSentinel:
                return False
            else:
                return self.value < operand.value

    def __gt__(self, operand):
        if type(operand) is int:
            if self.type == NodeType.Node:
                return self.value > operand
            else:
                return self.type == NodeType.RightSentinel
        elif type(operand) is SkipListNode:
            if self.type == NodeType.RightSentinel:
                return operand.type != NodeType.RightSentinel
            elif self.type == NodeType.LeftSentinel:
                return False
            else:
                return self.value > operand.value

    def __eq__(self, operand):
        if operand is None:
            return False
        elif type(operand) is int:
            if self.type == NodeType.Node:
                return self.value == operand
            else:
                return False
        elif self.type == NodeType.Node and operand.type == NodeType.Node:
            return self.value == operand.value
        else:
            return self.type == operand.type

    def __le__(self, operand):
        return self < operand or self == operand

    def __ge__(self, operand):
        return self > operand or self == operand


class SkipList:
    def init_level(self, prev):
        left_sentinel = SkipListNode.create_sentinel(NodeType.LeftSentinel)
        right_sentinel = SkipListNode.create_sentinel(NodeType.RightSentinel)
        left_sentinel.link_next(right_sentinel)
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
        self.levels = [self.init_level((None, None))]
        self.size = 0

    def __repr__(self):
        result = ""
        for level in self.levels:
            level_repr = ""
            current = level[0]
            while current != None:
                level_repr = level_repr + repr(current) + "-"
                current = current.next
            result = result + level_repr + "\n"

        return result

    def promotions(self):
        return rand.geometric(1 / 2)

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

    def insert(self, value, verbose=False):
        insert_after = []
        new_node = SkipListNode.create_node(value, None, None)
        begin = self.levels[-1][0]

        while begin != None:  # itterate while we do not reach the bottom layer
            current = begin

            while current.next < new_node:
                current = current.next

            insert_after.append(current)
            begin = current.below

        insert_after = insert_after[::-1]
        _promotions = self.promotions()

        # account for the difference in the height cause by promotions
        if _promotions > len(self.levels):
            diff = _promotions - len(self.levels)
            added = self.add_levels(diff)
            insert_after.extend(added)

        for ptr in range(_promotions):
            after = insert_after[ptr]

            next = after.next
            after.link_next(new_node)
            new_node.link_next(next)

            new_node = SkipListNode.create_node(value, None, None, below=new_node)

        if verbose:
            print(self)

        self.size += 1

    def delete(self, value):
        node = self.search(value)

        if node == None:
            return False
        else:
            while node != None:
                node.prev.link_next(node.next)
                node = node.below
            return True


list = SkipList()
for i in range(20):
    list.insert(i, False)

list.delete(15)
print(list)

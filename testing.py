from collections import namedtuple
import difflib
import cProfile
from time import time
import tqdm
import random

# my cpp version, may be installed as different name
import myers_diff as mdc

N = 75  # length
NC = 10 # num chars

def main():
    # original
    orig = [str(random.choice(range(NC))) for i in range(N)]
    # different version
    tag = [str(random.choice(range(NC))) for i in range(N)]

    for i in range(10):
        compare()


def compare():
    orig = [str(random.choice(range(NC))) for i in range(N)]
    tag = [str(random.choice(range(NC))) for i in range(N)]

    a = time()
    diff = myers_diff(orig, tag)
    b = time()

    a = time()
    diffc = mdc.myers_diff2(orig, tag)
    b = time()
    diffc = [(tc(i), j) for i, j in diffc]

    assert diffc == diff

def myers_diff(a_lines, b_lines, V = False):
    """ See http://www.xmailserver.org/diff2.pdf """
    frontier = {1: (0, [])}
    a_max = len(a_lines)
    b_max = len(b_lines)
    for d in range(0, a_max + b_max + 1):
        for k in range(-d, d + 1, 2):
            go_down = (k == -d or
                    (k != d and frontier[k - 1][0] < frontier[k + 1][0]))
            if V:
                print("go down", go_down)
            if go_down:
                if V:
                    print("last", k+1)
                old_x, history = frontier[k + 1]
                x = old_x
            else:
                if V:
                    print("last", k-1)
                old_x, history = frontier[k - 1]
                x = old_x + 1
            if V:
                print("old_x", old_x)

            history = history[:]
            y = x - k
            if V:
                print("x,y", x, y)
            if 1 <= y <= b_max and go_down:
                history.append((1, b_lines[y-1]))
            elif 1 <= x <= a_max:
                history.append((2, a_lines[x-1]))

            while x < a_max and y < b_max and a_lines[x] == b_lines[y]:
                x += 1
                y += 1
                history.append((0, a_lines[x-1]))
            if V:
                print("new x,y", x, y)

            if x >= a_max and y >= b_max:
                return [(t(i), j) for i, j in history]
            else:
                frontier[k] = (x, history)
    assert False, 'Could not find edit script'

def t(a):
    if a == 0:
        return 'Keep'
    elif a == 1:
        return 'Insert'
    else:
        return 'Remove'

def tc(a):
    if a == 2:
        return 'Remove'
    elif a == 1:
        return 'Insert'
    else:
        return 'Keep'

def myers_diff2(a_lines, b_lines):
    """ See http://www.xmailserver.org/diff2.pdf """
    frontier = {1: (0, ())}
    a_max = len(a_lines)
    b_max = len(b_lines)
    for d in range(0, a_max + b_max + 1):
        for k in range(-d, d + 1, 2):
            go_down = (k == -d or
                    (k != d and frontier[k - 1][0] < frontier[k + 1][0]))
            if go_down:
                old_x, history = frontier[k + 1]
                x = old_x
            else:
                old_x, history = frontier[k - 1]
                x = old_x + 1

            #history = history[:]
            y = x - k
            if 1 <= y <= b_max and go_down:
                history = (history, (1, b_lines[y-1]))
            elif 1 <= x <= a_max:
                history = (history, (2, a_lines[x-1]))

            while x < a_max and y < b_max and a_lines[x] == b_lines[y]:
                x += 1
                y += 1
                history = (history, (0, a_lines[x-1]))

            if x >= a_max and y >= b_max:
                ret_history = []
                while history != ():
                    history, last = history
                    ret_history.append(last)
                return [(t(i), j) for i, j in reversed(ret_history)]
            else:
                frontier[k] = (x, history)
    assert False, 'Could not find edit script'


if __name__ == "__main__":
    main()

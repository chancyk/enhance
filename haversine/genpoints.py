import json
import random


def rand_sign():
    if random.randrange(2) == 0:
        return -1
    else:
        return 1


def rand_value():
    return random.randrange(100000000) * rand_sign() / 1000000


if __name__ == '__main__':
    # generate a list of 10 million random point pairs
    pairs = []
    for i in range(10000000):
        pairs.append({'x0': rand_value(), 'y0': rand_value(), 'x1': rand_value(), 'y1': rand_value()})

    # write it to a JSON file
    with open('data_10000000_flex.json', 'w') as f:
        json.dump({'pairs': pairs}, f, indent=2)

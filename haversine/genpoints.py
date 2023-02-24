import json
import random

# generate a list of 10 million random point pairs
pairs = []
for i in range(10000000):
    pairs.append({'x0': random.random(), 'y0': random.random(), 'x1': random.random(), 'y1': random.random()})

# write it to a JSON file
with open('data_10000000_flex.json', 'w') as f:
    json.dump({'pairs': pairs}, f)

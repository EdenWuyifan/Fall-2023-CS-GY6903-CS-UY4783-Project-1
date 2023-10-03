# Read files under `results` directory
# and calculate the accuracy

import glob
import os
from collections import defaultdict
from prettytable import PrettyTable

files = glob.glob('results/*/*.out')

# read files and calculate accuracy

results = defaultdict(lambda: defaultdict(dict))

for file in sorted(files):
    with open(file, 'r') as f:
        expand = os.path.dirname(file).split('/')[-1]
        filename = os.path.basename(file).split('.')[0]
        keylen, correct = filename.split('_')
        keylen = int(keylen)
        expand = int(expand)
        if expand < 6:
            continue
        answer = f.readlines()[-1].strip().split(' ')[-1]
        if answer == correct:
            results[expand][keylen][correct] = True
        else:
            results[expand][keylen][correct] = False


for expand, result in sorted(results.items(), key=lambda x: x[0]):
    print('Search Space =', expand)
    table = PrettyTable()
    acc_sum = 0.0
    table.field_names = [
        'Key Length',
        'Cipher1', 'Cipher2', 'Cipher3', 'Cipher4', 'Cipher5',
        'Accuracy']
    for keylen, ox in sorted(result.items(), key=lambda x: x[0]):
        row = [keylen] + [None] * 6
        for i, (correct, is_correct) in enumerate(ox.items(), 1):
            row[i] = 'O' if is_correct else 'X'
        accuracy = len([x for x in ox.values() if x]) / 5
        acc_sum += accuracy
        row[-1] = str(accuracy * 100) + '%'
        table.add_row(row)
    table.add_row(['Average'] + [''] * 5 + [f'{acc_sum / len(result) * 100:.1f}%'])
    print(table)


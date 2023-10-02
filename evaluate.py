# Read files under `results` directory
# and calculate the accuracy



# glob files under `results` directory

import glob
import os
from collections import defaultdict
from prettytable import PrettyTable

files = glob.glob('results/*.out')

# read files and calculate accuracy


results = defaultdict(lambda: defaultdict(dict))

for file in sorted(files):
    with open(file, 'r') as f:
        filename = os.path.basename(file).split('.')[0]
        keylen, expand, correct = filename.split('_')
        keylen = int(keylen)
        expand = int(expand)
        answer = f.readlines()[-1].strip().split(' ')[-1]
        if answer == correct:
            results[expand][keylen][correct] = True
        else:
            results[expand][keylen][correct] = False


for expand, result in results.items():
    print('Expand factor', expand)
    table = PrettyTable()
    table.field_names = [
        'Key Length',
        'Correct1', 'Correct2', 'Correct3', 'Correct4', 'Correct5',
        'Accuracy']
    for keylen, ox in result.items():
        row = [keylen] + [None] * 6
        for i, (correct, is_correct) in enumerate(ox.items(), 1):
            row[i] = 'O' if is_correct else 'X'
        accuracy = len([x for x in ox.values() if x]) / 5
        row[-1] = str(accuracy * 100) + '%'
        table.add_row(row)
    print(table)


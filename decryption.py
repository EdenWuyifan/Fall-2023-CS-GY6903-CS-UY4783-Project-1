from itertools import combinations, pairwise
import string
import cProfile

import numpy as np
import argparse

ctoi = { ' ': 0 }

for c in string.ascii_lowercase:
  ctoi[c] = ord(c) - ord('a') + 1

itoc = { i:c for c, i in ctoi.items() }

plains = [
    "barmiest hastes spades fevers cause wisped unconquerable tropical pythagoras rebukingly price ephedra overdecorates linked smitten trickle scanning cognize oaken casework politicize significate influenceable precontrived clockers defalcation workwomen splintery kids placidness harebrained liberalism neuronic clavierist attendees matinees prospectively bubbies longitudinal raving relaxants rigged oxygens chronologist briniest tweezes profaning abeyances fixity gulls coquetted budgerigar drooled unassertive shelter subsoiling surmounted underwear jobbed hobnailed fulfilling jaywalking testabilit",
    "denunciation chronaxy imperilment incurred defrosted protectorates committeemen refractory narcissus bridlers weathercocks occluding orchectomy syncoms beamy opticopupillary acculturation scouting headstrong tosh preconscious weekday reich saddler mercerizes saucepan bifold regenerate chit reviewable easiness brazed essentially idler dependable predicable locales rededicated cowbird kvetched confusingly airdrops dreggier privileges tempter anaerobes glistened sartorial distrustfulness papillary ughs proctoring duplexed pitas traitorously unlighted cryptographer odysseys metamer either meliorat",
    "trenched wickerwork orientation candidnesses nets opalescing pharynxes recompensive incomes shoes porcine pursue blabbered irritable ballets grabbed scything oscillogram despots disarraying curiousest crappier friending wining cypher insubmissive oceanid bowlegs voider recook parochial trop gravidly vomiting hurray friended uncontestable situate fen cyclecars gads macrocosms dhyana overruns impolite europe cynical jennet tumor noddy canted clarion opiner incurring knobbed planeload megohm dejecting campily dedicational invaluable praecoces coalescence dibbuk bustles flay acuities centimeters l",
    "angle obliquely bean mariachi walkup toilettes squib tightwads pipe extents rejoicing nectar asker dreadfuls kidnappers interstate incrusting quintessential neglecter copycat phosphatic streakier frowning phantasmagories supinates imbibers inactivates tingly deserter steerages beggared pulsator laity salvageable bestrode interning stodgily cracker excisions quanted arranges poultries sleds shortly packages apparat fledge alderwomen halvah verdi ineffectualness entrenches franchising merchantability trisaccharide limekiln sportsmanship lassitudes recidivistic locating iou wardress estrus potboi",
    "schmeering institutor hairlocks speeder composers dramatics eyeholes progressives headmaster attractant subjugator peddlery vigil dogfights pixyish comforts aretes brewage felinities salerooms reminiscent hermaphrodism simultaneous spondaics hayfork armory refashioning battering darning tapper pancaked unaffected televiewer mussiness pollbook sieved reclines restamp cohosh excludes homelier coacts refashioned loiterer prospectively encouragers biggest pasters modernity governorships crusted buttoned wallpapered enamors supervisal nervily groaning disembody communion embosoming tattles turbans ",
]


def stream_diff(s1, s2):
    if isinstance(s1, list):
        s1 = np.array(s1)
    if isinstance(s2, list):
        s2 = np.array(s2)
    
    if len(s1) < len(s2):
        s2 = s2[:len(s1)]
    elif len(s1) > len(s2):
        s1 = s1[:len(s2)]

    ds = s1 - s2
    for i, d in enumerate(ds):
        if d < 0:
            ds[i] += 27
    return ds


# Very Hot Function. Any optimization here will speed up the program significantly
def entropy(data):
    counts = np.bincount(data)
    counts = counts[counts > 0]
    probs = counts / len(data)
    ent = -np.sum(probs * np.log(probs))
    return ent


def encode(text):
    return np.array([ctoi[c] for c in text])


def cipher_removed_at(ciphertext, i):
    return ciphertext[:i] + ciphertext[i+1:]


# Optimize the input ciphertext to match the plaintext,
# using entropy change within the searchspace
# Return the range of the ciphertext that could be removed to reduce the entropy
def select_first_local_minimum_removal(ciphertext, plaintext, search_space=30) -> range:

    cipher_stream = encode(ciphertext[:search_space])
    plain_stream = encode(plaintext[:search_space])
    diffs = stream_diff(cipher_stream, plain_stream)
    start_ent = entropy(diffs)

    prev_ent = start_ent

    first_new_ciphertext = ciphertext[1:][:search_space]
    first_new_cipher_stream = encode(first_new_ciphertext)
    diffs = stream_diff(first_new_cipher_stream, plain_stream)
    first_ent = entropy(diffs)

    allow_increasing = first_ent > start_ent * 1.02

    ci = 0
    ents = []
    de_list = [] # delta(entropy)
    while ci < search_space:
        new_ciphertext = cipher_removed_at(ciphertext, ci)
        new_cipher_stream = encode(new_ciphertext[:search_space])
        plain_stream = encode(plaintext[:search_space])
        diffs = stream_diff(new_cipher_stream, plain_stream)
        ent = entropy(diffs)
        ents.append(ent)

        de = (ent - prev_ent) / prev_ent * 100
        de_list.append(de)

        if de > 1.2 and not allow_increasing:
            return range(max(0, ci-4), min(search_space, ci + 1))
        if de < 0:
            allow_increasing = False

        ci += 1
        prev_ent = ent
    
    return range(0)



def detect_start_entropy_anomaly(ciphertext, N=30, threshold=0.186, ax=None, pick_min=False):
    cipher_stream = encode(ciphertext[:N])

    ents = []

    for pi, plaintext in enumerate(plains):
        plain_stream = encode(plaintext[:N])
        diffs = stream_diff(cipher_stream, plain_stream)
        ent = entropy(diffs)
        ents.append(ent)
    
    if ax:
        ax.bar(range(1, 6), ents, width=0.5)
        ent_avg = np.mean(ents)
        ent_std = np.std(ents)

        ax.set_ylim(ent_avg - 5 * ent_std, ent_avg + 3 * ent_std)

        ax.set_title(f'std={ent_std:.4f}')
        ax.axhline(ent_avg, color='r', linestyle='--', label='avg')
        ax.axhline(ent_avg - 2 * ent_std, color='gray', linestyle='-.', label='-2 std')
        ax.axhline(ent_avg + 2 * ent_std, color='gray', linestyle='-.', label='+2 std')
    
    if pick_min:
        return np.argmin(ents)
    
    if np.std(ents) > threshold:
        return np.argmin(ents)

    return -1


def search_fft_fit(ciphertext, plaintext, removal_candids, search_space=30, axes=None):
    # print('candidates', list(removal_candids))
    for ss in range(8, search_space):
        for i, ci in enumerate(removal_candids):
            new_ciphertext = cipher_removed_at(ciphertext, ci)
            cipher_stream = encode(new_ciphertext[:ss])
            plain_stream = encode(plaintext[:ss])
            diffs = stream_diff(cipher_stream, plain_stream)

            freqs = np.fft.fft(diffs)
            mags = np.abs(freqs)
            mags = mags[:len(mags)//2]
            zeros = np.where(mags < 1e-5)[0]

            if len(zeros) >= len(mags) * 0.4:
                return ss, ci
    
    return -1, None


def try_remove_one_random_char(ciphertext):
    for pi, plaintext in enumerate(plains):
        candids = select_first_local_minimum_removal(ciphertext, plaintext)
        l, remove_i = search_fft_fit(ciphertext, plaintext, candids)

        # optimization success
        if l != -1:
            return pi
    
    return -1


def remove_chars(ciphertext, indices, N):
    segments = []
    for i in range(len(indices)):
        if i == 0:
            segments.append(ciphertext[:indices[i]])
        else:
            segments.append(ciphertext[indices[i-1]+1:indices[i]])
    
    segments.append(ciphertext[indices[-1]+1:N + len(indices)])
    return ''.join(segments)


def remove_many_chars_with_fft_test(ciphertext, plaintext, N=48, n_range=range(2, 5), std_multiplier=3):
    plain_stream = encode(plaintext[:N])
    cipher_stream = encode(ciphertext[:N + 4])
    diffs_cache: dict[int, list[int]] = {}

    # precompute diffs
    diffs_cache[0] = stream_diff(cipher_stream, plain_stream) # no removal
    diffs_cache[1] = stream_diff(cipher_stream[1:], plain_stream) # remove first char
    diffs_cache[2] = stream_diff(cipher_stream[2:], plain_stream) # remove first 2 chars
    diffs_cache[3] = stream_diff(cipher_stream[3:], plain_stream) # remove first 3 chars
    diffs_cache[4] = stream_diff(cipher_stream[4:], plain_stream) # remove first 4 chars

    for n_remove in n_range:
        possible_indices = list(combinations(range(N), n_remove))
        ents = np.zeros((len(possible_indices), ))
        for i, indices in enumerate(possible_indices):
            # reuse precomputed diffs
            cursor = 0
            diffs = [None] * N

            diffs[:indices[0]] = diffs_cache[0][:indices[0]]
            cursor = indices[0]
            # diffs.extend(diffs_cache[0][:indices[0]])
            for j, (start, stop) in enumerate(pairwise(indices)):
                diffs[cursor:cursor + stop - start -1] = diffs_cache[j+1][start-j:stop-j-1]
                cursor += stop - start - 1
                # diffs.extend(diffs_cache[j+1][start-j:stop-j-1])
            diffs[cursor:] = diffs_cache[n_remove][indices[-1]-len(indices)+1:]
            # diffs.extend(diffs_cache[n_remove][indices[-1]-len(indices)+1:])
            diffs = np.array(diffs, dtype=int)

            ent = entropy(diffs)
            ents[i] = ent
        
        ent_avg = ents.mean()
        ent_std = ents.std()

        anomaly_indices = np.where(ents < ent_avg - std_multiplier * ent_std)

        # using rank to test low-entropy candidates first
        # does not guarantee to find the answer faster
        
        indices_list = np.array(possible_indices)[anomaly_indices]

        for i, indices in enumerate(indices_list):
            new_ciphertext = remove_chars(ciphertext, indices, N)
            new_cipher_stream = encode(new_ciphertext)
            diffs = stream_diff(new_cipher_stream, plain_stream[:N])

            for M in range(14, N):
                freqs = np.fft.fft(diffs[:M])
                mags = np.abs(freqs)
                mags = mags[:len(mags)//2]
                zeros = np.where(mags < 1e-5)[0]
                # odd_zeros = np.all(mags[1::2] == 0)
                # if odd_zeros:
                #     print(zeros)
                #     print(mags)
                #     return tuple(indices)

                if len(zeros) >= len(mags) // 4:
                    return tuple(indices)

    return -1


def decrypt(ciphertext, N=30, threshold=0.186, std_multiplier=3):
    """
    # Strategy used:
    1. Start Point Entropy Analysis -> Look for anomaly using std
    2. If no anomaly found,
        try to remove a single character and look for anomaly again
        using entropy change and FFT
    3. If no repeating pattern found, try to remove many characters, for low-entropy candidates.
    """

    anomaly = detect_start_entropy_anomaly(ciphertext, N, threshold)

    if anomaly >= 0:
        return (anomaly, 'start anomaly')

    optimization_success_pi = try_remove_one_random_char(ciphertext)

    if optimization_success_pi >= 0:
        return (optimization_success_pi, 'optim')
    
    for i, plaintext in enumerate(plains):
        result = remove_many_chars_with_fft_test(
            ciphertext,
            plaintext,
            N=48,
            std_multiplier=std_multiplier
        )

        if result != -1:
            return (i, 'fft many chars')


    # now it's just a guess
    return (detect_start_entropy_anomaly(ciphertext, N=60, threshold=0.11, pick_min=True), 'last guess: minimum entropy')


if __name__ == "__main__":
    ciphertext = input('Input ciphertext:').strip('\n')

    parser = argparse.ArgumentParser()
    parser.add_argument('--profile', action='store_true', default=False)
    args = parser.parse_args()

    if args.profile:
        profiler = cProfile.Profile()


    if args.profile:
        profiler.enable()
    guess, reason = decrypt(ciphertext)

    if args.profile:
        profiler.disable()
        profiler.print_stats(sort='tottime')

    print(reason)
    print(guess)
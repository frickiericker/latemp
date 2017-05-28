import sys
import io
import numpy as np

def main():
    temp_map = np.loadtxt(sys.stdin)

    num_latitude, _ = temp_map.shape
    num_north = num_latitude // 2
    north = temp_map[:num_north, :]
    south = temp_map[num_north:, :][::-1]
    resolve_holes(north)
    resolve_holes(south)
    south = south[::-1]
    temp_map = np.concatenate((north, south), axis=0)

    stdout = sys.stdout.detach()
    sys.stdout = io.TextIOWrapper(stdout)
    save_tsv(stdout, temp_map)

def resolve_holes(hemi):
    prev = hemi[0]
    for i in range(1, hemi.shape[0]):
        hole = hemi[i] < prev
        hemi[i, hole] = prev[hole]
        prev = hemi[i]

def save_tsv(filepath, array):
    np.savetxt(filepath, array, fmt='%.6g', delimiter='\t')

if __name__ == '__main__':
    main()

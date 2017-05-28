import sys
import io
import numpy as np

def main():
    temp_map = np.loadtxt(sys.stdin)

    min_temp = np.min(temp_map)
    max_temp = np.max(temp_map)
    norm_temp = (max_temp - temp_map) / (max_temp - min_temp)

    max_lat = 90
    lat_map = max_lat * np.sqrt(norm_temp)

    stdout = sys.stdout.detach()
    sys.stdout = io.TextIOWrapper(stdout)
    save_tsv(stdout, lat_map)

def save_tsv(filepath, array):
    np.savetxt(filepath, array, fmt='%.6g', delimiter='\t')

if __name__ == '__main__':
    main()

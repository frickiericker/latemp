import os

import netCDF4 as nc
import numpy as np

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(SCRIPT_DIR)
DATA_DIR = os.path.join(PROJECT_DIR, 'data')

def main():
    lat, _, temp = load_mean_temperature()
    lat = np.tile(lat, (temp.shape[1], 1)).T
    assert lat.shape == temp.shape
    save_tsv('test05-lat.tsv', lat)
    save_tsv('test05-temp.tsv', temp)

def save_tsv(filepath, array):
    np.savetxt(filepath, array, fmt='%.6g', delimiter='\t')

def load_mean_temperature():
    filepath = os.path.join(DATA_DIR, 'air.sig995.2012.nc')
    dataset = nc.Dataset(filepath)
    lat = dataset.variables['lat'][:]
    lon = dataset.variables['lon'][:]
    temp_track = dataset.variables['air'][:]
    mean_temp = np.mean(temp_track, axis=0)
    return center_lon_to_grinidge(lat, lon, mean_temp)

def center_lon_to_grinidge(lat, lon, val):
    meridian = list(lon > 180).index(True)
    lon = np.concatenate((lon[meridian:] - 360, lon[:meridian]), axis=0)
    val = np.concatenate((val[:, meridian:], val[:, :meridian]), axis=1)
    return lat, lon, val

if __name__ == '__main__':
    main()

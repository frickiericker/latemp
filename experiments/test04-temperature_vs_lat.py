import os

import matplotlib as mpl
import matplotlib.pyplot as plt
import netCDF4 as nc
import numpy as np
import shapefile as shp

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(SCRIPT_DIR)
DATA_DIR = os.path.join(PROJECT_DIR, 'data')

def main():
    rings = load_country_rings()
    lat, lon, temp = load_mean_temperature()

    fig, ax = plt.subplots(figsize=(18, 9))
    ax.plot(lat, np.median(temp, axis=1))
    ax.plot(lat, 230 + 70 * (np.cos(lat / 90 * np.pi) + 1) / 2)
    ax.set_xlabel('Latitude (deg.)')
    ax.set_ylabel('Mean temperature (K)')
    fig.savefig('test04.png')

def load_country_rings():
    filepath = os.path.join(DATA_DIR, 'ne_50m_admin_0_countries.shp')
    shapes = shp.Reader(filepath)
    rings = []
    for shape in shapes.iterShapes():
        rings.extend(get_exterior_rings(shape))
    return rings

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

def get_exterior_rings(shape):
    shape_type, coordinates = get_geometry(shape)
    if shape_type == 'Polygon':
        return [get_polygon_exterior_ring(coordinates)]
    if shape_type == 'MultiPolygon':
        return [get_polygon_exterior_ring(polygon) for polygon in coordinates]
    return None

def get_geometry(shape):
    # https://gist.github.com/sgillies/2217756
    shape_type = shape.__geo_interface__['type']
    coordinates = shape.__geo_interface__['coordinates']
    return shape_type, coordinates

def get_polygon_exterior_ring(polygon):
    # http://geojson.org/geojson-spec.html#polygon
    exterior_ring, *interior_rings = polygon
    return exterior_ring

if __name__ == '__main__':
    main()

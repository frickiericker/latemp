import os

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import shapefile as shp

def main():
    rings = load_country_rings()

    fig, ax = plt.subplots(figsize=(18, 9))
    for ring in rings:
        ring = np.array(ring)
        x = ring[:, 0]
        y = ring[:, 1]
        ax.plot(x, y, lw=0.5)
    fig.savefig('test02.png')

def load_country_rings():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    data_dir = os.path.join(project_dir, 'data')
    filepath = os.path.join(data_dir, 'ne_50m_admin_0_countries.shp')
    shapes = shp.Reader(filepath)
    rings = []
    for shape in shapes.iterShapes():
        rings.extend(get_exterior_rings(shape))
    return rings

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

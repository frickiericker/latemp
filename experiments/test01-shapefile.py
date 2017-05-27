import os
import shapefile as shp

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    data_dir = os.path.join(project_dir, 'data')
    filepath = os.path.join(data_dir, 'ne_50m_admin_0_countries.shp')
    shapes = shp.Reader(filepath)
    for shape in shapes.iterShapes():
        shape_type, coordinates = get_geometry(shape)
        print(describe_shape(shape_type, coordinates))

def get_geometry(shape):
    # https://gist.github.com/sgillies/2217756
    shape_type = shape.__geo_interface__['type']
    coordinates = shape.__geo_interface__['coordinates']
    return shape_type, coordinates

def describe_shape(shape_type, coordinates):
    if shape_type == 'Polygon':
        return describe_polygon(coordinates)
    elif shape_type == 'MultiPolygon':
        return describe_multipolygon(coordinates)
    raise Exception('unrecognized shape: ' + shape_type)

def describe_polygon(polygon):
    # http://geojson.org/geojson-spec.html#polygon
    exterior_ring, *interior_rings = polygon
    num_vertices = len(exterior_ring)
    num_holes = len(interior_rings)
    result = str(num_vertices)
    if num_holes > 0:
        result += ':' + str(num_holes)
    return '<' + result + '>'

def describe_multipolygon(polygons):
    return ' '.join(describe_polygon(polygon) for polygon in polygons)

if __name__ == '__main__':
    main()

import segmentation
import numpy as np


def _attractPixels(pix_loc, attractors, value):
    if len(pix_loc) == 0:
        return
    to_process_more = []
    for coord, p in pix_loc:
        values = [
            value(coord[0], coord[1]), # current pixel
            value(coord[0]-1, coord[1]), # left pixel
            value(coord[0], coord[1]-1), # above pixel
            value(coord[0]+1, coord[1]), # right pixel
            value(coord[0], coord[1]+1) # bellow pixel
        ]
        max = 0
        for i in range(1,3):
            if values[i] > values[max]:
                max = i
        for i in range(3,5):
            if values[i] >= values[max]:
                max = i
        if max == 0:
            # We are at the attractor pixel
            if not coord in attractors:
                attractors[coord] = []
            attractors[coord].append(p)
        if max == 1:
            # move left
            to_process_more.append(((coord[0]-1, coord[1]), p))
        if max == 2:
            # move up
            to_process_more.append(((coord[0], coord[1]-1), p))
        if max == 3:
            # move right
            to_process_more.append(((coord[0]+1, coord[1]), p))
        if max == 4:
            # move down
            to_process_more.append(((coord[0], coord[1]+1), p))
    _attractPixels(to_process_more, attractors, value)
    
    
def _mergeAttractors(attractors):
    merged = []
    for coord, pixels in attractors.items():
        done = False
        for i in range(len(merged)):
            minxy = merged[i][0]
            maxxy = merged[i][1]
            if coord[0] >= minxy[0]-1 and coord[0] <= maxxy[0]+1 and coord[1] >= minxy[1]-1 and coord[1] <= maxxy[1]+1:
                merged[i][0] = (min(coord[0], minxy[0]), min(coord[1], minxy[1]))
                merged[i][1] = (max(coord[0], maxxy[0]), max(coord[1], maxxy[1]))
                merged[i][2].extend(pixels)
                done = True
                break
        if not done:
            merged.append([coord, coord, pixels])
    return merged


def _groupToImage(group, pixtoval):
    xmin = group.properties['MinXY'][0]
    ymin = group.properties['MinXY'][1]
    xmax = group.properties['MaxXY'][0]
    ymax = group.properties['MaxXY'][1]
    width = xmax - xmin + 1
    height = ymax - ymin + 1
    im = np.zeros((width, height))
    for p in group.pixel_list:
        x = p.x - xmin
        y = p.y - ymin
        im[x][y] = pixtoval(p)
    return im


def deblend(group, group_prop, pixel_value):
    # Make a function which returns the values of the pixels with shifted x,y
    # and returns -1 outside the image
    im = _groupToImage(group, lambda p:p.properties[pixel_value])
    minxy = group.properties['MinXY']
    maxxy = group.properties['MaxXY']
    def value(x,y):
        if (x < minxy[0] or x > maxxy[0] or y < minxy[1] or y > maxxy[1]):
            return -1
        return im[x - minxy[0]][y - minxy[1]]
    
    # Regroup the pixels based on attractors
    pix_loc = [((p.x, p.y), p) for p in group.pixel_list]
    attractors = {}
    _attractPixels(pix_loc, attractors, value)
    merged = _mergeAttractors(attractors)
    
    # If we end up with a single group return the original group
    if len(merged) == 1:
        return [group]
    
    # If we are here we need to construct the new groups
    result = []
    i = 0
    for minxy,maxxy,pixels in merged:
        g = segmentation.PixelGroup()
        i += 1
        g.id = str(group.id) + '_' + str(i)
        g.pixel_list = pixels
        for name, prop in group_prop.items():
                v = prop.initValue()
                for p in g.pixel_list:
                    v = prop.nextValue(v, p)
                g.properties[name] = v
        result.append(g)
    return result
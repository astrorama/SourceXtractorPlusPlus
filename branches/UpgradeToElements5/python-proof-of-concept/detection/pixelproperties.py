def getImageFlag(im):
    return lambda x,y : im[y][x] > 0 if (x<len(im[0]) and y<len(im)) else False

def getImageValue(im):
    return lambda x,y : im[y][x]
class SubtractImage:
    
    def __init__(self, im, bg):
        self._im = im
        self._bg = bg
        
    def value(self, x, y):
        return self._im[y][x] - self._bg(x, y)
    
    def __len__(self):
        return len(self._im)
    
    def __getitem__(self, y):
        return SubtractImage._GetItemHelper(self, y)
    
    class _GetItemHelper:
        def __init__(self, parent, y):
            self._parent = parent
            self._y = y
        def __len__(self):
            return len(self._parent._im[self._y])
        def __getitem__(self, x):
            return self._parent.value(x, self._y)
    
    def __iter__(self):
        return SubtractImage._ImgIter(self)
    
    class _ImgIter:
        def __init__(self, parent):
            self._parent = parent
            self._y = -1
            self._ymax = len(parent._im) - 1
        def next(self):
            if self._y == self._ymax:
                raise StopIteration
            self._y += 1
            return SubtractImage._IterHelper(self._parent, self._y)
        
    class _IterHelper:
        def __init__(self, parent, y):
            self._parent = parent
            self._y = y
        def __iter__(self):
            return SubtractImage._RowIter(self._parent, self._y)
        
    class _RowIter:
        def __init__(self, parent, y):
            self._parent = parent
            self._y = y
            self._x = -1
            self._xmax = len(parent._im[y]) - 1
        def next(self):
            if self._x == self._xmax:
                raise StopIteration
            self._x += 1
            return self._parent.value(self._x, self._y)

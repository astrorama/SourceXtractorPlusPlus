import itertools
from abc import ABCMeta, abstractmethod


class PixelGroup:
    
    def __init__(self):
        self.start = '*'
        self.end = '*'
        self.id = None
        self.pixel_list = []
        self.properties = {}


class Pixel:
    
    def __init__(self, x, y, value, properties):
        self.x = x
        self.y = y
        self.value = value
        self.properties = properties


class GroupProperty:
    __metaclass__ = ABCMeta
    
    @abstractmethod
    def initValue(self):
        pass
    
    @abstractmethod
    def nextValue(self, current_value, pixel):
        pass


class SegmentationAlgo:
    """Implementation of the Lutz algorithm for pixel group detection."""
    
    def __init__(self, threshold=lambda x,y,v:v>0, pixel_properties={}, group_properties=[]):
        """Creates a new SegmentationAlgo instance.
        
        Args:
            threshold :
                A function returning True if a pixel is of type object or False
                if it is of type non-object. It will be called with the
                following parameters:
                    - x : The x coordinate of the pixel
                    - y : The y coordinate of the pixel
                    - value : The value of the pixel
                The coordinates reported are relative to the first pixel the
                algorithm processes ((0,0) is the first pixel). The default
                behavior is to check for positive values.
            pixel_properties :
                A dictionary with instructions for setting the properties of
                each pixel. The keys are strings containing the name of the
                property and the values are methods for calculating the value of
                the pixel property. These functions are called with the
                following parameters:
                    - x : The x coordinate of the pixel
                    - y : The y coordinate of the pixel
                The coordinates reported are relative to the first pixel the
                algorithm processes ((0,0) is the first pixel). The default
                behavior is to not add any pixel properties.
            group_properties :
                A dictionary with instructions for setting the properties of
                each pixel group. The keys are strings containing the name of
                the property and the values are objects which implement the
                GroupProperty interface. The default behavior is to not add any
                group properties.
          
        """
        self._threshold = threshold
        self._pixel_properties = pixel_properties
        self._group_properties = group_properties
        
        
    def scan(self, image, pixel_group_listener, progress_listener=None):
        """Scans an image for pixel groups.
        
        Args:
            image :
                The image to process. It must be an iterable object, which
                iterates through its rows. The rows themselves must be iterable
                through their pixel values.
            pixel_group_listener :
                The function which is notified when the algorithm has detected a
                pixel group object. It will be called with the following
                parameters:
                    - group : The newly detected pixel group
            progress_listener :
                A function notified every time a full row has been progressed.
                It will be called with the following parameters:
                    - progress : Number of already processed lines
        """
        
        COMPLETE = 1
        INCOMPLETE = 2
        NONOBJECT = 3
        OBJECT = 4
        
        self.next_obj_id = 1
        def _publishGroup(group):
            group.id = self.next_obj_id
            self.next_obj_id += 1
            for name, group_prop in self._group_properties.items():
                v = group_prop.initValue()
                for p in group.pixel_list:
                    v = group_prop.nextValue(v, p)
                group.properties[name] = v
            pixel_group_listener(group)
        
        marker = []
        ps_stack = []
        group_stack = []
        inc_group_map = {}
        
        y = 0 # Used for row counting
        for row in image:
            ps = COMPLETE
            cs = NONOBJECT
            
            x = 0 # Used for pixel counting
            for value in itertools.chain(row, [0]):
                # We iterate through the row pixels and an extra zero pixel
                
                if len(marker) == x:
                    # Initialize the marker (first time accessed)
                    marker.append(0)
                    
                last_marker = marker[x]
                marker[x] = 0
                
                in_object = self._threshold(x, y, value)
                if in_object:
                    # We have an object pixel
                    
                    if cs != OBJECT:
                        # Previous pixel not object, start new segment
                        cs = OBJECT
                        
                        if ps == OBJECT: 
                            # Pixel touches segment on preceding scan
                                
                            if group_stack[-1].start == '*':
                                # First pixel of object on current scan
                                marker[x] = 'S'
                                group_stack[-1].start = x
                            else:
                                marker[x] = 'S0'
                                
                        else:
                            # Start of completely new pixel group
                            ps_stack.append(ps)
                            ps = COMPLETE
                            group_stack.append(PixelGroup())
                            marker[x] = 'S'
                            group_stack[-1].start = x
                    
                if last_marker != 0:
                    # There is a marker from the previous scan to process
                    # This is done for both object and non-object pixels

                    if last_marker == 'S':
                        # Start of pixel group on preceding scan
                        ps_stack.append(ps)
                        if cs == NONOBJECT:
                            # The S marker is the first encounter with this group
                            ps_stack.append(COMPLETE)
                            group_stack.append(inc_group_map.pop(x))
                            group_stack[-1].start = '*'
                        else:
                            # Add group to current group
                            group_stack[-1].pixel_list.extend(inc_group_map.pop(x).pixel_list)
                        ps = OBJECT


                    if last_marker == 'S0':
                        # Start of secondary segment of group on preceding scan

                        if cs == OBJECT and ps == COMPLETE:
                            # Current group is joined to preceding group
                            ps_stack.pop()
                            old_group = group_stack.pop()
                            group_stack[-1].pixel_list.extend(old_group.pixel_list) # Merge pixel lists
                            if group_stack[-1].start == '*':
                                group_stack[-1].start = old_group.start
                            else:
                                marker[old_group.start] = 'S0'
                        
                        ps = OBJECT

                    if last_marker == 'F0':
                        ps = INCOMPLETE

                    if last_marker == 'F':
                        ps = ps_stack.pop()

                        if cs == NONOBJECT and ps == COMPLETE:
                            # If no more of current group to come then finish it
                            old_group = group_stack.pop()
                            if old_group.start == '*':
                                # Pixel group completed
                                _publishGroup(old_group)
                            else:
                                marker[old_group.end] = 'F'
                                inc_group_map[old_group.start] = old_group
                                
                            ps = ps_stack.pop()
                
                if in_object:
                    # Update current group by current pixel
                    pix_prop = {k:f(x,y) for k,f in self._pixel_properties.items()}
                    group_stack[-1].pixel_list.append(Pixel(x, y, value, pix_prop))
                
                else:
                    # The current pixel is not object
                    
                    if cs == OBJECT:
                        # Previous pixel was object. Finish segment
                        cs = NONOBJECT
                        
                        if ps != COMPLETE:
                            # End of segment but not necessarily of section
                            marker[x] = 'F0'
                            group_stack[-1].end = x
                        
                        else:
                            # End of final segment of group section
                            ps = ps_stack.pop()
                            marker[x] = 'F'
                            old_group = group_stack.pop()
                            inc_group_map[old_group.start] = old_group
                    
                    
                
                # Finished processing the pixel
                x += 1
            
            # Finished processing the row, report to the progress_listener
            y += 1
            if progress_listener:
                progress_listener(y)
        
        # Process the pixel groups left in the inc_group_map
        for group in inc_group_map.values():
            _publishGroup(group)
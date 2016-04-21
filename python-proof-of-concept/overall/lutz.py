import datamodel as dm
import tasks as tsk
import segmentation as segm

import itertools

class _PixelGroup:
    
    def __init__(self):
        self.start = '*'
        self.end = '*'
        self.pixel_list = []

class Lutz(segm.Segmentation):
    """Segmentation implementation based on the Lutz algorithm"""
    
    def __init__(self):
        super(Lutz, self).__init__()
    
    def scan(self,image):
        
        COMPLETE = 1
        INCOMPLETE = 2
        NONOBJECT = 3
        OBJECT = 4
        
        def _publishGroup(group):
            source = dm.PixelSource(group.pixel_list, tsk.task_registry)
            self._notifyListeners(source)
        
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
                
                in_object = value > 0
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
                            group_stack.append(_PixelGroup())
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
                    group_stack[-1].pixel_list.append((x, y))
                
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
#        
        # Process the pixel groups left in the inc_group_map
        for group in inc_group_map.values():
            _publishGroup(group)
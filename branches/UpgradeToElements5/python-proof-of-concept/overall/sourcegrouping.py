import datamodel as dm
import maincomponents as mc

class CriteriaAllSources(mc.SelectionCriteria):
       def mustBeProcessed(self, source):
           return True
       
class GroupingCriteria(object):
    def shouldGroup(self, pixel_source_list, pixel_source):
        raise NotImplementedError()

class SourceGrouping(mc.PixelSourceListener, mc.ProcessSourceListener, mc.PixelSourceListEventGenerator):
    
    def __init__(self, grouping_criteria):
        super(SourceGrouping, self).__init__()
        self.pixel_source_lists = []
        self.grouping_criteria = grouping_criteria

    def handlePixelSource(self, pixel_source):
        lists_to_merge = [l for l in self.pixel_source_lists if self.grouping_criteria.shouldGroup(l, pixel_source)]
        
        if lists_to_merge:
            main_list = lists_to_merge[0]
            main_list.append(pixel_source)

            for l in lists_to_merge[1:]:
                self.pixel_source_lists.remove(l)
                main_list.merge(l)
        else:
            self.pixel_source_lists.append(dm.PixelSourceList([pixel_source]))
            
                    
    def processSources(self, selection_criteria):
        # construct a list of PixelSourceLists with at least one source that must be processed
        pixel_source_lists_to_process = [psl for psl in self.pixel_source_lists
                                          if next((s for s in psl if selection_criteria.mustBeProcessed(s)), None)]
        
        for psl in pixel_source_lists_to_process:
            self.pixel_source_lists.remove(psl)
            self._notifyListeners(psl)
        

class SourceGroupingFactory(mc.Configurable):
    
    class NeverGroupCriteria(GroupingCriteria):
        def shouldGroup(self, pixel_source_list, pixel_source):
            return False
        
    class OverlappingBoundariesCriteria(GroupingCriteria):
        def shouldGroup(self, pixel_source_list, pixel_source):
            boundaries = pixel_source.getProperty('PixelBoundaries')
            
            overlap_list_bounding_box =  not (boundaries.getMin()[0] > pixel_source_list.getMax()[0] or 
                                              boundaries.getMax()[0] < pixel_source_list.getMin()[0] or
                                              boundaries.getMin()[1] > pixel_source_list.getMax()[1] or 
                                              boundaries.getMax()[1] < pixel_source_list.getMin()[1])
            
            # if we overlap the PixelSourceList's bounding box we check its individual sources for an overlap 
            if overlap_list_bounding_box:
                for s in pixel_source_list:
                    other_boundaries = s.getProperty('PixelBoundaries')
                    if not (boundaries.getMin()[0] > other_boundaries.getMax()[0] or 
                            boundaries.getMax()[0] < other_boundaries.getMin()[0] or
                            boundaries.getMin()[1] > other_boundaries.getMax()[1] or 
                            boundaries.getMax()[1] < other_boundaries.getMin()[1]):
                        return True
                    
            return False
    
    def __init__(self, task_registry):
        super(SourceGroupingFactory, self).__init__()
        self.task_registry = task_registry

    def getSourceGrouping(self):
        return SourceGrouping(self.OverlappingBoundariesCriteria())


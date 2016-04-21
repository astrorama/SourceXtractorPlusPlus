import datamodel as dm
import maincomponents as mc

class CriteriaAllSources(mc.SelectionCriteria):
       def mustBeProcessed(self, source):
           return True

class SourceGrouping(mc.PixelSourceListener, mc.ProcessSourceListener, mc.SourceGroupEventGenerator, mc.Configurable):
    
    def __init__(self, task_registry):
        super(SourceGrouping, self).__init__()
        self.sources = []
        self.task_registry = task_registry

    def handlePixelSource(self, pixel_source):
        self.sources.append(pixel_source)
        
    def processSources(self, selection_criteria):
        for source in self.sources:
            if selection_criteria.mustBeProcessed(source):
                self._notifyListeners(dm.SourceGroup([source], self.task_registry))

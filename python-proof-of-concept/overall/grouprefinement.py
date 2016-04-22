import maincomponents as mc
import configuration as conf
import datamodel as dm
import tasks as tsk


class SourceGroupRefineAction(object):
    """Interface of classes than can be used for refining a SourceGroup by the
    SourceGroupRefinement"""
    
    def refine(self, source_group):
        """This method should be implemented to check the sources inside the
        group and try to identify hidden sources inside the group, using more
        sophisticated methods, like modeling (redeblending). If no refinement is
        required, the input should just be forwarded, If new sources are detected
        a new group should be created (via the appropriate SourceGroup constructor)
        and returned."""
        raise NotImplementedError()
    
    
    
        
        
class SourceGroupRefinement(mc.SourceGroupEventGenerator, mc.PixelSourceListListener):
    """Class responsible for the group refinement. It is a PixelSourceListListener
    so it can be connected with the source of the PixelSourceLists. For each list
    it performs a set of refinement actions, it wraps the final result in a
    SourceGroup and it notifies its own listeners for it"""
    
    def __init__(self, action_list):
        super(SourceGroupRefinement, self).__init__()
        self.action_list = action_list
    
    def handlePixelSourceList(self, pixel_source_list):
        for action in self.action_list:
            pixel_source_list = action.refine(pixel_source_list)
        # Here all actions are done, so group the sources in a SourceGroup and
        #notify the listeners
        self._notifyListeners(dm.SourceGroup(pixel_source_list, tsk.task_registry))
    
    
    
    
class SourceGroupRefinementFactory(mc.Configurable):
    
    def getSourceGroupRefinement(self):
        # At the moment we return a refinement which has no actions, so it just
        # forwards its inputs
        return SourceGroupRefinement([])

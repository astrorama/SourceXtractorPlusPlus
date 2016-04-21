import maincomponents as mc
import configuration as conf
import datamodel as dm


class SourceGroupRefineAction(object):
    """Interface of classes than can be used for refining a SourceGroup by the
    SourceGroupRefinement"""
    
    def refine(self, source_group):
        """This method should be implemented to check the sources inside the
        group and try to identify hidden sources inside the group, using more
        sophisticated methods, like modeling (redeblending). If no refinement is
        required, the input should just be forwared, If new sources are detected
        a new group should be created (via the appropriate SourceGroup constructor)
        and returned."""
        raise NotImplementedError()
    
    
    
        
        
class SourceGroupRefinement(mc.SourceGroupEventGenerator, mc.SourceGroupListener):
    """Class responsible for the group refinement. It is a SourceGroupListener
    so it can be connected with the source of the SourceGroups. For each group
    it performs a set of refinement actions and it notifies its own
    listeners for the outcome"""
    
    def __init__(self, action_list):
        super(SourceGroupRefinement, self).__init__()
        self.action_list = action_list
    
    def handleSourceGroup(self, source_group):
        for action in self.action_list:
            action.refine(source_group)
        # Here all actions are done, so notify the listeners
        self._notifyListeners(source_group)
    
    
    
    
class SourceGroupRefinementFactory(mc.Configurable):
    
    def getSourceGroupRefinement(self):
        # At the moment we return a refinement which has no actions, so it just
        # forwards its inputs
        return SourceGroupRefinement([])

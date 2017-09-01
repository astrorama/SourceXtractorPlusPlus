import maincomponents as mc
import configuration as conf
import datamodel as dm
import tasks as tsk


class PixelSourceListRefineAction(object):
    """Interface of classes than can be used for refining a PixelSourceList by the
    SourceGroupRefinement"""
    
    def refine(self, pixel_source_list):
        """This method should be implemented to check the sources inside the
        list and try to refine them or to identify hidden sources using more
        sophisticated methods, like modeling (redeblending)"""
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
            action.refine(pixel_source_list)
        # Here all actions are done, so group the sources in a SourceGroup and
        #notify the listeners
        self._notifyListeners(dm.SourceGroup(pixel_source_list, tsk.task_registry))
    
    
    

class MergeNeighborsAction(PixelSourceListRefineAction):
    """Merge neighbors implementation that it just checks if their total pixel
    counts are more than 10 times different"""
    
    def _areSingleSource(self, s1, s2):
        pass
    
    def refine(self, pixel_source_list):
        # If we have a single source return
        if len(pixel_source_list) <= 1:
            return
        
        # Create a map of the sources which should be merged
        to_be_merged = []
        for i in range(len(pixel_source_list)):
            to_be_merged.append([])
            s1 = pixel_source_list[i]
            s1_pix_no = len(s1.getPixelList())
            s1_total_value = sum(s1.getProperty('DetectionFramePixelValues').getPixelValues())
            for j in range(i+1, len(pixel_source_list)):
                s2 = pixel_source_list[j]
                s2_pix_no = len(s2.getPixelList())
                s2_total_value = sum(s2.getProperty('DetectionFramePixelValues').getPixelValues())
                if 10 * s2_total_value < s1_total_value or 10 * s1_total_value < s2_total_value:
                    to_be_merged[i].append(j)
        
        # method for iteratively analyse the to_be_merged
        def getGroupToMerge(result, i, grouping_map):
            # If this source groups with nothing, continue
            if not grouping_map[i]:
                return None
            result.add(i)
            for j in grouping_map[i]:
                result.add(j)
                getGroupToMerge(result, j, grouping_map)
            grouping_map[i] = []
            
            
        for i in range(len(to_be_merged)):
            group = set()
            getGroupToMerge(group, i, to_be_merged)
            if len(group) == 0:
                continue
            # Get the sources
            sources = [pixel_source_list[j] for j in group]
            # Remove them from the list
            for s in sources:
                pixel_source_list.remove(s)
            # Create a new one and add it
            new_pix_list = []
            for s in sources:
                new_pix_list.extend(s.getPixelList())
            pixel_source_list.append(dm.PixelSource(new_pix_list, tsk.task_registry))
            



class SourceGroupRefinementConfig(conf.Configuration):
    """Configuration class which is responsible to handle the user input to decide
    which PixelSourceListRefineActions will be used"""
    
    def __init__(self, manager):
        super(SourceGroupRefinementConfig, self).__init__(manager)
        self.action_list = []
    
    def getProgramOptions(self):
        return [
            conf.OptionDescr('NEIGHBOR_MERGING', str, "One of OFF, ON")
        ]
    
    def preInitialize(self, user_values):
        if not 'NEIGHBOR_MERGING' in user_values:
            raise Exception('Missing obligatory parameter NEIGHBOR_MERGING')
        if not user_values['NEIGHBOR_MERGING'] in ('OFF', 'ON'):
            raise Exception("Invalid NEIGHBOR_MERGING parameter")
    
    def initialize(self, user_values):
        # First chose which deblending algorithm we use
        if user_values['NEIGHBOR_MERGING'] == 'ON':
            self.action_list.append(MergeNeighborsAction())
                
    def getRefinementActions(self):
        return self.action_list

    
    
class SourceGroupRefinementFactory(mc.Configurable):
    
    def reportConfDependencies(self, config_manager):
        config_manager.registerConfiguration(SourceGroupRefinementConfig)
    
    def configure(self, config_manager):
        self.action_list = config_manager.getConfiguration(SourceGroupRefinementConfig).getRefinementActions()
    
    def getSourceGroupRefinement(self):
        return SourceGroupRefinement(self.action_list)

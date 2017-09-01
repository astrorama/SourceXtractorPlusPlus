import maincomponents as mc
import configuration as conf


class OutputColumnManager(object):
    
    def __init__(self):
        self.col_names = set()
        self.type_dict = {}
        self.func_dict = {}
    
    def registerColumn(self, col_name, prop_type, func):
        if col_name in self.col_names:
            raise Exeption('Column '+col_name+' method duplication (properties '+self.type_dict+' and '+prop_type+')')
        self.col_names.add(col_name)
        self.type_dict[col_name] = prop_type
        self.func_dict[col_name] = func
    
    def getColumnNames(self):
        return self.col_names
    
    def convertToColumns(self, source, col_names):
        columns = {}
        for c in col_names:
            prop_type = self.type_dict[c]
            prop = source.getProperty(prop_type.__name__)
            value = self.func_dict[c](prop)
            columns[c] = value
        return columns
    

output_column_manager = OutputColumnManager()



class CatalogColumnsConfig(conf.Configuration):
    
    def __init__(self, manager):
        super(CatalogColumnsConfig, self).__init__(manager)
    
    def getProgramOptions(self):
        return [
            conf.OptionDescr('OUT_COLUMNS', str, "The columns of the output catalog")
        ]
    
    def preInitialize(self, user_values):
        if not 'OUT_COLUMNS' in user_values:
            self.col_names = []
        else:
            self.col_names = user_values['OUT_COLUMNS'].split()
        for c in self.col_names:
            if not c in output_column_manager.getColumnNames():
                raise Exception('Unknown output catalog column name: '+c)
    
    def getColNames(self):
        return self.col_names
    
    
    
    

class OutputHandler(mc.Configurable, mc.SourceListener):
    
    def __init__(self):
        self.print_title = True
        self.next_id = 1
    
    def reportConfDependencies(self, config_manager):
        config_manager.registerConfiguration(CatalogColumnsConfig)
    
    def configure(self, config_manager):
        self.col_names = config_manager.getConfiguration(CatalogColumnsConfig).getColNames()
    
    def handleSource(self, source):
        if self.print_title:
            self.print_title = False
            print ''
            print '# ID'
            for c in self.col_names:
                print '#', c
        print self.next_id, '\t',
        self.next_id += 1
        if self.col_names:
            values = output_column_manager.convertToColumns(source, self.col_names)
            for c in self.col_names:
                print values[c], '\t', 
        print ''
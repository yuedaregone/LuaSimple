
import CGILua
def initialize(context):
    context.registerClass(
        CGILua.lua, 
        meta_type='CGILua Installation',
        permission='CGILua Installation',
        constructors=(CGILua.lua_addForm, CGILua.lua_add),
        )
 
import os
import subprocess
import bpy
#import mathutils

from bpy.props import (StringProperty,
                       BoolProperty,
                       IntProperty,
                       FloatProperty,
                       FloatVectorProperty,
                       EnumProperty,
                       PointerProperty,
                       )
from bpy.types import (Panel,
                       Operator,
                       AddonPreferences,
                       PropertyGroup,
                       )

bl_info = {
    "name": "Just4RIR",
    "description": "Launch Ray Tracing calculation for acoustic Room Impulse Response generation and display results",
    "author": "Robin GUEGUEN",
    "version": (0, 0),
    "blender": (2, 77, 0),
    "warning": "",
    "category": "3D View"}


## Open executable file
def OPEN(file):
    #os.system(file)
    subprocess.Popen(file)


class Settings(PropertyGroup):

    file = StringProperty(name = "file",
                          default = "",
                          description = "File to be lauched",
                          subtype = 'FILE_PATH'
    )
    file2 = StringProperty(name = "file2",
                          default = "",
                          description = "File to be imported",
                          subtype = 'FILE_PATH'
    )   

class RayTracingPanel(bpy.types.Panel):
    bl_label = "Just4RIR"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'TOOLS'
    bl_category = "Tools"
    
    def draw(self, context):
        mytool = context.scene.my_tool
        
        # FILE
        self.layout.prop(mytool, "file", text="App file")
        
        # export buttons
        TheCol = self.layout.column(align=True)
        
        row = TheCol.row(align=True)
        row.operator("exp.src", text="EXPORT SOURCE", icon="MESH_UVSPHERE")
        row.operator("exp.listener", text="EXPORT LISTENER", icon="MESH_MONKEY")
        # run button
        TheCol = self.layout.column(align=True)
        TheCol.operator("run.rt", text="EXPORT MESH & RUN", icon="PLAY")
                
        #import
        self.layout.prop(mytool, "file2", text="Import Obj")
        
        TheCol = self.layout.column(align=True)
        TheCol.operator("imp.rt", text="IMPORT", icon="IMPORT")
        
#        TheCol = self.layout.column(align=True)
#        TheCol.operator("sph.rt", text="Spherical SI", icon="MESH_UVSPHERE")

class runRayTracing(bpy.types.Operator):
    bl_idname = "run.rt"
    bl_label = "Launch Ray Tracing algorythme"

    def invoke(self, context, event):
        T = context.scene.my_tool
        inFile = T.file # recupération du répertoire de l'app
        objFile = T.file[:-8] + "mesh4RIR.obj"
        try:            
            bpy.ops.export_scene.obj(filepath = objFile,use_selection=True,use_mesh_modifiers=True,use_triangles=True)        

##### Attention le chemin du repertoire doit être complet #######
            
            OPEN(inFile)

        except AttributeError:
            print("OUPS !")
        return {"FINISHED"}
    
class expSource(bpy.types.Operator):
    bl_idname = "exp.src"
    bl_label = "Export Source"

    def invoke(self, context, event):
        T = context.scene.my_tool
        inFile = T.file # recupération du répertoire de l'app
        srcFile = T.file[:-8] + "src4RIR.obj"
        try:            
            bpy.ops.export_scene.obj(filepath = srcFile,use_selection=True,use_mesh_modifiers=True,use_triangles=True)          

        except AttributeError:
            print("OUPS !")
        return {"FINISHED"}
    
class expListener(bpy.types.Operator):
    bl_idname = "exp.listener"
    bl_label = "Export Listener"

    def invoke(self, context, event):
        T = context.scene.my_tool
        inFile = T.file # recupération du répertoire de l'app
        ltnFile = T.file[:-8] + "listener4RIR.obj"
        try:            
            bpy.ops.export_scene.obj(filepath = ltnFile,use_selection=True,use_mesh_modifiers=True,use_triangles=True)          

        except AttributeError:
            print("OUPS !")
        return {"FINISHED"}

class impRayTracing(bpy.types.Operator):
    bl_idname = "imp.rt"
    bl_label = "Import raytracing algorythme Output"

    def invoke(self, context, event):
        T = context.scene.my_tool
        
        count1=0
        obs = bpy.data.objects
        for ob in obs:
            if "SI_Projected" in ob.name:
                ob.select = True 
                bpy.context.scene.objects.active = ob
                count1=count1+1 
        
        inFile = T.file2 # recupération du répertoire de l'app
        try:            
             bpy.ops.import_scene.obj(filepath = inFile)

        except AttributeError:
            print("OUPS !")
            
        
        bpy.ops.object.select_all(action='DESELECT')
        count2=0
        for ob in obs:
            if "SI_Projected" in ob.name:
                ob.select = True 
                bpy.context.scene.objects.active = ob 
                count2=count2+1
                
        try:
            if count2>count1:            
                bpy.ops.object.editmode_toggle()
                bpy.context.space_data.pivot_point = 'INDIVIDUAL_ORIGINS'
                bpy.ops.transform.resize(value=(2,2,2), constraint_axis=(False, False, False), constraint_orientation='GLOBAL', mirror=False, proportional='DISABLED', proportional_edit_falloff='SMOOTH', proportional_size=2.14359)
                bpy.ops.object.editmode_toggle()
                bpy.ops.object.modifier_add(type='SOLIDIFY')
                bpy.context.object.modifiers["Solidify"].thickness = 1
                bpy.context.object.modifiers["Solidify"].offset = 0
                bpy.ops.object.modifier_add(type='SUBSURF')
                bpy.context.object.modifiers["Subsurf"].levels = 2
        except AttributeError:
            print("OUPS !")
                    
        return {"FINISHED"}

#class sphericalSI(bpy.types.Operator):
#    bl_idname = "sph.rt"
#    bl_label = "Convert to spherical SI"

#    def invoke(self, context, event):
#        
#        # si on veut des sources images rondes
#        try:            
#            bpy.ops.object.editmode_toggle()
#            bpy.context.space_data.pivot_point = 'INDIVIDUAL_ORIGINS'
#            bpy.ops.transform.resize(value=(2,2,2), constraint_axis=(False, False, False), constraint_orientation='GLOBAL', mirror=False, proportional='DISABLED', proportional_edit_falloff='SMOOTH', proportional_size=2.14359)
#            bpy.ops.object.editmode_toggle()
#            bpy.ops.object.modifier_add(type='SOLIDIFY')
#            bpy.context.object.modifiers["Solidify"].thickness = 1
#            bpy.context.object.modifiers["Solidify"].offset = 0
#            bpy.ops.object.modifier_add(type='SUBSURF')
#            bpy.context.object.modifiers["Subsurf"].levels = 2
#        except AttributeError:
#            print("OUPS !")
#        return {"FINISHED"}    

bpy.utils.register_class(expListener)
bpy.utils.register_class(expSource)
bpy.utils.register_class(impRayTracing)
bpy.utils.register_class(runRayTracing)
bpy.utils.register_class(RayTracingPanel)
#bpy.utils.register_class(sphericalSI)

def register():
    bpy.utils.register_module(__name__)
    bpy.types.Scene.my_tool = PointerProperty(type=Settings)

def unregister():
    bpy.utils.unregister_module(__name__)
    del bpy.types.Scene.my_tool

if __name__ == "__main__":
    register()

# Just4RIR
Library for room acoustic analyse (linked with Blender)

Quick use :

- Download the .zip file corresponding to your OS in https://github.com/RobinGueguen/Just4RIR.
- Download Just4RIR_addon.py and add it in your Blender interface.
- In Blender, in the App File section of Just4RIR tool, select the executable Just4RIR.
- Rename materials of your room with a name that begin with a reference from Material.txt file followed by a space (or create a new reference if needed).
- Add and place objects containing the word "source" and other containing "listener" inside your room. The radius of the listener will impact the measure (1m is a good value).
- Select all your scene (room, obstacles, sources and listeners).
- Click on EXPORT MESH & RUN.
- Check your parameters on Just4RIR interface.
- Click on Compute !

Then, it depends of what you need :
- To analyse the impulse response : Go on the Analyse section and click on Display Echogram.
- To listen the reverberated audio : Go on the Audio section, select your audio file and click on  Generate reverberated audio.
- To visualize Rays or Images-sources : Go on the Export section and click the corresponding button. Then, go on Blender, in the "Import Obj" field of the Just4RIR tool, select mesh4RIR\_EXPORT.obj and click on IMPORT. 

Or read the user manual : https://github.com/RobinGueguen/Just4RIR/blob/master/Just4RIR%20-%20User%20Manuel.pdf

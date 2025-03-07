ASSIGNMENT 3

How to Run :
To run our scene, or any other of the scenegraphs, the command is in the form: programname path-to-scenegraph 
    => .\Scenegraphs.exe .\scenegraphmodels\scene.txt
For face-hierarchy:
    => .\Scenegraphs.exe .\scenegraphmodels\face-hierarchy-commands.txt        

Important files:
scene.txt --> the composite scene that holds all components joined together 
base.txt --> the rectangular base as specified in the instruction
tower.txt --> the building as specified in the instruction
two-humans-commands.txt --> the composite scene that holds two humans posing
    human-one-pose.txt --> human one with unique pose
    human-two-pose.txt --> human two with unique pose

In the view,onMousePress() records the current mouse position and sets isDraging to true, which enables
the calculation of how much the mouse moves in the onMouseMove() function. This movement is converted 
to rotation angles and given these angles the rotations matrix for x and y are updated. This is reflected
in the display which applies the transormation and produces the live interactive effect. 


Work split:
All work was split evenly - we both met up in person nearly all sessions and pair programmed. 


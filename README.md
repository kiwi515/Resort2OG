# Resort2OG
Command-line tool for converting the KCL and PMP files seen in Wii Sports Resort golf courses to files usable in Wii Sports 
  
#### Progress (Complete!)  
  
- [X] KCL support  
- [X] PMP support  
    
## Usage
Resort2OG is run through Command Prompt as it is a command-line tool.  
Syntax is as follows: `Resort2OG.exe kcl/pmp myFile.kcl/myFile.pmp`  
  
## PMP
PMP files in Golf are used for object placement, some examples being the player start position, all possible pin positions, trees, etc. 
- PMP object placement is great for objects that are re-used throughout multiple courses, and ones that would be redundant to include as part of the course model. 
- Every PMP object has a group ID, and an object ID.   

This tool converts all objects' group and object IDs to be valid IDs when used in the original Wii Sports.
  
## KCL
KCL files are the Wii's most common collision filetype, and has been seen on other consoles as well, such as the DS.  
- It contains a collision model, often times being a simpler version of the course model.  
- The KCL format uses spatial indexing and an octree to allow for very efficient collision.  

This tool converts the collision flags of the KCL model to match the ones used in the original Wii Sports.  
  
## Credits
- GibHaltmannKill for the documentation used to work with PMP files; PMP support would not be possible without this knowledge
- Jimmy Kaz for testing and documenting the Resort KCL values so I didn't spend many hours doing so myself

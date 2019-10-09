## iLAMP: Exploring High-Dimensional Spacing through Backward
Multidimensional Projection
This code is an implementation of the work described by Amorin et al in their paper. The pdf is available here [[http://www.lcad.icmc.usp.br/~nonato/pubs/iLAMP-VAST.pdf](pdf)]

## LAMP: Local Affine Multidimensional Projection
This script is responsible to convert multidimensional data into 2d data.
You must set an environment variable called ILAMP_LAMP_SCRIPT. This variable points to **lamp.py** script filepath.
Also, for this script to work, you have to install mppy package into you python distribution. More information in https://github.com/thiagohenriquef/mppy

## ILamp Unity Component
[![Cubes](https://github.com/diegomazala/ilamp/blob/master/doc/imp_cube.gif?raw=true)]

![Unity Component](https://github.com/diegomazala/ilamp/blob/master/doc/imp_unity_behaviour.png?raw=true)

* Imp Type: ILamp or Rbf
* Run Lamp: Force to run LAMP algorithm and generate a new 2d file
* Filename: The file's names that will be generated and loaded
* Base Meshes: The meshes used to blend in between. Be carefull to check if after Unity import thoses they still have the same vertex count
* ILamp Parameters: used to configure ilamp algorithm. It's recommended to use the number of neighbours equal to the number of meshes in order to avoid "jumps" when hovering the mouse over the screen and blending the meshes
* Wireframe: A material to show polygons of the mesh
* Template Mesh: The template model that will receive the resultant mesh


## Video (click on the image)

* Video extracted from Unity scene Imp_Figurantes
[![Video](https://github.com/diegomazala/ilamp/blob/master/doc/ilamp_video_frame.png?raw=true)](https://vimeo.com/287528984)




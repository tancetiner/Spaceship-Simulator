# Spaceship Simulator
A 3D game project that player controls a spaceship around space and gets points by getting the target cube and destroying the meteors. 

### Table of contents

* [Introduction](#introduction)
* [Functionality and Techniques](#functionality-and-techniques)
* [Collaboration](#collaboration) 
* [Commits](#commits)
* [Gameplay](#gameplay)
* [Controls](#controls)


### Introduction
This was the term project for our course "Cs405 - Computer Graphics" in Sabanci University given by professor Selim Balcisoy. I prepared this project according to the requirements and deadlines of this course.\
I used C++ and OpenGL library to create the game. 

### Functionality and Techniques
I was required to implement some basic functionality and some advanced techniques.\
\
Basic functionalities are as following:
* 3D viewing and objects
* User input
* Lighting and smooth shading
* Texture mapping

And there were some advanced techniques such as:
* 2D rendering = To show the point and healht information to user
* Collision Detection = For checking the collisions between ship-planet, ship-meteor, meteor-borderBox, ship-borderBox
* Basic Animation = There are basic animations for when ship fires the meteor and ship gets the target cube. 

### Collaboration
I worked individually for this project, but throughout the semester we had recitations where our teaching assistant Alp Cihan teached us some practical concepts about OpenGL.\
As we learn more concepts he was providing us template libraries to use for our projects. I added a note for that at the top of the code in mentioned header files.

### Commits
Throughout the semester I worked on the project on my local machine and when it's finished and graded, I decided to add it to my Github profile. That's why it consists the initial commit only.

### Gameplay
The game has a fairly easy gameplay. We have a spaceship to control (see controls section) and we gain points by either getting the 'target cube' (2 points) or destroying the meteors by firing them (1 point).\
In that time, we also need to not hit the planets (game just restarts) or the meteors flying around (losing 1 health bar). 
Player have 5 health bar. If player reaches 12 points, player wins! If player run out of health bar, a game over text is shown and game resets itself.

### Controls
W     --> Accelerate\
S     --> Break\
A     --> Rotate Left\
D     --> Rotate Right\
Space --> Fire\



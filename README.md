# Second Animation - The Pipeline


This is the first example of how to use the glm library to set up the modelview, projection, and lookat matrices and how to use the shader to apply them to your vertices.  In the first animation we used a univorm variable (offset).  In this example we are using uniform 4x4 matrices.  I tried to add comments where things get interesting.

As before, compile with

```
g++ main.cpp -lGL -lGLEW -lglfw
```



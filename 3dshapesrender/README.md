# 3D Shapes Render

This project renders various 3D shapes in the terminal. It mainly applies knowledge from Graphics, Calculus, and Linear Algebra to output the projection of the 3D shape onto a 2D screen.
Key concepts include:
  - Graphics:
    - Z Buffer
    - Frame Buffer
    - Luminance
  - Calculus:
    - Equations of various shapes
    - Multivariable functions
  - Linear Algebra:
    - Rotation Matrices
    - Matrix Operations
    - Vector Operations

## Torus (Donut)

The torus is rendered using a luminance calculation to create a more realistic lighting effect. The luminance is calculated based on the dot product of the surface normal and the light source direction. The points on the surface of the torus are rendered with a variety of possible characters. The characters were chosen based on how many pixels make up the character. For example, the higher the luminance, the denser the character used to render the point (characters like &#@). Whereas for lower luminance values, the characters used are less dense (characters like .,-) This creates a shading effect that makes the torus appear more three-dimensional.

![Rotating Torus](./torus.gif)

## Cube

![Rotating Cube](./cube.gif)

## Octahedron

![Rotating Octahedron](./octahedron.gif)

## Pyramid

![Rotating Pyramid](./pyramid.gif)


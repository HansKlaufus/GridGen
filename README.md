# Grid Generation Techniques

In the recent months the program GridGen has been developed for generating grids around a virtual aerofoil placed in a virtual windtunnel. The program can be considered as a pre-processor for any existing Navier-Stokes solver, which can solve the actual pressure distributions and velocity fields around the aerofoil.

The program has been written in the C programming language using a modular approach. This modular structure makes the program maintainable and suitable for future extensions (e.g. implementation of a Delaunay triangulation scheme).

The input of the program is an ASCII file describing the aerofoil geometry, the windtunnel geometry and control parameters for the distribution type and relaxation.

The program is able to produces output in two formats:
* ASCII files suitable for the GNUPlot software package.
* ASCII files suitable for the VIGIE software package.

The program is suitable for generating meshes of the following kinds:
* Algebraic meshes based on transfinite interpolation.
* Elliptic meshes based on the Laplace equation (without any source function).
* Elliptic meshes based on the Middlecoff scheme.
* Unstructured meshes by destructuring an elliptic grid generated using the Middlecoff scheme.

The resulting grids are of good quality and therefore suitable for further usage with any solver to calculate two-dimensional viscous flows around given aerofoils.

# Phase Field Model Simulation of Dendritic Growth
$$
  \tau  \partial_t \phi = \nabla \left( W \nabla \phi \right) + \sum_{w=x,y} \partial_w \left(\left|\nabla \phi\right|^2 W \frac{\partial W}{\partial \left(\partial_w \phi\right)}\right) + \phi - \phi^3 - \lambda \left(\phi - \phi^2\right) u
$$

## Requirements
- `CMake`
- `Ninja`
- C++ Compiler
  - `C++17`
- OpenMP
- Nlohmann JSON
- libtensor
  - https://github.com/MaterialsModellingLab/libtensor.git

## Build & Install
This only install header-only library for Phase Field Model Simulation.

```shell
cmake --preset release
cmake --build build/release
cmake --install build/release
```

## Run
```shell
./build/release/phase_field_2d --verbose
```

![result](./media/phase_field_2d.gif)



## Build VTK converter tools
If you want to convert the output data to VTK format, you need to build the converter tools. Note that you will need to install the VTK library.

```shell
cmake --preset tools
cmake --build build/tools
```

## Run VTK converter tools
```shell
# Converted reults are stored in `./vtk` directory
ls ./output/*dat | xargs -IXXX ./build/tools/dat2vtk_2d --input XXX
```

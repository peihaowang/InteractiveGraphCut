# InteractiveGraphCut

## Introduction

We implement a graph-cut based algorithm for object and background segmentation given prior seeds, which was proposed by Y. Boykov et al. We build this algorithm into a callable library with handy user interfaces, both static and dynamic linked libraries are provided.

*InteractiveGraphCut* requires a prior input which labels a subset of pixels, as known as seeds. The algorithm will produce the mask of foreground object through these seeds. That's why the algorithm is called "interactive". The following illustration gives an intuition of how graph-cut works.

| Input | Output | Mask |
|:--------------:|:--------------:|:--------------:|
| ![Input](/showcases/input.jpg?raw=true) | ![Output](/showcases/output.jpg?raw=true) | ![Mask](/showcases/mask.jpg?raw=true)

The typical graph-cut is an energy optimization method. In this work,  we basically convert grided images into bidirected-connected graphs, and assign the weights to edges. These weights mainly indicate the coherence between two neighbor pixels. Then we add one source vertex and one sink vertex. All pixels will be connected with both terminals. These two terminals can be regarded as two labels, i.e. foreground and background. The weights of these links are computed by the prior input, indicating how likely each pixel can be labeled by each terminal. The segmentation problem is therefore converted into a graph-cut problem. We adopt Dinic algorithm to solve the max-flow.

## Installation

Before configure and compile *InteractiveGraphCut*, you may need to setup the dependencies `OpenCV` first. Refer to [Installation for Windows](https://docs.opencv.org/3.4/d3/d52/tutorial_windows_install.html) or [Installation for Linux](https://docs.opencv.org/3.3.0/d7/d9f/tutorial_linux_install.html) for installation of `OpenCV`. `OpenCV` is required mainly for image I/O and  representation.

You may note that, here are some convenient package manager which could help you install them directly. After configure these dependencies, you can clone this repository by the following command:

```
git clone https://github.com/peihaowang/InteractiveGraphCut.git
```

Afterward, run `CMake` to configure the solution and start to build the generated project. In unix-like systems, you can use the following commands to make it:

```
mkdir build && cd build
cmake ..
make -j8
make install
```

Here are some troubleshoots you may need to pay attention:

1. During configuring, dependency missing errors may occur, for `CMake` failes to find the installed library. The quickest solution is to create `3rdparty/lib` directory under the project folder and then copy the corresponding library files into it.

2. Upgrading `CMake` or compilers could be a good choice if there occurs wired issues while you are configuring or compiling the project.

3. While running `make install`, `CMake` installs built libraries and executables into the system directories by default. To custom the installation path, please specify the argument `-DCMAKE_INSTALL_PREFIX`. See [CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html) for details.

## Usage

### Library

*PoissonImageEditing* is released under `LGPL` license. Both shared and static libraries are available for end users. After building and installing *PoissonImageEditing*, you could include the header `PoissonImage.h` and link binary files in your own project. The basic usages of provided APIs are presented as follows:

```
bool ::seamlessClone(cv::InputArray src, cv::InputArray dst, cv::InputArray mask
        , const cv::Point& offset, cv::OutputArray output
        , GradientScheme gradientSchm = GradientScheme::Maximum
        , DiffOp gradientOp = DiffOp::Backward
        , DiffOp divOp = DiffOp::Forward
        , PerfMetric* perfMetric = nullptr
);
```

1. It requires source and destination images as inputs and a target image on which output the final result.

2. Mask is provided to tell the algorithm rough boundaries of the objects in source image(Hence, their sizes should be the same).

3. The source image is initially put on the center of the destination image, but offset means that it is possible to move the source objects on the destination canvas by giving a translation.

4. The following three parameters tweak several steps in the algorithm slightly, where `Gradient Scheme` specifies the blending policy to the gradient fields of source and destination images, whose differences as shown below. The other two parameters specifies which discrete differential operator will be taken for gradient field and `div` operator respectively.

| Naive | Replace | Average | Maximum |
|:--------------:|:--------------:|:----------------:|:----------------:|
| ![Naive](/showcases/case3/naive.jpg?raw=true) | ![Replace](/showcases/case3/replace.jpg?raw=true) | ![Average](/showcases/case3/average.jpg?raw=true) | ![Maximum](/showcases/case3/maximum.jpg?raw=true) |

5. The retriever of performance metric is an optional parameter, which measures the running time of each stage in the algorithm. Leave the pointer null to ignore the performance metric.

## Executable

The sample code can be compiled into a command-line utility, which has basic features to seamlessly combine two images together. To run the executable appropriately, read the following usages please:

```
Usage: PoissonImageEditor <source-path> <destination-path> <output-path>
        [ <mask-path> [ <x-offset> <y-offset> [ <-r|-a|-m> [ <-b|-f|-c> <-b|-f|-c> ] ] ] ]
```

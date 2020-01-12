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

Both shared and static libraries of *InteractiveGraphCut* are available for end users. After building and installing *InteractiveGraphCut*, you use it simply by including the header `GraphCut.h` and link binary files into your own project. The basic usages of provided APIs are presented as follows:

```
static void cutImage(cv::InputArray image, cv::OutputArray result, cv::OutputArray mask
    , const std::vector<cv::Point>& foreSeeds, const std::vector<cv::Point>& backSeeds
    , float lambda = 0.01f, float sigma = -1.0, PerfMetric* perfMetric = nullptr
);
```

1. The API requires one input image and two output image, where `result` will be the image of foreground object and `mask` will contain a binary bitmap.

2. Both `foreSeeds` and `backSeeds` are the user-specified prior labels mentioned before. They are both a subset of pixel coordinates. And they must not share an intersection.

3. Two hyperparameters `lambda` and `sigma` are provided to tune *InteractiveGraphCut*. `lambda` is known as the coefficient of the region properties term. `sigma` is the variance applied in smooth penalty, usually computed from the input image by default. Leaving both parameters below 0 indicates the default value.

4. The retriever of performance metric `perfMetric` is an optional parameter, given to measure running time in each computational step. Leave `nullptr` to ignore it.

### Executable

The sample code can be compiled into a command-line utility, which has basic features to do image segmentation. Before you feed the target image into *InteractiveGraphCut*, you may need to create a 3-channel prior seed image with the identical size to your input, where red pixel labels foreground, greed pixel labels background and black for the remaining pixels. To run the executable appropriately, read the following usages please:

```
Usage: GraphCutter <input image> <input seed> <output path> <mask path>
```

## References

1. [Boykov, Yuri Y., and M-P. Jolly. "Interactive graph cuts for optimal boundary & region segmentation of objects in ND images." Proceedings eighth IEEE international conference on computer vision. ICCV 2001. Vol. 1. IEEE, 2001.](https://cs.uwaterloo.ca/~yboykov/Abstracts/iccv01-abs.html)
2. [Boykov, Yuri, and Vladimir Kolmogorov. "An experimental comparison of min-cut/max-flow algorithms for energy minimization in vision." IEEE Transactions on Pattern Analysis & Machine Intelligence 9 (2004): 1124-1137.](https://cs.uwaterloo.ca/~yboykov/Abstracts/pami04-abs.shtml)
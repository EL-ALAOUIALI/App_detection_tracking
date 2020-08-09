# Object detection using Hog descriptor and Svm classifier
The code in this file is divided into 3 parts:
## Part 1: Implement HOG (histogram of oriented gradient)  decriptor for an image following by the procedure below
- Compute the gradient of the image, orientation ranges from 0 - 180
- Then the image is divided to small cells
- Build 9-bin histogram of each cell. Build gradient vector of each cell.
- Cells are grouped into blocks. Blocks can overlap each other.
- Build gradient vector for each block by simply concatenating gradient vectors of its cells.
- Nomorlize the vector to make it invariant to light condition
- Concatenate all block vectors to build a gradient vector to the image
## Part 2: Visualize HOG descriptor
- Histogram of orientation is expressed as as star-shape where each line has the orientation associated to the bin it
represents and the length of the line is proportional to that bin count.

## Part 3: Use OpenCv built-in detector to find a person on the scene

Code reference: Opencv 3 computer vision application (Robert Laganiere)



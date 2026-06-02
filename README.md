# Dendritic Spine Analysis

A C++ image processing project for the automatic detection and morphological analysis of dendritic spines from microscopic neuron images.

## Overview

This project implements a complete pipeline for dendritic spine analysis, including:

- Global and adaptive image thresholding
- Morphological filtering and noise removal
- Connected component analysis using BFS
- Distance Transform computation
- Skeletonization using the Zhang-Suen thinning algorithm
- Detection of endpoints and junction points
- Dendritic trunk extraction through iterative pruning
- Dendritic spine segmentation
- Morphological measurements and statistical analysis

## Processing Pipeline

1. Image binarization (global or adaptive thresholding)
2. Morphological operations for noise reduction
3. Connected component filtering
4. Distance Transform computation
5. Skeleton extraction using Zhang-Suen thinning
6. Skeleton point classification (endpoints and junctions)
7. Dendritic trunk extraction
8. Spine isolation and segmentation
9. Morphological feature extraction

## Extracted Metrics

The application computes several morphological characteristics of dendritic spines:

- Total spine count
- Spine length
- Mean spine length
- Length standard deviation
- Spine head width estimation
- Mean head width
- Head width standard deviation
- Spine length histogram

## Algorithms Used

- Adaptive Thresholding with Integral Image (Summed Area Table)
- Breadth First Search (BFS)
- Distance Transform
- Zhang-Suen Skeletonization
- Connected Component Labeling
- Morphological Opening (Erosion + Dilation)

## Applications

This project can be used for neuroscience research involving:

- Dendritic morphology analysis
- Synaptic connectivity studies
- Automated spine quantification
- Microscopic image analysis

## Author

Rares Ceuta

# SoftSysProject

## Functionality
Our code allows a user to create multiple balls of various sizes and starting velocities. As the `GameLoop` runs, these balls will glide around the screen, looking for situations in which a ball touches or overlaps either another ball or the edge of the screen. In such a situation, the ball will bounce back accordingly.

Video Demonstration: https://www.youtube.com/watch?v=vrl5L5qtZg8

## User Guide
To install freeglut, use `sudo apt-get freeglut3-dev`.
To compile, use the `make` command from the root directory.
To run, use the `bin/BallBouncer` command.
To do both, use `make && bin/BallBouncer`.

## Architecture Overview

Our code follows a component based architecture where a component encapsulates a single behavior. The components implemented here are `CircleRender`, `Physics`, `WallBounceScript`, and `Collider`. Instances of these components can be initialized and passed a `GameObject* parent` parameter to modify the behavior of that `parent`. See UML diagram in "SoftSysArchitecture.png".

#### `GameObject`
An instance of the `GameObject` class contains only an `x` and `y` coordinate and displays no behavior until assigned as `parent` to a component.

#### `Component`
All of the implemented components, `CircleRender`, `Physics`, `WallBounceScript`, and `Collider`, inherit their basic structure from the `Component` class. The `Component` class itself simply initializes generic update functions that are customized and overwritten when a specific component inherits from the `Component` class.

#### `CircleRender`
This behavior allows a circle with a specified `radius` to be displayed with its center located at the `x` and `y` coordinates of its `parent`.

#### `Physics`
This behavior assigns a velocity that will update the `x` and `y` coordinates of its `parent` as the `GameLoop` runs.

#### `WallBounceScript`
This behavior checks at each iteration of the `GameLoop` for circles touching the window boundaries. When a circle is found touching one of these boundaries, `WallBounceScript` modifies that circle's velocity to direct it away from that boundary.

#### `Collider`
This behavior detects when two circles are touching or overlapping and modifies their velocites to simulate two physical balls bouncing off of one another.

#### `The Gameloop`
We use an implementation of an on-demand gameloop. This loop tries to ensure components such as physics are run at a constant rate (default is 60 Hz) at the cost of other updates such as rendering. This makes it so that computers with different graphics and compute cabability will still simulate the game the same. 

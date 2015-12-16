# SoftSysProject

## Functionality
blah de blah de blah de blah

## User Guide
To compile, use the `make` command from the root directory.
To run, use the `bin/BallBouncer` command.
To do both, use `make && bin/BallBouncer`.

## Architecture Overview

Our code follows a component based architecture where a component encapsulates a single behavior. The components implemented here are `CircleRender`, `Physics`, `WallBounceScript`, and `Collider`. Instances of these components can be initialized and passed a `GameObject* parent` parameter to modify the behavior of that `parent`.

#### `GameObject`
An instance of the `GameObject` class contains only an `x` and `y` coordinate and displays no behavior until assigned as `parent` to a component.

#### `Component`
blah de blah de blah de blah

#### `CircleRender`
This behavior allows a circle to be displayed with its center located at the `x` and `y` coordinates of its `parent`.

#### `Physics`
This behavior assigns a velocity that will update the `x` and `y` coordinates of its `parent` as the `GameLoop` runs.

#### `WallBounceScript`
This behavior checks at each iteration of the `GameLoop` for circles touching the window boundaries. When a circle is found touching one of these boundaries, `WallBounceScript` modifies that circle's velocity to direct it away from that boundary.

#### `Collider`
This behavior detects when two circles are touching or overlapping and modifies their velocites to simulate two physical balls bouncing off of one another.
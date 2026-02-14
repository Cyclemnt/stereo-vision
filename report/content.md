# Content to include in the project report

## Camera hardware

Set cameras brightness, disable auto-exposure ... (Using OpenCV, or else using bash scripts with v4l2-ctl)

## Stereo setup

In the stereo configuration, make sure the two images are taken at the same time.
When wired through a USB hub, use a hub with external power source, otherwise the cameras will fail at taking images simultaneously.
Even with tight cam->grab(), there still is a time difference between the two pictures (see video taken on Fab 14th)
So, let's use multithreading : 
    - Two threads = Two cameras
    - They capture continuously, starting from the same exact time std::this_thread::sleep_until
    - The thread 

## OS considerations

As the program need to interact with physical hardware (USB cameras), it might very easily brake when not using Linux/UNIX.

## OpenCV

## Open3D
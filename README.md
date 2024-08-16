# CANLight
## following a certain CAN standard, used the [toomoss](http://www.toomoss.cn/api/index.html) api
Purpose: 6 lights representing the 5 gear positions and the P pressed gear. Also counts the total amount of frames received from each light
updated: added buttons that represented the real gears, interpreted based on gear positions, and angle interpretations
Potential Bug: When switching between the lights, the sender will send a couple of frames of Neutral, which might cause some problems.
Note: provided in this repo is all the signal processing logic doc (Chinese).

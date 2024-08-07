# CANLight
## following a certain CAN standard, repurposed the [toomoss](http://www.toomoss.cn/api/index.html) api
Purpose: 6 lights representing the 5 gears and the P pressed gear. Also counts the total amount of frames received from each light
Potential Bug: When switching between the lights, the sender will send a couple of frames of Neutral, which might cause some problems.

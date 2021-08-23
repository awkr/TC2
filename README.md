# TC2
A multiplayer parkour game based on Unreal Engine, inspired by Chrome dinosour.

# Operation
`W` `S` move forward & backward

`A` `D` move right & left

`Space` `Left` mouse button jump

# Snapshots

![SplashScreen](document/1-SplashScreen.png)
![Home](document/2-Home.png)
![SOLO](document/3-Solo.png)
![Multiplayer-1](document/4-Multiplayer-1.png)
![Multiplayer-2](document/4-Multiplayer-2.png)
![GameOver](document/5-GameOver.png)

# 操作感调校
1. Movement Lerp：typically axis value is -1, 0, 1. if directly set to `scale`, it will cause rapid changes of the character speed.
2. Gravity:
    - Jump rise & fall: Mario style
        ![Rise](document/Curve-1.png)
        ![Fall](document/Curve-2.png)
    - Impulse ( shock )
3. Moving dust: if the speed exceeds a certain threshold, there should be some environment react, such as dust, footsteps.
4. Camera shake

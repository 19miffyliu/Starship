# Description
- This is the Starship SD1-A4 assignment. The user can control a ship to move around using keyboard or Xbox controller, and fire bullets at asteroids/beetles/wasps to destroy them.
- This project requires the Engine dll library to work, please see the instructions in "Engine" project before trying to open this project in Visual Studio.


[Build instructions (with solution files)]
1. Open the "Starship.sln" solution in "Starship" folder using Visual Studio 2022.
2. Click "Build->Build Solution" on the top navigation bar.


# Run instructions
With exe file:
1. Double-click on the "Starship_Release_x64.exe" file.

With solution files:
1. Press "Control" + "F5" to run the solution without debugging.
3. Navigate to "Starship/Run" folder in file explorer.
4. Double-click on the "Starship_Release_x64.exe" file.


# Gameplay Description
There are 3 types of entity in the game: player ship, asteroid, and bullet.

At the start of the game, there are:
- 1 player ship (spawn at the center of the screen, facing right)
- 5 asteroids (floating at random direction with speed = 10 units/second)
- 3 beetles (move towards player with speed = 11 units/second)
- 2 wasps (circles around and gets closer to player with acceleration = 10 units/second)

Player ship life:
- Player ship has a total of 4 lives.
- Dying consumes 1 life.

Player ship will die if:
- collides with an asteroid/beetle/wasp.

Asteroid/beetle/wasp will die if:
- gets hit by bullets for three times.

Spawn Invincibility
- Player will have 3 seconds of invincibility when they respawn. The ship will become white, then gradually transitioning back to regular blue color.

Enemy Waves
- There are total of 5 enemy waves.
- Each wave comes with (number of previous wave * 1.5) of each type of enemies.
	- For example, the first wave has 5 asteroids, 3 beetles, 2 wasps, so the second wave will have 7 asteroids, 4 beetles, 3 wasps.
- When all beetles and wasps are eliminated by player, the next wave will spawn immediately.
- When the player beats all enemy waves, the player wins and the game goes back to the attract mode.


# Gameplay Controls
Keyboard:
- Press "N" in attract mode to start the game
- Press "E" to thrust toward where the ship is facing
- Press "S" to turn to the left and "F" to the right
- Press "space" to fire a bullet from the tip of the ship
- Hold "space" to perform rapidfire
- Press "R" to perform starburst
- Hold "shift" to enable slow-motion (can rotate with normal speed in slow-motion)
- When dead, press "N" to respawn the ship at the center of the screen
- Press "ESC" in game to go back to attract mode
- Press "ESC" in attract mode to exit the program

Xbox controller:
- Press "START" or "A" in attract mode to start the game
- Press "A" to fire a bullet from the tip of the ship
- Hold "A" to perform rapidfire
- Press "X" to perform starburst
- Move left joystick to turn to the left/right and thrust toward where the ship is facing
	- the more tilted joystick is, the more acceleration the ship gets
- When dead, press "START" to respawn the ship at the center of the screen
- Hold "RB" to enable slow-motion (can rotate with normal speed in slow-motion)

# Debug Controls
Player can:
- Press F1 to toggle debug drawing for all entities
- Press "I" to spawn an asteroid at random position offscreen
- Press "F8" to reset the game
- Press "P" to pause/un-pause the game
- Hold "T" to slow the rate of time to 10% normal, while held
- Press "O" to un-pause the game, allows one frame of Update to run, then pauses the game

# UI Description

Player life count
There is a player life count shown as number of ships at the top-left of the screen.
- Every time player presses 'N' to respawn, the count of the ship will decrease by one.

Enemy life count
There is an enemy health bar shown on top of beetles and wasps.
- The health bar will show the current health each enemy has.

Hit indication
Beetles and wasps will flash red if they got hit by a bullet.

Overheat Bar
There is a bar at the top-middle of the screen.
- Every time player fires a bullet, the bar increases.
- Rapidfire and Starburst will increase the heat dramatically.
- If the Overheat bar reaches 100%, player will not be able to perform rapid fire.
- If the Overheat bar is greater than 50%, player will not be able to perform starburst.

Minimap
There is a minimap at the top-right of the screen.
- The current visible screen is shown as white box inside a larger box(the world map).
- The asteroids are shown as grey triangles.
- The beetles are shown as green triangles.
- The wasps are shown as yellow triangles.


# Miscellaneous features

Thrust flame
- When thrusting, a flame will ignite at the back of the ship. The length and loudness of sound effects depend on the thrusting magnitude (more visible via controller).


Debris
- Debris will fly off an entity if:
	- they got hit by bullets (except the player ship, which does not receive damage from bullets)
	- they die

Starry background
- The map's background will have a number of stars that will move past player as they fly by.


SFXs
- There are sound effects for:
	i.	Player shoots bullet
	ii.	Player dies
	iii.	Player respawns
	iv.	Enemy dies (different per Entity type)
	v.	New enemy wave starts
	vi.	Game over – Victory!
	vii.	Game over – Defeat!
	viii.	Attract mode
	ix.	Gameplay mode
	x.	Player Thrusting ship forward (volume & pitch adjusted according to thrust magnitude)




# [Warnings
Only 30 wasps, 50 beetles, 100 asteroids, 200 bullets, and 300 debris are allowed to be in the world at once. If the player attempts to spawn more asteroid, an ERROR_RECOVERABLE dialogue will open and stop the player from spawning more.


# Known bugs
None


# Lonic vs RayKon | A Donkey Kong Clone
A Donkey Kong clone featuring the beloved RSGL mascot, Lonic. Made using RGFW and software rendering.

# goals 
A major goal for this project is to make a game handled in a 'simple' way. There is no game state. All the game 'knows' about is where it's information about the player and the barrels. 

1) Collision is handled by checking if a sprite draw replaced another color.
2) All sprite are stored as static bitmap arrays.
3) The map is a bitmap that is created at startup.
